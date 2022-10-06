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

const glm::vec3 pink{1.000f, 0.749f, 0.749f};
const glm::vec3 magenta{0.984f, 0.192f, 0.600f};
const glm::vec3 red{1.000f, 0.000f, 0.000f};
const glm::vec3 brown{0.749f, 0.502f, 0.251f};
const glm::vec3 orange{1.000f, 0.502f, 0.000f};
const glm::vec3 yellow{1.000f, 0.922f, 0.239f};
const glm::vec3 lime{0.749f, 1.000f, 0.000f};
const glm::vec3 green{0.000f, 0.996f, 0.000f};
const glm::vec3 olive{0.624f, 0.549f, 0.094f};
const glm::vec3 teal{0.000f, 0.502f, 0.502f};
const glm::vec3 cyan{0.000f, 0.725f, 0.949f};
const glm::vec3 blue{0.000f, 0.000f, 1.000f};
const glm::vec3 indigo{0.055f, 0.298f, 0.973f};
const glm::vec3 violet{0.502f, 0.000f, 0.502f};
const glm::vec3 purple{0.749f, 0.000f, 0.251f};
const glm::vec3 black{0.000f, 0.000f, 0.000f};
const glm::vec3 white{1.000f, 1.000f, 1.000f};
const glm::vec3 gray{0.502f, 0.502f, 0.502f};
const glm::vec3 lGray{0.749f, 0.749f, 0.749f};
const glm::vec3 dGray{0.251f, 0.251f, 0.251f};
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
