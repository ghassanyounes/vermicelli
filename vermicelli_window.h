/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#ifndef __VERMICELLI_VERMICELLI_WINDOW_H__
#define __VERMICELLI_VERMICELLI_WINDOW_H__
#pragma once

#ifndef APP_NAME
#define APP_NAME "default mWindow title"
#endif

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <glm/vec2.hpp>
#include <string>

namespace vermicelli {

class VermicelliWindow {
  glm::u32vec2   mDim;
  std::string    mName;
  SDL2pp::Window mWindow;

  void initWindow();

  VermicelliWindow(const VermicelliWindow &) = delete;

  VermicelliWindow &operator=(const VermicelliWindow &) = delete;

public:
  explicit VermicelliWindow(std::string name = APP_NAME, glm::u32vec2 dim = {800, 600});

  ~VermicelliWindow() = default;

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  SDL_Window *Get() { return mWindow.Get(); };

  VkExtent2D getExtent() { return {mDim.x, mDim.y}; }
};

}

#endif //__VERMICELLI_VERMICELLI_WINDOW_H__
