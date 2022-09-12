/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#ifndef __VERMICELLI_VERMICELLI_APPLICATION_H__
#define __VERMICELLI_VERMICELLI_APPLICATION_H__
#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include "vermicelli_window.h"
#include "vermicelli_device.h"
#include "vermicelli_renderer.h"
#include "vermicelli_game_object.h"
#include <memory>
#include <vector>

namespace vermicelli {

class Application {
  VermicelliWindow                  mWindow{"Vermicelli", mDim};
  bool                              mVerbose;
  VermicelliDevice                  mDevice{mWindow, mVerbose};
  VermicelliRenderer                mRenderer{mWindow, mDevice, mVerbose};
  std::vector<VermicelliGameObject> mGameObjects;

  void loadGameObjects();

public:
  explicit Application(bool verbose);

  ~Application();

  Application(const Application &) = delete;

  Application &operator=(const Application &) = delete;

  static constexpr glm::u32vec2 mDim{800, 600};

  void run();

};

}

#endif //__VERMICELLI_VERMICELLI_APPLICATION_H__