#include "esp_encoder_motor.h"

#include <Arduino.h>

#include <thread>

#include "driver/gpio.h"
#include "driver/ledc.h"

namespace em {

namespace {
constexpr float kDefaultPositionP = 20.0;
constexpr float kDefaultPositionI = 0.1;
constexpr float kDefaultPositionD = 5.0;
constexpr float kDefaultSpeedP = 15.0;
constexpr float kDefaultSpeedI = 4.0;
constexpr float kDefaultSpeedD = 1.0;
}  // namespace

EncoderMotor::EncoderMotor(const uint8_t pin_positive,
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
  //   pid_position_.p = kDefaultPositionP;
  //   pid_position_.i = kDefaultPositionI;
  //   pid_position_.d = kDefaultPositionD;

  pid_speed_.p = kDefaultSpeedP;
  pid_speed_.i = kDefaultSpeedI;
  pid_speed_.d = kDefaultSpeedD;
  pid_speed_.max_integral = ceil(MotorDriver::kMaxPwmDuty / pid_speed_.i);
}

void EncoderMotor::Init() {
  std::lock_guard<std::mutex> l(mutex_);
  if (active_) {
    return;
  }

  motor_driver_.Init();

  pinMode(pin_a_, INPUT_PULLUP);
  pinMode(pin_b_, INPUT_PULLUP);

  attachInterruptArg(pin_a_, EncoderMotor::OnPinAFalling, this, FALLING);
  active_ = true;
  xTaskCreate(&EncoderMotor::Loop, "loop", 4096, this, 0, nullptr);
}

void EncoderMotor::RunPwmDuty(const int16_t duty) {
  std::lock_guard<std::mutex> l(mutex_);
  if (kPwmMode == mode_ && motor_driver_.PwmDuty() == duty) {
    return;
  }

  mode_ = kPwmMode;
  motor_driver_.PwmDuty(duty);
}

void EncoderMotor::RunSpeedRpm(const int16_t speed_rpm) {
  std::lock_guard<std::mutex> l(mutex_);
  if (speed_rpm == target_speed_rpm_ && kSpeedMode == mode_) {
    return;
  }

  mode_ = kSpeedMode;
  target_speed_rpm_ = speed_rpm;

  if (fabs(pid_speed_.i) > __FLT_EPSILON__) {
    pid_speed_.integral = motor_driver_.PwmDuty() / pid_speed_.i;
  }
}

void EncoderMotor::Stop() {
  std::lock_guard<std::mutex> l(mutex_);
  mode_ = kNoneMode;
  motor_driver_.Break();
  pid_speed_.integral = 0;
}

int64_t EncoderMotor::EncoderPulseCount() const {
  return pulse_count_;
}

int32_t EncoderMotor::SpeedRpm() const {
  return speed_rpm_;
}

int16_t EncoderMotor::PwmDuty() const {
  std::lock_guard<std::mutex> l(mutex_);
  return motor_driver_.PwmDuty();
}

void EncoderMotor::OnPinAFalling(void* self) {
  reinterpret_cast<EncoderMotor*>(self)->OnPinAFalling();
}

void EncoderMotor::Loop(void* self) {
  reinterpret_cast<EncoderMotor*>(self)->Loop();
}

void EncoderMotor::OnPinAFalling() {
  if (gpio_get_level(static_cast<gpio_num_t>(pin_b_)) == b_level_at_a_falling_edge_) {
    ++pulse_count_;
  } else {
    --pulse_count_;
  }
}

void EncoderMotor::Loop() {
  while (active_) {
    CalculateSpeed();

    mutex_.lock();
    const auto mode = mode_;
    mutex_.unlock();

    switch (mode) {
      case kSpeedMode: {
        UpdatePwmForSpeed();
        break;
      }
      default: {
        break;
      }
    }

    delay(5);
  }
}

void EncoderMotor::CalculateSpeed() {
  const uint64_t now = esp_timer_get_time() / 1000;
  if (last_update_speed_time_ == 0) {
    last_update_speed_time_ = now;
    return;
  }

  const double duration = now - last_update_speed_time_;

  if (duration < 50.0) {
    return;
  }

  const double pulse_count = pulse_count_;
  speed_rpm_ = (pulse_count - previous_pulse_count_) * 60000.0 / duration / total_ppr_;
  const int32_t speed = speed_rpm_;
  previous_pulse_count_ = pulse_count;
  last_update_speed_time_ = now;
}

void EncoderMotor::UpdatePwmForSpeed() {
  std::lock_guard<std::mutex> l(mutex_);
  if (mode_ != kSpeedMode) {
    return;
  }

  const uint64_t now = esp_timer_get_time() / 1000;
  if (last_update_pwm_time_ == 0) {
    last_update_pwm_time_ = now;
    return;
  }

  if (now - last_update_pwm_time_ < 50) {
    return;
  }

  const float speed_error = constrain(target_speed_rpm_ - speed_rpm_, -50, 50);
  pid_speed_.integral = constrain(pid_speed_.integral + speed_error, -pid_speed_.max_integral, pid_speed_.max_integral);
  const int16_t duty = round(pid_speed_.p * speed_error + pid_speed_.i * pid_speed_.integral);
  motor_driver_.PwmDuty(duty);
  last_update_pwm_time_ = now;
}

}  // namespace em