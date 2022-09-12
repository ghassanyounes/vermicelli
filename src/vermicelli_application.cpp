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

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<VermicelliModel> createCubeModel(VermicelliDevice &device, glm::vec3 offset, bool verbose) {
  std::vector<VermicelliModel::Vertex> vertices{

          // left face (white)
          {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
          {{-.5f, .5f,  .5f},   {.9f, .9f, .9f}},
          {{-.5f, -.5f, .5f},   {.9f, .9f, .9f}},
          {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
          {{-.5f, .5f,  -.5f},  {.9f, .9f, .9f}},
          {{-.5f, .5f,  .5f},   {.9f, .9f, .9f}},

          // right face (yellow)
          {{.5f,  -.5f, -.5f},  {.8f, .8f, .1f}},
          {{.5f,  .5f,  .5f},   {.8f, .8f, .1f}},
          {{.5f,  -.5f, .5f},   {.8f, .8f, .1f}},
          {{.5f,  -.5f, -.5f},  {.8f, .8f, .1f}},
          {{.5f,  .5f,  -.5f},  {.8f, .8f, .1f}},
          {{.5f,  .5f,  .5f},   {.8f, .8f, .1f}},

          // top face (orange, remember y axis points down)
          {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
          {{.5f,  -.5f, .5f},   {.9f, .6f, .1f}},
          {{-.5f, -.5f, .5f},   {.9f, .6f, .1f}},
          {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
          {{.5f,  -.5f, -.5f},  {.9f, .6f, .1f}},
          {{.5f,  -.5f, .5f},   {.9f, .6f, .1f}},

          // bottom face (red)
          {{-.5f, .5f,  -.5f},  {.8f, .1f, .1f}},
          {{.5f,  .5f,  .5f},   {.8f, .1f, .1f}},
          {{-.5f, .5f,  .5f},   {.8f, .1f, .1f}},
          {{-.5f, .5f,  -.5f},  {.8f, .1f, .1f}},
          {{.5f,  .5f,  -.5f},  {.8f, .1f, .1f}},
          {{.5f,  .5f,  .5f},   {.8f, .1f, .1f}},

          // nose face (blue)
          {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
          {{.5f,  .5f,  0.5f},  {.1f, .1f, .8f}},
          {{-.5f, .5f,  0.5f},  {.1f, .1f, .8f}},
          {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
          {{.5f,  -.5f, 0.5f},  {.1f, .1f, .8f}},
          {{.5f,  .5f,  0.5f},  {.1f, .1f, .8f}},

          // tail face (green)
          {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f,  .5f,  -0.5f}, {.1f, .8f, .1f}},
          {{-.5f, .5f,  -0.5f}, {.1f, .8f, .1f}},
          {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f,  -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f,  .5f,  -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto                            &v: vertices) {
    v.mPosition += offset;
  }
  return std::make_unique<VermicelliModel>(device, vertices, verbose);
}


void Application::loadGameObjects() {
  std::shared_ptr<VermicelliModel> model = createCubeModel(mDevice, {0.0f, 0.0f, 0.0f}, mVerbose);

  auto cube = VermicelliGameObject::createGameObject();
  cube.mModel                  = model;
  cube.mTransform.mTranslation = {0.0f, 0.0f, 0.5f};
  cube.mTransform.mScale       = {0.5f, 0.5f, 0.5f};

  mGameObjects.push_back(std::move(cube));
}

}