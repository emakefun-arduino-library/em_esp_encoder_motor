#include "i2c_device.h"

namespace {
enum MemoryAddress : uint8_t {
  kMemoryAddressExample = 0x00,
};
}

namespace emakefun {

I2cDevice::I2cDevice(const uint8_t i2c_address, TwoWire& wire) : i2c_address_(i2c_address), wire_(wire) {
  // do somethings
}

I2cDevice::ErrorCode I2cDevice::Initialize() {
  // do somethings

  wire_.beginTransmission(i2c_address_);
  return static_cast<ErrorCode>(wire_.endTransmission());
}

}  // namespace emakefun