#include "esp_encoder_motor.h"

/**
 * E0:
 * +: GPIO 27
 * -: GPIO 13
 * A: GPIO 18
 * B: GPIO 19
 */
static em::EspEncoderMotor g_encoder_motor_0(
    GPIO_NUM_27, GPIO_NUM_13, GPIO_NUM_18, GPIO_NUM_19, 12, 90, em::EspEncoderMotor::PhaseRelation::kAPhaseLeads);

/**
 * E1:
 * +: GPIO 4
 * -: GPIO 2
 * A: GPIO 5
 * B: GPIO 23
 */
static em::EspEncoderMotor g_encoder_motor_1(
    GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_5, GPIO_NUM_23, 12, 90, em::EspEncoderMotor::PhaseRelation::kAPhaseLeads);

/**
 * E2:
 * +: GPIO 17
 * -: GPIO 12
 * A: GPIO 35
 * B: GPIO 16
 */
static em::EspEncoderMotor g_encoder_motor_2(
    GPIO_NUM_17, GPIO_NUM_12, GPIO_NUM_35, GPIO_NUM_36, 12, 90, em::EspEncoderMotor::PhaseRelation::kAPhaseLeads);

/**
 * E3:
 * +: GPIO 15
 * -: GPIO 14
 * A: GPIO 34
 * B: GPIO 39
 */
static em::EspEncoderMotor g_encoder_motor_3(
    GPIO_NUM_15, GPIO_NUM_14, GPIO_NUM_34, GPIO_NUM_39, 12, 90, em::EspEncoderMotor::PhaseRelation::kAPhaseLeads);

void setup() {
  Serial.begin(115200);
  printf("setting up\n");
  g_encoder_motor_0.Init();
  g_encoder_motor_1.Init();
  g_encoder_motor_2.Init();
  g_encoder_motor_3.Init();
  pinMode(26, INPUT);
  printf("setup completed\n");
}

void loop() {
  const int16_t speed_rpm = map(analogRead(26), 0, 4095, -21, 21) * 5;

  g_encoder_motor_0.RunSpeed(speed_rpm);
  g_encoder_motor_1.RunSpeed(speed_rpm);
  g_encoder_motor_2.RunSpeed(speed_rpm);
  g_encoder_motor_3.RunSpeed(speed_rpm);

  printf("target speed rpm: %4" PRIi16 ", current speed rpm: [%4" PRId32 ", %4" PRId32 ", %4" PRId32 ", %4" PRId32
         "], pwm duties: [%5 " PRIi16 ", %5" PRIi16 ", %5" PRIi16 ", %5" PRIi16 "], pulse counts: [%" PRId64 ", %" PRId64
         ", %" PRId64 ", %" PRId64 "]\n",
         speed_rpm,
         g_encoder_motor_0.SpeedRpm(),
         g_encoder_motor_1.SpeedRpm(),
         g_encoder_motor_2.SpeedRpm(),
         g_encoder_motor_3.SpeedRpm(),
         g_encoder_motor_0.PwmDuty(),
         g_encoder_motor_1.PwmDuty(),
         g_encoder_motor_2.PwmDuty(),
         g_encoder_motor_3.PwmDuty(),
         g_encoder_motor_0.EncoderPulseCount(),
         g_encoder_motor_1.EncoderPulseCount(),
         g_encoder_motor_2.EncoderPulseCount(),
         g_encoder_motor_3.EncoderPulseCount());

  delay(100);
}