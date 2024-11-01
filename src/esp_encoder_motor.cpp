#include "esp_encoder_motor.h"

#include <Arduino.h>

#include <thread>

#include "driver/gpio.h"
#include "driver/ledc.h"

namespace emf {

namespace {
constexpr float kDefaultPositionP = 20.0;
constexpr float kDefaultPositionI = 0.1;
constexpr float kDefaultPositionD = 5.0;
constexpr float kDefaultSpeedP = 15.0;
constexpr float kDefaultSpeedI = 4.0;
constexpr float kDefaultSpeedD = 1.0;
}  // namespace

EncoderMotor::EncoderMotor(const uint8_t pos_pin,
                           const uint8_t neg_pin,
                           const uint8_t a_pin,
                           const uint8_t b_pin,
                           const uint32_t ppr,
                           const uint32_t reduction_ration,
                           const bool is_a_head_in_forward)
    : a_pin_(a_pin),
      b_pin_(b_pin),
      motor_driver_(pos_pin, neg_pin),
      total_ppr_(ppr * reduction_ration),
      b_level_at_a_falling_edge_(is_a_head_in_forward ? HIGH : LOW) {
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

  pinMode(a_pin_, INPUT_PULLUP);
  pinMode(b_pin_, INPUT_PULLUP);

  attachInterruptArg(a_pin_, EncoderMotor::OnPinAFalling, this, FALLING);
  active_ = true;
  xTaskCreate(&EncoderMotor::Loop, "loop", 4096, this, 0, nullptr);
}

void EncoderMotor::RunPwm(const int16_t duty) {
  std::lock_guard<std::mutex> l(mutex_);
  mode_ = kPwmMode;
  motor_driver_.Duty(duty);
}

void EncoderMotor::RunSpeed(const int16_t rpm) {
  std::lock_guard<std::mutex> l(mutex_);
  if (target_speed_ == rpm && mode_ == kSpeedMode) {
    return;
  }

  mode_ = kSpeedMode;
  target_speed_ = rpm;

  if (fabs(pid_speed_.i) > __FLT_EPSILON__) {
    pid_speed_.integral = motor_driver_.Duty() / pid_speed_.i;
  }
}

void EncoderMotor::Stop() {
  std::lock_guard<std::mutex> l(mutex_);
  mode_ = kNoneMode;
  motor_driver_.Break();
  pid_speed_.integral = 0;
}

int64_t EncoderMotor::EncoderPulseCount() const {
  return current_pulse_count_;
}

int32_t EncoderMotor::Speed() const {
  return current_speed_;
}

int16_t EncoderMotor::PwmDuty() const {
  std::lock_guard<std::mutex> l(mutex_);
  return motor_driver_.Duty();
}

void EncoderMotor::OnPinAFalling(void* self) {
  reinterpret_cast<EncoderMotor*>(self)->OnPinAFalling();
}

void EncoderMotor::Loop(void* self) {
  reinterpret_cast<EncoderMotor*>(self)->Loop();
}

void EncoderMotor::OnPinAFalling() {
  if (gpio_get_level(static_cast<gpio_num_t>(b_pin_)) == b_level_at_a_falling_edge_) {
    ++current_pulse_count_;
  } else {
    --current_pulse_count_;
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

  if (duration < 20.0) {
    return;
  }

  const double current_pulse_position = current_pulse_count_;
  current_speed_ = (current_pulse_position - previous_pulse_count_) * 60000.0 / duration / total_ppr_;
  previous_pulse_count_ = current_pulse_position;
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

  if (now - last_update_pwm_time_ < 20) {
    return;
  }

  const float speed_error = constrain(target_speed_ - current_speed_, -20, 20);
  pid_speed_.integral = constrain(pid_speed_.integral + speed_error, -pid_speed_.max_integral, pid_speed_.max_integral);
  const int16_t duty = round(pid_speed_.p * speed_error + pid_speed_.i * pid_speed_.integral);
  motor_driver_.Duty(duty);
  last_update_pwm_time_ = now;
}

}  // namespace emf