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
    glm::mat4 mat4();

    glm::mat3 normalMatrix();
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
