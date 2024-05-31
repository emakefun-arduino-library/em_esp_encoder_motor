#include "i2c_device.h"

#define INFINITE_LOOP_ON_FAILURE InfiniteLoopOnFailure(__FUNCTION__, __LINE__)

namespace {

emakefun::I2cDevice g_i2c_device;

void InfiniteLoopOnFailure(const char* function, const uint32_t line_number) {
  Serial.println(String(F("entering an infinite loop due to failure in ")) + function + F(", at line number: ") + line_number);
  while (true) {
    yield();
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println(F("setup"));

  Wire.begin();

  const auto ret = g_i2c_device.Initialize();

  if (emakefun::I2cDevice::kOK == ret) {
    Serial.println(F("i2c device initialization successful"));
  } else {
    Serial.print(F("i2c device initialization failed: "));
    Serial.println(ret);
    INFINITE_LOOP_ON_FAILURE;
  }

  Serial.println(F("setup successful"));
}

void loop() { yield(); }
