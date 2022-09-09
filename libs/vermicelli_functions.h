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

namespace vermicelli {
namespace color {
glm::vec3 RGB(uint8_t r, uint8_t g, uint8_t b);

glm::vec3 CMYK(float c, float m, float y, float k);

glm::vec3 hex(std::string hex);

glm::vec3 hex(uint32_t i_hex);

}

void helpMenu();

}

#endif //__VERMICELLI_VERMICELLI_FUNCTIONS_H__
