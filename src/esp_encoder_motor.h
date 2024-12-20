#pragma once

#ifndef _EM_ESP_ENCODER_MOTOR_H_
#define _EM_ESP_ENCODER_MOTOR_H_

/**
 * @file esp_encoder_motor.h
 */

#include <WString.h>

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <thread>

#include "utils/motor_driver.h"

namespace em {
/**
 * @~Chinese
 * @class EncoderMotor
 * @brief 该类主要功能如下：
 * @details
 * -# 支持编码器输出信号为A、B两相且A、B两通道信号序列相位差为90度的编码电机。
 * -# 支持以指定的速度（RPM）驱动电机运转，或者以指定的PWM占空比直接驱动电机。
 * -# 支持获取电机当前的转速信息，单位为RPM。
 * -# 支持获取编码脉冲计数值，此计数值在A相下降沿进行更新，电机正转时计数值加1，反转时减1。
 * -# 支持获取电机驱动器当前设置的PWM占空比。
 * @example run_rpm.ino 以指定的转速（RPM）转动电机
 * @example run_pwm.ino 以指定的PMW占空比转动电机
 * @example forward_stop_backward.ino 电机前进后退停止
 * @example detect_phase_relation.ino
 * 将编码电机按照示例程序中的说明接入到指定位置，程序运行成功后，会根据电机正转时编码器AB相的实际相位关系，
 * 在串口打印输出是使用 @ref kAPhaseLeads 还是 @ref
 * kBPhaseLeads，以此帮助用户确定在创建EncoderMotor对象时phase_relation参数应设置的值。
 * @example run_rpm_with_analog_input.ino 依据特定 IO 口的模拟值动态设定电机转动速度
 */
/**
 * @~English
 * @class EncoderMotor
 * @brief The main functions of this class are as follows:
 * @details
 * -# Supports encoded motors with encoder output signals of A and B phases and a phase difference of 90 degrees
 * between the signal sequences of A and B channels.
 * -# Supports driving the motor at a specified speed (RPM) or directly driving the motor with a specified PWM duty cycle.
 * -# Supports obtaining the current speed information of the motor in RPM.
 * -# Supports obtaining the encoder pulse count value. This count value is updated at the falling edge of phase A, incremented
 * by 1 during forward rotation and decremented by 1 during reverse rotation.
 * -# Supports obtaining the PWM duty cycle currently set on the motor driver.
 * @example run_rpm.ino Rotate the motor at the specified speed (RPM).
 * @example run_pwm.ino Rotate the motor with the specified PWM duty cycle.
 * @example forward_stop_backward.ino Move the motor forward, stop it, and then move it backward.
 * @example detect_phase_relation.ino Connect the encoded motor to the specified positions as described in the example program.
 * After the program runs successfully, it will print out whether kAPhaseLeads or kBPhaseLeads should be used based on the
 * actual phase relationship between the A and B phases of the encoder during forward rotation of the motor, helping the user
 * determine the value that should be set for the phase_relation parameter when creating an EncoderMotor object.
 * @example run_rpm_with_analog_input.ino Dynamically set the rotation speed of the motor according to the analog value of the
 * specific I/O port.
 */
class EncoderMotor {
 public:
  /**
   * @~Chinese
   * @brief 主版本号。
   */
  /**
   * @~English
   * @brief Major version number.
   */
  static constexpr uint8_t kVersionMajor = 1;

  /**
   * @~Chinese
   * @brief 次版本号。
   */
  /**
   * @~English
   * @brief Minor version number.
   */
  static constexpr uint8_t kVersionMinor = 0;

  /**
   * @~Chinese
   * @brief 修订版本号。
   */
  /**
   * @~English
   * @brief Patch version number.
   */
  static constexpr uint8_t kVersionPatch = 0;

  /**
   * @~Chinese
   * @brief 用于明确电机正转时编码器AB相的相位关系，以便在脉冲计数及后续速度计算等操作中依据正确的相位关系进行处理。
   */
  /**
   * @~English
   * @brief Used to clarify the phase relationship between phase A and phase B of the encoder when the motor is rotating
   * forward, so that the correct phase relationship can be used in operations such as pulse counting and subsequent speed
   * calculation.
   */
  enum class PhaseRelation {
    /**
     * @~Chinese
     * @brief 表示电机正转时A相领先于B相。
     */
    /**
     * @~English
     * @brief Represents the situation where phase A leads phase B when the motor is rotating forward.
     */
    kAPhaseLeads,

