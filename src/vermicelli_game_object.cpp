/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/20/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_game_object.h"

namespace vermicelli {

glm::mat3 TransformComponent::normalMatrix() {
  const float     c3           = glm::cos(mRotation.z);
  const float     s3           = glm::sin(mRotation.z);
  const float     c2           = glm::cos(mRotation.x);
  const float     s2           = glm::sin(mRotation.x);
  const float     c1           = glm::cos(mRotation.y);
  const float     s1           = glm::sin(mRotation.y);
  const glm::vec3 inverseScale = 1.0f / mScale;
  /* REMEMBER: THIS IS COLUMN MAJOR; this looks like: when interpreted mathematically
   * This follows the Y-X-Z rotation
   Scale x * (c1 * c3 + s1 * s2 * s3), scale.y * (c3 * s1 * s2 - c1 * s3) , scale.z * (c2 * s1), transform x
   Scale x * (c2 * s3)               , scale.y * (c2 * c3)                , scale.z * (-s2)    , transform y
   Scale x * (c1 * s2 * s3 - c3 * s1), scale.y * (c1 * c3 * s2 + s1 * s3) , scale.z * (c1 * c2), transform z
   0                                 , 0                                  , 0                  , 1
  */
  return glm::mat3{
          {inverseScale.x * (c1 * c3 + s1 * s2 * s3), inverseScale.x * (c2 * s3),
                                                                                  inverseScale.x *
                                                                                  (c1 * s2 * s3 - c3 * s1)},
          {inverseScale.y * (c3 * s1 * s2 - c1 * s3), inverseScale.y * (c2 * c3), inverseScale.y *
                                                                                  (c1 * c3 * s2 + s1 * s3)},
          {inverseScale.z * (c2 * s1),                inverseScale.z * (-s2),     inverseScale.z * (c1 * c2),}};
}

glm::mat4 TransformComponent::mat4() {
  const float c3 = glm::cos(mRotation.z);
  const float s3 = glm::sin(mRotation.z);
  const float c2 = glm::cos(mRotation.x);
  const float s2 = glm::sin(mRotation.x);
  const float c1 = glm::cos(mRotation.y);
  const float s1 = glm::sin(mRotation.y);
  /* REMEMBER: THIS IS COLUMN MAJOR; this looks like: when interpreted mathematically
   * This follows the Y-X-Z rotation
   Scale x * (c1 * c3 + s1 * s2 * s3), scale.y * (c3 * s1 * s2 - c1 * s3) , scale.z * (c2 * s1), transform x
   Scale x * (c2 * s3)               , scale.y * (c2 * c3)                , scale.z * (-s2)    , transform y
   Scale x * (c1 * s2 * s3 - c3 * s1), scale.y * (c1 * c3 * s2 + s1 * s3) , scale.z * (c1 * c2), transform z
   0                                 , 0                                  , 0                  , 1
  */
  return glm::mat4{
          {mScale.x * (c1 * c3 + s1 * s2 * s3), mScale.x * (c2 * s3), mScale.x * (c1 * s2 * s3 - c3 * s1), 0.0f},
          {mScale.y * (c3 * s1 * s2 - c1 * s3), mScale.y * (c2 * c3), mScale.y * (c1 * c3 * s2 + s1 * s3), 0.0f},
          {mScale.z * (c2 * s1),                mScale.z * (-s2),     mScale.z * (c1 * c2),                0.0f},
          {mTranslation.x,                      mTranslation.y,       mTranslation.z,                      1.0f}};
}
}