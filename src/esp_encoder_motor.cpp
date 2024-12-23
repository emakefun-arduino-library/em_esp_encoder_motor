#include "esp_encoder_motor.h"

#include <Arduino.h>

#include <thread>
#include <utility>

#include "driver/gpio.h"
#include "driver/ledc.h"

namespace em {

namespace {
constexpr float kDefaultPositionP = 20.0;
constexpr float kDefaultPositionI = 0.1;
constexpr float kDefaultPositionD = 5.0;
constexpr float kDefaultSpeedP = 3.0;
constexpr float kDefaultSpeedI = 1.0;
constexpr float kDefaultSpeedD = 1.0;
constexpr int32_t kDeadRpmZone = 10;
}  // namespace

EspEncoderMotor::EspEncoderMotor(const uint8_t pin_positive,
                           const uint8_t pin_negative,
                           const uint8_t pin_a,
                           const uint8_t pin_b,
                           const uint32_t ppr,
                           const uint32_t reduction_ration,
                           const PhaseRelation phase_relation)
    : pin_a_(pin_a),
      pin_b_(pin_b),
      motor_driver_(pin_positive, pin_negative),
      total_ppr_(ppr * reduction_ration),
      b_level_at_a_falling_edge_(phase_relation == PhaseRelation::kAPhaseLeads ? HIGH : LOW) {
  rpm_pid_.p = kDefaultSpeedP;
  rpm_pid_.i = kDefaultSpeedI;
  rpm_pid_.d = kDefaultSpeedD;
  rpm_pid_.max_integral = ceil(EspMotor::kMaxPwmDuty / rpm_pid_.i);
}

void EspEncoderMotor::Init() {
  std::lock_guard<std::mutex> l(mutex_);
  if (update_rpm_thread_ != nullptr) {
    return;
  }

  motor_driver_.Init();

  pinMode(pin_a_, INPUT_PULLUP);
  pinMode(pin_b_, INPUT_PULLUP);

  attachInterruptArg(pin_a_, EspEncoderMotor::OnPinAFalling, this, FALLING);
  update_rpm_thread_ = new std::thread(&EspEncoderMotor::UpdateRpm, this);
}

void EspEncoderMotor::SetSpeedPid(const float p, const float i, const float d) {
  std::lock_guard<std::mutex> l(mutex_);
  rpm_pid_.p = p;
  rpm_pid_.i = i;
  rpm_pid_.d = d;

  if (i > 0) {
    rpm_pid_.max_integral = ceil(EspMotor::kMaxPwmDuty / rpm_pid_.i);
  } else {
    rpm_pid_.max_integral = 0;
  }
}

void EspEncoderMotor::GetSpeedPid(float* const p, float* const i, float* const d) {
  std::lock_guard<std::mutex> l(mutex_);
  if (p != nullptr) {
    *p = rpm_pid_.p;
  }
  if (i != nullptr) {
    *i = rpm_pid_.i;
  }
  if (d != nullptr) {
    *d = rpm_pid_.d;
  }
}

EspEncoderMotor::~EspEncoderMotor() {
  std::unique_lock<std::mutex> lock(mutex_);
  DeleteThread(driving_thread_);
  DeleteThread(update_rpm_thread_);
}

void EspEncoderMotor::RunPwmDuty(const int16_t duty) {
  std::unique_lock<std::mutex> lock(mutex_);
  DeleteThread(driving_thread_);
  target_speed_rpm_ = 0;

  if (motor_driver_.PwmDuty() == duty) {
    return;
  }

  motor_driver_.PwmDuty(duty);
}

void EspEncoderMotor::RunSpeed(const int16_t speed_rpm) {
  std::lock_guard<std::mutex> l(mutex_);
  if (driving_thread_ == nullptr) {
    rpm_pid_.integral = 0;
    driving_thread_ = new std::thread(&EspEncoderMotor::Driving, this);
  }

  if (speed_rpm == target_speed_rpm_) {
    return;
  }

  target_speed_rpm_ = speed_rpm;
  drive_ = true;
  condition_.notify_all();
}

void EspEncoderMotor::Stop() {
  std::unique_lock<std::mutex> lock(mutex_);
  DeleteThread(driving_thread_);
  motor_driver_.Stop();
  target_speed_rpm_ = 0;
  rpm_pid_.integral = 0;
}

int64_t EspEncoderMotor::EncoderPulseCount() const {
  return pulse_count_;
}

int32_t EspEncoderMotor::SpeedRpm() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return speed_rpm_;
}

int16_t EspEncoderMotor::PwmDuty() const {
  std::lock_guard<std::mutex> l(mutex_);
  return motor_driver_.PwmDuty();
}

int32_t EspEncoderMotor::TargetRpm() const {
  std::lock_guard<std::mutex> l(mutex_);
  return target_speed_rpm_;
}

void EspEncoderMotor::OnPinAFalling(void* self) {
  reinterpret_cast<EspEncoderMotor*>(self)->OnPinAFalling();
}

void EspEncoderMotor::OnPinAFalling() {
  if (gpio_get_level(static_cast<gpio_num_t>(pin_b_)) == b_level_at_a_falling_edge_) {
    ++pulse_count_;
  } else {
    --pulse_count_;
  }
}

void EspEncoderMotor::UpdateRpm() {
  std::unique_lock lock(mutex_);
  last_update_speed_time_ = std::chrono::system_clock::now();
  while (!condition_.wait_until(
      lock, last_update_speed_time_ + std::chrono::milliseconds(50), [this]() { return update_rpm_thread_ == nullptr; })) {
    const auto now = std::chrono::system_clock::now();
    const double duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update_speed_time_).count();
    const double pulse_count = pulse_count_;
    speed_rpm_ = (pulse_count - previous_pulse_count_) * 60000.0 / duration / total_ppr_;
    previous_pulse_count_ = pulse_count;
    last_update_speed_time_ = now;
    if (driving_thread_ != nullptr) {
      drive_ = true;
      condition_.notify_all();
    }
  }
}

void EspEncoderMotor::Driving() {
  std::unique_lock lock(mutex_);

  while (driving_thread_ != nullptr) {
    condition_.wait(lock, [this]() { return driving_thread_ == nullptr || drive_; });

    if (driving_thread_ == nullptr) {
      return;
    }

    if (target_speed_rpm_ < kDeadRpmZone && target_speed_rpm_ > -kDeadRpmZone) {
      motor_driver_.PwmDuty(0);
    } else {
      const float speed_error = target_speed_rpm_ - speed_rpm_;
      rpm_pid_.integral = constrain(rpm_pid_.integral + speed_error, -rpm_pid_.max_integral, rpm_pid_.max_integral);
      const int16_t duty = round(rpm_pid_.p * speed_error + rpm_pid_.i * rpm_pid_.integral);
      motor_driver_.PwmDuty(duty);
    }
    drive_ = false;
  }
}

void EspEncoderMotor::DeleteThread(std::thread*& thread) {
  if (thread != nullptr) {
    const auto temp = std::exchange(thread, nullptr);
    condition_.notify_all();
    mutex_.unlock();
    temp->join();
    delete temp;
    mutex_.lock();
  }
}

}  // namespace em