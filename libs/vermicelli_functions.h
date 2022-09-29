/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/9/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_FUNCTIONS_H__
#define __VERMICELLI_VERMICELLI_FUNCTIONS_H__
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> // PI
#include <iostream>
#include <functional>

namespace vermicelli {
namespace color {
glm::vec3 RGB(uint8_t r, uint8_t g, uint8_t b);

glm::vec3 CMYK(float c, float m, float y, float k);

glm::vec3 hex(std::string hex);

glm::vec3 hex(uint32_t i_hex);

}

void helpMenu();


const uint32_t RAND_SERIES = 0x9E3779b9; // Helps ensure correct output from hash function.

template<typename T, typename... Rest>
void hashCombine(std::size_t &seed, const T &v, const Rest &... rest) {
  seed ^= std::hash<T>{}(v) + RAND_SERIES + (seed << 6) + (seed >> 2); // bit-shifting is to shuffle the incoming seed.
  (hashCombine(seed, rest), ...);
}

}

#endif //__VERMICELLI_VERMICELLI_FUNCTIONS_H__
