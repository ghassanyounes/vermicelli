/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_window.h"
#include <stdexcept>

namespace vermicelli {

VermicelliWindow::VermicelliWindow(const std::string name, const glm::u32vec2 dim)
        : mDim(dim),
          mName(name),
          mWindow(mName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mDim.x, mDim.y,
                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN) {
  initWindow();
  SDL_SetWindowData(mWindow.Get(), "window_userData_frame", this);

  /// This is equivalent to glfwSetFrameBufferSizeCallback implementation, requires some handling of events
  SDL_AddEventWatch(reinterpret_cast<SDL_EventFilter>(VermicelliWindow::frameBufferResizeCallback), mWindow.Get());
}

void VermicelliWindow::initWindow() {
  SDL_Vulkan_LoadLibrary(nullptr);
}

void VermicelliWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (SDL_Vulkan_CreateSurface(mWindow.Get(), instance, surface) != SDL_TRUE) {
    throw std::runtime_error("failed to create window surface!");
  }
}

int VermicelliWindow::frameBufferResizeCallback(void *userData, SDL_Event *event) {
  /// Check to see if it was a resize event - that allows it to be a specific callback
  if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    auto Window = reinterpret_cast<VermicelliWindow *>(SDL_GetWindowData((SDL_Window *) userData,
                                                                         "window_userData_frame"));
    Window->mFrameBufferResized = true;
    Window->mDim                = {event->window.data1, event->window.data2};
  }
  return 1;
}


}