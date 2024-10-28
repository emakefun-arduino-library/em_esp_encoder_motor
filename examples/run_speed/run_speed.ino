/**
 * @example run_speed.ino
 */

#include "esp_encoder_motor.h"

namespace {
auto const encoder_motor_0 = new em::EncoderMotor(GPIO_NUM_27, GPIO_NUM_13, GPIO_NUM_19, GPIO_NUM_18, 12, 90, true);
auto const encoder_motor_1 = new em::EncoderMotor(GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_25, GPIO_NUM_23, 12, 90, true);
auto const encoder_motor_2 = new em::EncoderMotor(GPIO_NUM_15, GPIO_NUM_14, GPIO_NUM_34, GPIO_NUM_35, 12, 90, true);
auto const encoder_motor_3 = new em::EncoderMotor(GPIO_NUM_17, GPIO_NUM_12, GPIO_NUM_39, GPIO_NUM_36, 12, 90, true);
}  // namespace

void setup() {
  Serial.begin(115200);
  printf("setup\n");
  encoder_motor_0->Init();
  encoder_motor_1->Init();
  encoder_motor_2->Init();
  encoder_motor_3->Init();
  printf("setup done\n");
}

void loop() {
  const int16_t speed_rpm = 100;

  encoder_motor_0->RunSpeed(speed_rpm);
  encoder_motor_1->RunSpeed(speed_rpm);
  encoder_motor_2->RunSpeed(speed_rpm);
  encoder_motor_3->RunSpeed(speed_rpm);

  printf("target speed(RMP): %4" PRIi16 ", speed(RPM):[%4" PRId32 ", %4" PRId32 ", %4" PRId32 ", %4" PRId32
         "], pwm duty:[%5 " PRIi16 ", %5" PRIi16 ", %5" PRIi16 ", %5" PRIi16 "], pulse counts:[%" PRId64 ", %" PRId64
         ", %" PRId64 ", %" PRId64 "]\n",
         speed,
         encoder_motor_0->Speed(),
         encoder_motor_1->Speed(),
         encoder_motor_2->Speed(),
         encoder_motor_3->Speed(),
         encoder_motor_0->PwmDuty(),
         encoder_motor_1->PwmDuty(),
         encoder_motor_2->PwmDuty(),
         encoder_motor_3->PwmDuty(),
         encoder_motor_0->EncoderPulseCount(),
         encoder_motor_1->EncoderPulseCount(),
         encoder_motor_2->EncoderPulseCount(),
         encoder_motor_3->EncoderPulseCount());

  delay(100);
}