//
// Created by bigtiddypiratebf on 9/6/22.
//

#include "vermicelli_window.h"
#include <stdexcept>

namespace vermicelli {

VermicelliWindow::VermicelliWindow(const std::string name, const glm::u32vec2 dim)
        : mDim(dim),
          mName(name),
          mWindow(mName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mDim.x, mDim.y,
                  SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN) {
  initWindow();
}

void VermicelliWindow::initWindow() {
  SDL_Vulkan_LoadLibrary(nullptr);
}

void VermicelliWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (SDL_Vulkan_CreateSurface(mWindow.Get(), instance, surface) != SDL_TRUE) {
    throw std::runtime_error("failed to create window surface!");
  }
}

}