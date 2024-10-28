#pragma once

#include <cstdint>

#include "utils/motor_driver.h"

namespace em {
class EncoderMotor {
 public:
  /**
   * @brief Constructor.
   *
   * @param[in] pos_pin Motor driver positive pin
   * @param[in] neg_pin Motor driver negative pin
   * @param[in] a_pin Encoder channel A pin
   * @param[in] b_pin Encoder channel B pin
   * @param[in] ppr Encoder pulses per revolution
   * @param[in] ratio Gear reduction ratio
   * @param[in] is_a_head_ff  Indicates the level of signal B at the falling edge of signal A when motor is in forward rotation
   */
  EncoderMotor(const uint8_t pos_pin,
               const uint8_t neg_pin,
               const uint8_t a_pin,
               const uint8_t b_pin,
               const uint32_t ppr,
               const uint32_t reduction_ration,
               const bool is_a_head_ff);

  /**
   * @brief Initialize motor settings.
   *
   */
  void Init();

  /**
   * @brief Set motor PWM directly.
   *
   * @param[in] duty PWM duty cycle (-4095 to 4095)
   */
  void RunPwm(const int16_t duty);

  /**
   * @brief Run motor at speed setpoint.
   *
   * @param[in] speed Speed setpoint(RPM)
   */
  void RunSpeed(const int16_t rpm);

  /**
   * @brief Get encoder pulse count.
   *
   * @return int32_t Encoder pulses
   */
  int64_t EncoderPulseCount() const;

  /**
   * @brief Get the current speed of the motor.
   *
   * This function returns the current speed value of the motor in revolutions per minute (RPM).
   *
   * @return The current speed of the motor in RPM.
   */
  int32_t Speed() const {
    return current_speed_;
  }

  /**
   * @brief Get the PWM duty cycle of the motor driver.
   *
   * This function returns the PWM duty cycle set on the motor driver.
   *
   * @return The PWM duty cycle of the motor driver.
   */
  int16_t PwmDuty() const {
    return motor_driver_.Duty();
  }

 private:
  static void OnPinAFalling(void* self);

  static void Loop(void* self);

  void OnPinAFalling();

  void Loop();

  void CalculateSpeed();

  void UpdatePwmForSpeed();

  enum Mode {
    kNoneMode,
    kPwmMode,
    kSpeedMode,
    kPositionMode,
  };

  struct Pid {
    float p = 0.0;
    float i = 0.0;
    float d = 0.0;
    float integral = 0.0;
    float last_error = 0.0;
    float max_integral = 0.0;
  };

  MotorDriver motor_driver_;
  const uint8_t a_pin_ = 0;
  const uint8_t b_pin_ = 0;
  double total_ppr_ = 0;
  const uint8_t b_level_at_a_falling_edge_ = 0;
  Pid pid_speed_;
  bool active_ = false;
  Mode mode_ = kNoneMode;
  int32_t previous_pulse_count_ = 0;
  int64_t current_pulse_count_ = 0;
  uint64_t last_update_speed_time_ = 0;
  uint64_t last_update_pwm_time_ = 0;

  volatile int32_t current_speed_ = 0;

  float target_speed_ = 0.0;
};
}  // namespace em