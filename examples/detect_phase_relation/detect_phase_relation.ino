#include "esp_encoder_motor.h"

static em::EncoderMotor g_encoder_motor_0(
    GPIO_NUM_27, GPIO_NUM_13, GPIO_NUM_18, GPIO_NUM_19, 12, 90, em::EncoderMotor::PhaseRelation::kAPhaseLeads);

void setup() {
  Serial.begin(115200);
  printf("setting up\n");
  g_encoder_motor_0.Init();
  g_encoder_motor_0.RunPwmDuty(4095);
  printf("setup completed\n");
}

void loop() {
  const auto speed_rpm = g_encoder_motor_0.SpeedRpm();
  if (speed_rpm > 0) {
    printf(
        "[%ul] Speed: %d RPM. The phase of A leads B. Constructed with the em::EncoderMotor::PhaseRelation::kAPhaseLeads "
        "enum.\n",
        millis(),
        speed_rpm);
  } else if (speed_rpm < 0) {
    printf(
        "[%ul] Speed: %d RPM. The phase of B leads A. Constructed with the em::EncoderMotor::PhaseRelation::kBPhaseLeads "
        "enum.\n",
        millis(),
        speed_rpm);
  } else {
    printf("The motor is not running currently.\n");
  }
  delay(200);
}