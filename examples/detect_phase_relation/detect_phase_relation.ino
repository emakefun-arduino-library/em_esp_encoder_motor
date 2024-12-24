#include "esp_encoder_motor.h"
#include "esp_encoder_motor_lib.h"

namespace {
constexpr uint32_t kPPR = 12;              // Pulses per revolution.
constexpr uint32_t kReductionRation = 90;  // Reduction ratio.

em::EspEncoderMotor g_encoder_motor_0(  // E0
    GPIO_NUM_27,                        // The pin number of the motor's positive pole.
    GPIO_NUM_13,                        // The pin number of the motor's negative pole.
    GPIO_NUM_18,                        // The pin number of the encoder's A phase.
    GPIO_NUM_19,                        // The pin number of the encoder's B phase.
    kPPR,                               // Pulses per revolution.
    kReductionRation,                   // Reduction ratio.
    em::EspEncoderMotor::kAPhaseLeads   // Phase relationship (A phase leads or B phase leads, referring to the situation when
                                        // the motor is rotating forward)
);
}  // namespace

void setup() {
  Serial.begin(115200);
  printf("setting up\n");
  printf("Emakefun ESP Encoder Motor Library Version: %s\n", em::esp_encoder_motor_lib::Version().c_str());
  g_encoder_motor_0.Init();
  g_encoder_motor_0.RunPwmDuty(4095);
  printf("setup completed\n");
}

void loop() {
  const auto rpm = g_encoder_motor_0.SpeedRpm();
  if (rpm > 0) {
    printf(
        "[%lu] RPM: %d . The phase of A leads B. Constructed with the em::EncoderMotor::PhaseRelation::kAPhaseLeads "
        "enum.\n",
        millis(),
        rpm);
  } else if (rpm < 0) {
    printf(
        "[%lu] RPM: %d . The phase of B leads A. Constructed with the em::EncoderMotor::PhaseRelation::kBPhaseLeads "
        "enum.\n",
        millis(),
        rpm);
  } else {
    printf("The motor is not running currently.\n");
  }
  delay(200);
}