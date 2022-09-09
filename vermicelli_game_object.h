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
#include <memory>

namespace vermicelli {

struct Transform2dComponent {
    glm::vec2 mTranslation{}; ///< Position Offset
    glm::vec2 mScale{1.0f, 1.0f}; ///< Object scale
    float     mRotation; ///< Object rotation
    glm::mat2 mat2() {
      const float s = glm::sin(mRotation);
      const float c = glm::cos(mRotation);
      // mat2 constructor takes columns, not rows!
      glm::mat2   scaleMat{
              {mScale.x, 0.0f},
              {0.0f,     mScale.y}
      };
      glm::mat2   rotMat{
              {c,  s},
              {-s, c}
      };
      // Remember that direction of matrix multiplication matters!
      return rotMat * scaleMat;
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
  Transform2dComponent             mTransform2d{};

private:
  explicit VermicelliGameObject(id_t objID) : mID{objID} {}

  id_t mID;
};
}

#endif //__VERMICELLI_VERMICELLI_GAME_OBJECT_H__
