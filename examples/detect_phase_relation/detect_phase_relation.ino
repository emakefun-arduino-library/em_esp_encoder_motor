#include "esp_encoder_motor.h"

static em::EspEncoderMotor g_encoder_motor_0(
    GPIO_NUM_27, GPIO_NUM_13, GPIO_NUM_18, GPIO_NUM_19, 12, 90, em::EspEncoderMotor::PhaseRelation::kAPhaseLeads);

void setup() {
  Serial.begin(115200);
  printf("setting up\n");
  g_encoder_motor_0.Init();
  g_encoder_motor_0.RunPwmDuty(4095);
  printf("setup completed\n");
}

void loop() {
  const auto rpm = g_encoder_motor_0.SpeedRpm();
  if (rpm > 0) {
    printf(
        "[%ul] RPM: %d . The phase of A leads B. Constructed with the em::EncoderMotor::PhaseRelation::kAPhaseLeads "
        "enum.\n",
        millis(),
        rpm);
  } else if (rpm < 0) {
    printf(
        "[%ul] RPM: %d . The phase of B leads A. Constructed with the em::EncoderMotor::PhaseRelation::kBPhaseLeads "
        "enum.\n",
        millis(),
        rpm);
  } else {
    printf("The motor is not running currently.\n");
  }
  delay(200);
}