#include "esp_motor.h"

namespace {

/**
 * M0:
 * +: GPIO 27
 * -: GPIO 13
 */
em::EspMotor g_motor_0(GPIO_NUM_27, GPIO_NUM_13);

/**
 * M1:
 * +: GPIO 4
 * -: GPIO 2
 */
em::EspMotor g_motor_1(GPIO_NUM_4, GPIO_NUM_2);

/**
 * M2:
 * +: GPIO 17
 * -: GPIO 12
 */
em::EspMotor g_motor_2(GPIO_NUM_17, GPIO_NUM_12);

/**
 * M3:
 * +: GPIO 15
 * -: GPIO 14
 */
em::EspMotor g_motor_3(GPIO_NUM_15, GPIO_NUM_14);
}  // namespace

void setup() {
  Serial.begin(115200);
  printf("setting up\n");
  g_motor_0.Init();
  g_motor_1.Init();
  g_motor_2.Init();
  g_motor_3.Init();
  printf("setup completed\n");
}

void loop() {
  g_motor_0.PwmDuty(1023);
  g_motor_1.PwmDuty(1023);
  g_motor_2.PwmDuty(1023);
  g_motor_3.PwmDuty(1023);
  printf("forward\n");
  delay(1000);

  g_motor_0.PwmDuty(-1023);
  g_motor_1.PwmDuty(-1023);
  g_motor_2.PwmDuty(-1023);
  g_motor_3.PwmDuty(-1023);
  printf("backward\n");
  delay(1000);
}