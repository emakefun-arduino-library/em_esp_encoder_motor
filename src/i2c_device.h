#ifndef I2C_DEVICE_
#define I2C_DEVICE_

#include <Wire.h>
#include <stdint.h>

namespace emakefun {
class I2cDevice {
 public:
  static constexpr uint8_t kDefaultI2cAddress = 0x00;

  /**
   * @enum ErrorCode
   * @brief 错误码
   */
  enum ErrorCode : uint32_t {
    kOK = 0,                                  /**< 0：成功 */
    kI2cDataTooLongToFitInTransmitBuffer = 1, /**< 1：I2C数据太长，无法装入传输缓冲区 */
    kI2cReceivedNackOnTransmitOfAddress = 2,  /**< 2：在I2C发送地址时收到NACK */
    kI2cReceivedNackOnTransmitOfData = 3,     /**< 3：在I2C发送数据时收到NACK */
    kI2cOtherError = 4,                       /**< 4：其他I2C错误 */
    kI2cTimeout = 5,                          /**< 5：I2C通讯超时 */
    kInvalidParameter = 6,                    /**< 6：参数错误 */
  };

  explicit I2cDevice(TwoWire& wire = Wire, const uint8_t i2c_address = kDefaultI2cAddress);
  ErrorCode Initialize();

 private:
  I2cDevice(const I2cDevice&) = delete;
  I2cDevice& operator=(const I2cDevice&) = delete;

  TwoWire& wire_ = Wire;
  const uint8_t i2c_address_ = kDefaultI2cAddress;
};
}  // namespace emakefun
#endif