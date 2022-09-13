/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/13/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#include "vermicelli_camera.h"

namespace vermicelli {

void
VermicelliCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
  mProjectionMatrix = glm::mat4{1.0f};
  mProjectionMatrix[0][0] = 2.0f / (right - left);
  mProjectionMatrix[1][1] = 2.0f / (bottom - top);
  mProjectionMatrix[2][2] = 1.0f / (far - near);
  mProjectionMatrix[3][0] = -(right + left) / (right - left);
  mProjectionMatrix[3][1] = -(bottom + top) / (bottom - top);
  mProjectionMatrix[3][2] = -near / (far - near);
}

void VermicelliCamera::setPerspectiveProjection(float fov_y, float aspect, float near, float far) {
  assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
  mProjectionMatrix = glm::mat4{1.0f};
  const float tanHalfFov_y = glm::tan(fov_y / 2.0f);
  mProjectionMatrix[0][0] = 1.0f / (aspect * tanHalfFov_y);
  mProjectionMatrix[1][1] = 1.0f / tanHalfFov_y;
  mProjectionMatrix[2][2] = far / (far - near);
  mProjectionMatrix[2][3] = 1.0f;
  mProjectionMatrix[3][2] = -(far * near) / (far - near);
}

void VermicelliCamera::setViewDirection(glm::vec3 camPos, glm::vec3 camDir, glm::vec3 up) {
  const glm::vec3 w{glm::normalize(camDir)};
  const glm::vec3 u{glm::normalize(glm::cross(w, up))};
  const glm::vec3 v{glm::cross(w, u)};

  mViewMatrix[0][0] = u.x;
  mViewMatrix[1][0] = u.y;
  mViewMatrix[2][0] = u.z;
  mViewMatrix[0][1] = v.x;
  mViewMatrix[1][1] = v.y;
  mViewMatrix[2][1] = v.z;
  mViewMatrix[0][2] = w.x;
  mViewMatrix[1][2] = w.y;
  mViewMatrix[2][2] = w.z;
  mViewMatrix[3][0] = -glm::dot(u, camPos);
  mViewMatrix[3][1] = -glm::dot(v, camPos);
  mViewMatrix[3][2] = -glm::dot(w, camPos);
}

void VermicelliCamera::setViewYXZ(glm::vec3 camPos, glm::vec3 camRot) {
  const float     c3 = glm::cos(camRot.z);
  const float     s3 = glm::sin(camRot.z);
  const float     c2 = glm::cos(camRot.x);
  const float     s2 = glm::sin(camRot.x);
  const float     c1 = glm::cos(camRot.y);
  const float     s1 = glm::sin(camRot.y);
  const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
  const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
  const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
  mViewMatrix = glm::mat4{1.f};
  mViewMatrix[0][0] = u.x;
  mViewMatrix[1][0] = u.y;
  mViewMatrix[2][0] = u.z;
  mViewMatrix[0][1] = v.x;
  mViewMatrix[1][1] = v.y;
  mViewMatrix[2][1] = v.z;
  mViewMatrix[0][2] = w.x;
  mViewMatrix[1][2] = w.y;
  mViewMatrix[2][2] = w.z;
  mViewMatrix[3][0] = -glm::dot(u, camPos);
  mViewMatrix[3][1] = -glm::dot(v, camPos);
  mViewMatrix[3][2] = -glm::dot(w, camPos);

}

}