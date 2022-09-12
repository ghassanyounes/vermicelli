/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_application.h"
#include "vermicelli_simple_render_system.h"
#include "vermicelli_functions.h"
#include <glm/gtc/constants.hpp> // PI
#include <stdexcept>
#include <array>

namespace vermicelli {

Application::Application(const bool verbose) : mVerbose(verbose) {
  loadGameObjects();
}

Application::~Application() {}

void Application::run() {
  VermicelliSimpleRenderSystem simpleRenderSystem{mDevice, mRenderer.getSwapChainRenderPass(), mVerbose};

  if (mVerbose) {
    std::cout << "maxPushConstantSize = " << mDevice.mProperties.limits.maxPushConstantsSize << std::endl;
  }
  bool running = true;
  while (running) {
    SDL_Event windowEvent;
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) {
        running = false;
        break;
      } else if (windowEvent.type == SDL_KEYDOWN) {
        switch (windowEvent.key.keysym.sym) {
          case SDLK_ESCAPE:
            running = false;
            break;
        }
      }
    }
    if (auto commandBuffer = mRenderer.beginFrame()) {

      /* TODO:
       * begin offscreen shadow pass
       * render shadow casting objects
       * end offscreen shadow pass
       */

      mRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, mGameObjects);
      mRenderer.endSwapChainRenderPass(commandBuffer);
      mRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(mDevice.device());
}

void Application::loadGameObjects() {
  std::vector<VermicelliModel::Vertex> vertices{
          {{0.0,    -0.5}, {0.0f,  1.0f,  1.0f}},
          {{0.25,   0.0},  {1.0f,  0.0f,  1.0f}},
          {{-0.25,  0.0},  {1.0f,  1.0f,  0.0f}},
          //
          {{0.0,    0.5},  {1.0f,  0.0f,  0.0f}},
          {{-0.5,   0.5},  {0.0f,  1.0f,  0.0f}},
          {{-0.25,  0.0},  {0.0f,  0.0f,  1.0f}},
          //
          {{0.0,    0.5},  {0.5f,  0.25f, 0.0f}},
          {{0.25,   0.0},  {0.0f,  0.5f,  0.25f}},
          {{0.5,    0.5},  {0.25f, 0.0f,  0.5f}},
          //
          {{0,      0},    {0.0f,  0.0f,  0.0f}},
          {{0.125,  0.25}, {0.5f,  0.5f,  0.5f}},
          {{-0.125, 0.25}, {1.0f,  1.0f,  1.0f}}
  };

  auto Model    = std::make_shared<VermicelliModel>(mDevice, vertices, mVerbose);
  auto Triangle = VermicelliGameObject::createGameObject();
  Triangle.mModel                    = Model;
  Triangle.mColor                    = color::hex("1f7a4d");
  Triangle.mTransform2d.mTranslation = {0.0f, 0.0f};
  Triangle.mTransform2d.mScale       = {1.0f, 1.0f};
  Triangle.mTransform2d.mRotation    = 0.25f * glm::two_pi<float>();

  mGameObjects.push_back(std::move(Triangle));
}

}