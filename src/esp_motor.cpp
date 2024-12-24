/**
 * @file esp_motor.cpp
 */

#include "esp_motor.h"

#include <Arduino.h>

namespace em {

EspMotor::EspMotor(const uint8_t pos_pin, const uint8_t neg_pin) : positive_pin_(pos_pin), negative_pin_(neg_pin) {
}

void EspMotor::Init() {
  ledcAttach(positive_pin_, kPwmFrequency, kPwmResolution);
  ledcAttach(negative_pin_, kPwmFrequency, kPwmResolution);
  Stop();
}

void EspMotor::PwmDuty(const int16_t pwm_duty) {
  pwm_duty_ = constrain(pwm_duty, -kMaxPwmDuty, kMaxPwmDuty);
  if (pwm_duty_ >= 0) {
    ledcWrite(positive_pin_, pwm_duty_);
    ledcWrite(negative_pin_, 0);
  } else {
    ledcWrite(positive_pin_, 0);
    ledcWrite(negative_pin_, -pwm_duty_);
  }
}

int16_t EspMotor::PwmDuty() const {
  return pwm_duty_;
}

void EspMotor::Stop() {
  pwm_duty_ = 0;
  ledcWrite(positive_pin_, kMaxPwmDuty);
  ledcWrite(negative_pin_, kMaxPwmDuty);
}

}  // namespace em