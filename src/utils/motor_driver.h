#pragma once

#include <cmath>
#include <cstdint>

namespace emf {

class MotorDriver {
 public:
  static constexpr uint8_t kPwmResolution = 12;

  static constexpr uint8_t kPwmFrequency = 10000;

  static_assert(kPwmResolution > 1);

  static constexpr int16_t kMaxPwmDuty = pow(2, kPwmResolution) - 1;

  explicit MotorDriver(const uint8_t pos_pin, const uint8_t neg_pin);

  ~MotorDriver() = default;

  void Init();

  void Duty(const int16_t duty);

  int16_t Duty() const;

  void Break();

  void Release();

 private:
  const uint8_t pos_pin_ = 0xFF;
  const uint8_t neg_pin_ = 0xFF;
  int16_t duty_ = 0;
};
}  // namespace em