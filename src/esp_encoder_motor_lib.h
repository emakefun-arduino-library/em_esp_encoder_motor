#pragma once

#ifndef _EM_ESP_ENCODER_MOTOR_LIB_H_
#define _EM_ESP_ENCODER_MOTOR_LIB_H_

/**
 * @file esp_encoder_motor_lib.h
 */

/**
 * @mainpage
 * @~Chinese
 * - Github链接： https://github.com/emakefun-arduino-library/em_esp_encoder_motor
 * - 库下载链接： https://github.com/emakefun-arduino-library/em_esp_encoder_motor/archive/refs/tags/latest.zip
 *
 * @~English
 * - Github link: https://github.com/emakefun-arduino-library/em_esp_encoder_motor
 * - Library download link: https://github.com/emakefun-arduino-library/em_esp_encoder_motor/archive/refs/tags/latest.zip
 */

#include <WString.h>

namespace em {
namespace esp_encoder_motor_lib {
/**
 * @~Chinese
 * @brief 主版本号。
 */
/**
 * @~English
 * @brief Major version number.
 */
constexpr uint8_t kVersionMajor = 1;

/**
 * @~Chinese
 * @brief 次版本号。
 */
/**
 * @~English
 * @brief Minor version number.
 */
constexpr uint8_t kVersionMinor = 0;

/**
 * @~Chinese
 * @brief 修订版本号。
 */
/**
 * @~English
 * @brief Patch version number.
 */
constexpr uint8_t kVersionPatch = 2;

/**
 * @~Chinese
 * @brief 获取版本号字符串。
 * @return 版本号字符串，格式为 major.minor.patch。
 */
/**
 * @~English
 * @brief Get the version number string.
 * @return The version number string in the format of major.minor.patch.
 */
String Version() {
  return String(kVersionMajor) + '.' + kVersionMinor + '.' + kVersionPatch;
}
}  // namespace esp_encoder_motor_lib
}  // namespace em

#endif