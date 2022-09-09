/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/9/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_functions.h"
///FIXME: Change to <format> when it's implemented by clang
#include <fmt/core.h>

namespace vermicelli {

void helpMenu() {
  std::cout << "Vermicelli, a Vulkan Renderer and Engine built on SDL2" << std::endl;
}

namespace color {

glm::vec3 RGB(uint8_t r, uint8_t g, uint8_t b) {
  return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f};
}

glm::vec3 CMYK(float c, float m, float y, float k) {
  return {(155.0f * (1 - c) * (1 - k)) / 255.0f, (155.0f * (1 - m) * (1 - k)) / 255.0f,
          (155.0f * (1 - y) * (1 - k)) / 255.0f};
}

/**
 * @brief A private function to handle different versions of hex colors all in one place.
 * @param colorValue The value of the color to change
 * @return glm vec3 of floats between 0 and 1 for r,g,b
 */
static glm::vec3 ref_hex(std::string &colorValue) {
  if (colorValue.at(0) == '#') {
    colorValue.erase(0, 1);
  }
  if (colorValue.length() == 3) {
    colorValue = std::string(colorValue.substr(0, 1) + colorValue.substr(0, 1)
                             + colorValue.substr(1, 1) + colorValue.substr(1, 1)
                             + colorValue.substr(2, 1) + colorValue.substr(2, 1));
  }
  float r = static_cast<float>(std::strtol(colorValue.substr(0, 2).c_str(), nullptr, 16));
  float g = static_cast<float>(std::strtol(colorValue.substr(2, 2).c_str(), nullptr, 16));
  float b = static_cast<float>(std::strtol(colorValue.substr(4, 2).c_str(), nullptr, 16));
  return {r / 255.0f, g / 255.0f, b / 255.0f};
}

/**
 * @brief Receives an input color in hex format as a string (with/without preceding #). Can handle 3-char and 6-char
 * @param col The input color as a string
 * @return glm vec3 of floats between 0 and 1 for r,g,b
 */
glm::vec3 hex(std::string col) {
  return ref_hex(col);
}

/**
 * @brief Receives an input color in hex format as an int. Can handle 3-char and 6-char
 * @param col The input color as an int
 * @return glm vec3 of floats between 0 and 1 for r,g,b
 */
glm::vec3 hex(uint32_t i_col) {
  std::string hex = fmt::format("{:x}", i_col);
  return ref_hex(hex);
}


}
}