    /**
     * @~Chinese
     * @brief 表示电机正转时B相领先于A相。
     */
    /**
     * @~English
     * @brief Represents the situation where phase B leads phase A when the motor is rotating forward.
     */
    kBPhaseLeads,
  };

  /**
   * @~Chinese
   * @brief 获取版本号字符串。
   * @return 版本号字符串，格式为 major.minor.patch。
   */
  /**
   * @~English
   * @brief Get the version number string.
   * @return The version number string in the format of major.minor.patch.
   */
  static String Version() {
    return String(kVersionMajor) + '.' + kVersionMinor + '.' + kVersionPatch;
  }

  /**
   * @~Chinese
   * @brief 构造函数，用于创建一个 EncoderMotor 对象。
   * @param[in] pin_positive 电机正极引脚编号。
   * @param[in] pin_negative 电机负极引脚编号。
   * @param[in] pin_a 编码器A相引脚编号。
   * @param[in] pin_b 编码器B相引脚编号。
   * @param[in] ppr 每转脉冲数。
   * @param[in] reduction_ration 减速比。
   * @param[in] phase_relation 相位关系（A相领先或B相领先，指电机正转时的情况），@ref PhaseRelation。
   * @details
   * 如果用户不清楚自己所使用的编码电机的phase_relation参数具体取值，可以使用示例程序 @ref detect_phase_relation.ino
   * 来帮助检测确定该参数的值。
   */
  /**
   * @~English
   * @brief Constructor for creating an EncoderMotor object.
   * @param[in] pin_positive The pin number of the motor's positive pole.
   * @param[in] pin_negative The pin number of the motor's negative pole.
   * @param[in] pin_a The pin number of the encoder's A phase.
   * @param[in] pin_b The pin number of the encoder's B phase.
   * @param[in] ppr Pulses per revolution.
   * @param[in] reduction_ration Reduction ratio.
   * @param[in] phase_relation Phase relationship (A phase leads or B phase leads, referring to the situation when the motor is
   * rotating forward), @ref PhaseRelation.
   * @details
   * If the user is unsure about the value of the phase_relation parameter for the encoded motor they are using, they
   * can use the example program @ref detect_phase_relation.ino to help detect and determine the value of this parameter.
   */
  EncoderMotor(const uint8_t pin_positive,
               const uint8_t pin_negative,
               const uint8_t pin_a,
               const uint8_t pin_b,
               const uint32_t ppr,
               const uint32_t reduction_ration,
               const PhaseRelation phase_relation);

  ~EncoderMotor();

  /**
   * @~Chinese
   * @brief 初始化电机设置。
   */
  /**
   * @~English
   * @brief Initialize motor settings.
   */
  void Init();

  /**
   * @~Chinese
   * @brief 使用给定的比例（P）、积分（I）、微分（D）参数值来设置速度PID控制器的参数。
   * @param[in] p 比例系数（P）的值。
   * @param[in] i 积分系数（I）的值。
   * @param[in] d 微分系数（D）的值。
   */
  /**
   * @~English
   * @brief Set the parameters of the speed PID controller with the given Proportional (P), Integral (I), and Derivative (D)
   * parameter values.
   * @param[in] p The value of the Proportional coefficient (P).
   * @param[in] i The value of the Integral coefficient (I).
   * @param[in] d The value of the Derivative coefficient (D).
   */
  void SetSpeedPid(const float p, const float i, const float d);

