#include "motor_driver.h"

#include <Arduino.h>

namespace emf {

MotorDriver::MotorDriver(const uint8_t pos_pin, const uint8_t neg_pin) : pos_pin_(pos_pin), neg_pin_(neg_pin) {
}

void MotorDriver::Init() {
  ledcAttach(pos_pin_, kPwmFrequency, kPwmResolution);
  ledcAttach(neg_pin_, kPwmFrequency, kPwmResolution);
  Break();
}

void MotorDriver::Duty(const int16_t duty) {
  duty_ = constrain(duty, -kMaxPwmDuty, kMaxPwmDuty);
  if (duty_ >= 0) {
    ledcWrite(pos_pin_, duty_);
    ledcWrite(neg_pin_, 0);
  } else {
    ledcWrite(pos_pin_, 0);
    ledcWrite(neg_pin_, -duty_);
  }
}

int16_t MotorDriver::Duty() const {
  return duty_;
}

void MotorDriver::Break() {
  duty_ = 0;
  ledcWrite(pos_pin_, kMaxPwmDuty);
  ledcWrite(neg_pin_, kMaxPwmDuty);
}

void MotorDriver::Release() {
  duty_ = 0;
  ledcWrite(pos_pin_, 0);
  ledcWrite(neg_pin_, 0);
}

}  // namespace em