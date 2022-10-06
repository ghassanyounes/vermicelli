/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/13/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_CAMERA_H__
#define __VERMICELLI_VERMICELLI_CAMERA_H__
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

namespace vermicelli {

class VermicelliCamera {
  glm::mat4 mProjectionMatrix{1.0f};
  glm::mat4 mViewMatrix{1.0f};
  glm::mat4 mInverseViewMatrix{1.0f};
public:
  void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

  void setPerspectiveProjection(float fov_y, float aspect, float near, float far);

  void setViewDirection(glm::vec3 camPos, glm::vec3 camDir, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});

  void setViewTarget(glm::vec3 camPos, glm::vec3 target, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f}) {
    setViewDirection(camPos, target - camPos, up);
  }

  void setViewYXZ(glm::vec3 camPos, glm::vec3 camRot);

  [[nodiscard]] const glm::mat4 &getProjection() const { return mProjectionMatrix; }

  [[nodiscard]] const glm::mat4 &getView() const { return mViewMatrix; }

  [[nodiscard]] const glm::mat4 &getInverseView() const { return mInverseViewMatrix; }
};

}

#endif //__VERMICELLI_VERMICELLI_CAMERA_H__
