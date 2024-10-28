/**
 * @example forward_stop_backward.ino
 */

#include "esp_encoder_motor.h"

namespace {
auto const g_encoder_motor_0 = new emf::EncoderMotor(GPIO_NUM_27, GPIO_NUM_13, GPIO_NUM_19, GPIO_NUM_18, 12, 90, true);
auto const g_encoder_motor_1 = new emf::EncoderMotor(GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_25, GPIO_NUM_23, 12, 90, true);
auto const g_encoder_motor_2 = new emf::EncoderMotor(GPIO_NUM_15, GPIO_NUM_14, GPIO_NUM_34, GPIO_NUM_35, 12, 90, true);
auto const g_encoder_motor_3 = new emf::EncoderMotor(GPIO_NUM_17, GPIO_NUM_12, GPIO_NUM_39, GPIO_NUM_36, 12, 90, true);
enum State {
  kNone,
  kForward,
  kBackward,
} g_state = kForward;
}  // namespace

void setup() {
  Serial.begin(115200);
  printf("setup\n");
  g_encoder_motor_0->Init();
  g_encoder_motor_1->Init();
  g_encoder_motor_2->Init();
  g_encoder_motor_3->Init();
  printf("setup done\n");
}

void loop() {
  const int16_t speed_rpm = 100;
  static auto s_state_changed_time = millis();

  switch (g_state) {
    case kNone:
      g_encoder_motor_0->Stop();
      g_encoder_motor_1->Stop();
      g_encoder_motor_2->Stop();
      g_encoder_motor_3->Stop();
      if (millis() - s_state_changed_time > 2000) {
        g_state = kForward;
        s_state_changed_time = millis();
      }
      break;
    case kForward:
      g_encoder_motor_0->RunSpeed(speed_rpm);
      g_encoder_motor_1->RunSpeed(speed_rpm);
      g_encoder_motor_2->RunSpeed(speed_rpm);
      g_encoder_motor_3->RunSpeed(speed_rpm);
      if (millis() - s_state_changed_time > 2000) {
        g_state = kBackward;
        s_state_changed_time = millis();
      }
      break;
    case kBackward:
      g_encoder_motor_0->RunSpeed(-speed_rpm);
      g_encoder_motor_1->RunSpeed(-speed_rpm);
      g_encoder_motor_2->RunSpeed(-speed_rpm);
      g_encoder_motor_3->RunSpeed(-speed_rpm);
      if (millis() - s_state_changed_time > 2000) {
        g_state = kNone;
        s_state_changed_time = millis();
      }
      break;
    default:
      break;
  }

  printf("target speed(RMP): %4" PRIi16 ", speed(RPM):[%4" PRId32 ", %4" PRId32 ", %4" PRId32 ", %4" PRId32
         "], pwm duty:[%5 " PRIi16 ", %5" PRIi16 ", %5" PRIi16 ", %5" PRIi16 "], pulse counts:[%" PRId64 ", %" PRId64
         ", %" PRId64 ", %" PRId64 "]\n",
         speed_rpm,
         g_encoder_motor_0->Speed(),
         g_encoder_motor_1->Speed(),
         g_encoder_motor_2->Speed(),
         g_encoder_motor_3->Speed(),
         g_encoder_motor_0->PwmDuty(),
         g_encoder_motor_1->PwmDuty(),
         g_encoder_motor_2->PwmDuty(),
         g_encoder_motor_3->PwmDuty(),
         g_encoder_motor_0->EncoderPulseCount(),
         g_encoder_motor_1->EncoderPulseCount(),
         g_encoder_motor_2->EncoderPulseCount(),
         g_encoder_motor_3->EncoderPulseCount());

  delay(100);
}