  /**
   * @~Chinese
   * @brief 通过指针获取速度PID控制器的比例（P）、积分（I）、微分（D）参数值。
   * @param[out] p 用于获取比例系数（P）值的指针，函数执行后该指针指向的内存位置将存储对应的参数值。
   * @param[out] i 用于获取积分系数（I）值的指针，函数执行后该指针指向的内存位置将存储对应的参数值。
   * @param[out] d 用于获取微分系数（D）值的指针，函数执行后该指针指向的内存位置将存储对应的参数值。
   */
  /**
   * @~English
   * @brief Get the Proportional (P), Integral (I), and Derivative (D) parameter values of the speed PID controller through
   * pointers.
   * @param[out] p Pointer used to get the value of the Proportional coefficient (P). After the function is executed, the memory
   * location pointed to by this pointer will store the corresponding parameter value.
   * @param[out] i Pointer used to get the value of the Integral coefficient (I). After the function is executed, the memory
   * location pointed to by this pointer will store the corresponding parameter value.
   * @param[out] d Pointer used to get the value of the Derivative coefficient (D). After the function is executed, the memory
   * location pointed to by this pointer will store the corresponding parameter value.
   */
  void GetSpeedPid(float* const p, float* const i, float* const d);

  /**
   * @~Chinese
   * @brief 直接设置电机的PWM占空比。
   * @param[in] pwm_duty PWM占空比（取值范围 -1023到1023）。
   */
  /**
   * @~English
   * @brief Set motor PWM directly.
   * @param[in] pwm_duty PWM duty cycle (-1023 to 1023).
   */
  void RunPwmDuty(const int16_t pwm_duty);

  /**
   * @~Chinese
   * @brief 以设定的速度值（RPM）运行电机。
   * @param[in] rpm 速度设定值（RPM）。
   */
  /**
   * @~English
   * @brief Run motor at speed setpoint.
   * @param[in] rpm Speed setpoint(RPM).
   */
  void RunRpm(const int16_t rpm);

  /**
   * @~Chinese
   * @brief 停止电机运行。
   */
  /**
   * @~English
   * @brief Stop motor.
   */
  void Stop();

  /**
   * @~Chinese
   * @brief 获取编码器脉冲计数。该计数值是在A相下降沿的时候计数，如果是正转会加一，反转则减一。
   * @return 编码器脉冲数。
   */
  /**
   * @~English
   * @brief Get encoder pulse count. The count value is incremented by one during forward rotation and decremented by one during
   * reverse rotation, counted at the falling edge of phase A.
   * @return int32_t Encoder pulses.
   */
  int64_t EncoderPulseCount() const;

  /**
   * @~Chinese
   * @brief 获取电机当前的转速（RPM）。
   * @return 电机当前的转速（RPM）。
   */
  /**
   * @~English
   * @brief Get the current speed of the motor.
   * @return The current speed of the motor in RPM.
   */
  int32_t Rpm() const;

  /**
   * @~Chinese
   * @brief 获取电机驱动器的PWM占空比。
   * @return 电机驱动器的PWM占空比。
   */
  /**
   * @~English
   * @brief Get the PWM pwm_duty cycle of the motor driver.
   * @return The PWM pwm_duty cycle of the motor driver.
   */
  int16_t PwmDuty() const;

  /**
   * @~Chinese
   * @brief 获取电机的目标转速（RPM）。
   * @return 电机的目标转速（RPM）。
   */
  /**
   * @~English
   * @brief Get the target speed of the motor in RPM.
   * @return The target speed of the motor in RPM.
   */
  int32_t TargetRpm() const;

 private:
  static void OnPinAFalling(void* self);

  void OnPinAFalling();

  void UpdateRpm();

  void Driving();

  void DeleteThread(std::thread*& thread);

  struct Pid {
    float p = 0.0;
    float i = 0.0;
    float d = 0.0;
    float integral = 0.0;
    float max_integral = 0.0;
  };

  mutable std::mutex mutex_;
  std::condition_variable condition_;
  std::thread* update_rpm_thread_ = nullptr;
  std::thread* driving_thread_ = nullptr;
  MotorDriver motor_driver_;
  const uint8_t pin_a_ = 0;
  const uint8_t pin_b_ = 0;
  const double total_ppr_ = 0;
  const uint8_t b_level_at_a_falling_edge_ = 0;
  Pid rpm_pid_;
  int64_t previous_pulse_count_ = 0;
  std::atomic<int64_t> pulse_count_ = 0;
  std::chrono::system_clock::time_point last_update_speed_time_ = std::chrono::time_point<std::chrono::system_clock>::min();
  int32_t rpm_ = 0;
  int32_t target_rpm_ = 0.0;
  bool drive_ = false;
};
}  // namespace em

#endif