/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/9/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_GAME_OBJECT_H__
#define __VERMICELLI_VERMICELLI_GAME_OBJECT_H__
#pragma once

#include "vermicelli_model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace vermicelli {

struct TransformComponent {
    glm::vec3 mTranslation{}; ///< Position Offset
    glm::vec3 mScale{1.0f, 1.0f, 1.0f}; ///< Object scale
    glm::vec3 mRotation{}; ///< Object rotation
    glm::mat4 mat4() {
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
              {mScale.x * (c1 * c3 + s1 * s2 * s3), mScale.x * (c2 * s3), mScale.x * (c1 * s2 * s3 - c3 * s1), 0.0f,},
              {mScale.y * (c3 * s1 * s2 - c1 * s3), mScale.y * (c2 * c3), mScale.y * (c1 * c3 * s2 + s1 * s3), 0.0f,},
              {mScale.z * (c2 * s1),                mScale.z * (-s2),     mScale.z * (c1 * c2),                0.0f,},
              {mTranslation.x,                      mTranslation.y,       mTranslation.z,                      1.0f}};
    }
};

class VermicelliGameObject {
public:
  using id_t = unsigned int;

  static VermicelliGameObject createGameObject() {
    static id_t currentID = 0;
    return VermicelliGameObject(currentID++);
  }

  VermicelliGameObject(const VermicelliGameObject &) = delete;

  VermicelliGameObject operator=(const VermicelliGameObject &) = delete;

  VermicelliGameObject(VermicelliGameObject &&) = default;

  VermicelliGameObject &operator=(VermicelliGameObject &&) = default;

  id_t getID() const { return mID; }

  std::shared_ptr<VermicelliModel> mModel{};
  glm::vec3                        mColor{};
  TransformComponent               mTransform{};

private:
  explicit VermicelliGameObject(id_t objID) : mID{objID} {}

  id_t mID;
};
}

#endif //__VERMICELLI_VERMICELLI_GAME_OBJECT_H__
