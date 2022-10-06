/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/13/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#include "vermicelli_keyboard_input.h"

namespace vermicelli {
void
VermicelliKeyboardInput::moveInPlaneXZ(float dt, vermicelli::VermicelliGameObject &gameObject) {
  static bool   jump;
  static bool   squat;
  glm::vec3     rotate{0};
  const uint8_t *keyState = SDL_GetKeyboardState(NULL);
  if (keyState[LookRight]) rotate.y += 1.0f;
  if (keyState[LookLeft]) rotate.y -= 1.0f;
  if (keyState[LookUp]) rotate.x += 1.0f;
  if (keyState[LookDown]) rotate.x -= 1.0f;

  if (glm::dot(rotate, rotate) > glm::epsilon<float>()) { // Make sure not to rotate if rot vector is 0
    gameObject.mTransform.mRotation += mLookSpeed * dt * glm::normalize(rotate);
  }

  gameObject.mTransform.mRotation.x = glm::clamp(gameObject.mTransform.mRotation.x, -1.5f, 1.5f);
  gameObject.mTransform.mRotation.y = glm::mod(gameObject.mTransform.mRotation.y, glm::two_pi<float>());

  float           yaw  = gameObject.mTransform.mRotation.y;
  const glm::vec3 forwardDir{glm::sin(yaw), 0.0f, glm::cos(yaw)};
  const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x}; // Must be perpendicular to fwd dir
  const glm::vec3 upDir{0.0f, -1.0f, 0.0f};
  float           yPos = gameObject.mTransform.mTranslation.y;

  glm::vec3 moveDir{0.0f};
  if (keyState[MoveFwd]) moveDir += forwardDir;
  if (keyState[MoveBack]) moveDir -= forwardDir;
  if (keyState[MoveRight]) moveDir += rightDir;
  if (keyState[MoveLeft]) moveDir -= rightDir;
  // TODO: Change down and up behavior to be a temporary squat and a jump
  if (keyState[MoveUp]) {
    moveDir += (yPos < -3.5f) ? glm::vec3(0.0f) : upDir;
    jump = true;
  } else { jump = false; }
  if (keyState[MoveDown]) {
    moveDir -= (yPos > -0.9f) ? glm::vec3(0.0f) : upDir;
    squat = true;
  } else { squat = false; }

  glm::vec3        norm = glm::normalize(moveDir);
  static glm::vec3 lastNorm;


  if (keyState[DebugKey]) {
    std::cout << "Game Object State: " << gameObject.mTransform.mTranslation.x << ","
              << gameObject.mTransform.mTranslation.y << "," << gameObject.mTransform.mTranslation.z << std::endl
              << "Moving up by: " << moveDir.x << "," << moveDir.y << "," << moveDir.z
              << " aka factor of " << lastNorm.x << "," << lastNorm.y << "," << lastNorm.z << std::endl;
  }
  bool moved = glm::dot(moveDir, moveDir) > glm::epsilon<float>();
  if (moved) { // Make sure not to move if move vector is 0
    gameObject.mTransform.mTranslation.x += mMoveSpeed * dt * norm.x;
    gameObject.mTransform.mTranslation.y += (jump ? 5.0f : 1.0f) * mMoveSpeed * dt * norm.y;
    gameObject.mTransform.mTranslation.z += mMoveSpeed * dt * norm.z;
    lastNorm = norm;
  } else if ((yPos < -2.05f || yPos > -1.95f)) {
    gameObject.mTransform.mTranslation.y -= mMoveSpeed * dt * lastNorm.y;
  } else {
    gameObject.mTransform.mTranslation.y = -2.0f;
  }

}
}