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
#include "vermicelli_functions.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

namespace vermicelli {

struct TransformComponent {
    glm::vec3 mTranslation{}; ///< Position Offset
    glm::vec3 mScale{1.0f, 1.0f, 1.0f}; ///< Object scale
    glm::vec3 mRotation{}; ///< Object rotation
    glm::mat4 mat4();

    glm::mat3 normalMatrix();
};

struct VermicelliPointLightComponent {
    float mLightIntensity = 1.0f;
};

class VermicelliGameObject {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, VermicelliGameObject>;

  static VermicelliGameObject createGameObject() {
    static id_t currentID = 0;
    return VermicelliGameObject(currentID++);
  }

  static VermicelliGameObject makePointLight(float intensity = 5.0f, float radius = 1.0f, glm::vec3 col = color::white);

  VermicelliGameObject(const VermicelliGameObject &) = delete;

  VermicelliGameObject operator=(const VermicelliGameObject &) = delete;

  VermicelliGameObject(VermicelliGameObject &&) = default;

  VermicelliGameObject &operator=(VermicelliGameObject &&) = default;

  id_t getID() const { return mID; }

  glm::vec3          mColor{};
  TransformComponent mTransform{};

  // Optional pointer components;
  std::shared_ptr<VermicelliModel>               mModel{};
  std::unique_ptr<VermicelliPointLightComponent> mPointLight = nullptr;

private:
  explicit VermicelliGameObject(id_t objID) : mID{objID} {}

  id_t mID;
};
}

#endif //__VERMICELLI_VERMICELLI_GAME_OBJECT_H__
