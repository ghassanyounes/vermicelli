/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_application.h"
#include "vermicelli_camera.h"
#include "vermicelli_simple_render_system.h"
#include "vermicelli_functions.h"
#include "vermicelli_keyboard_input.h"
#include "vermicelli_buffer.h"
#include <glm/gtc/constants.hpp> // PI
#include <stdexcept>
#include <array>
#include <chrono>
#include <numeric>

using hiResClock = std::chrono::high_resolution_clock;
using duration = std::chrono::duration<float, std::chrono::seconds::period>;

namespace vermicelli {

struct GlobalUbo {
    glm::mat4 mProjectionView{1.0f};
    glm::vec3 mLightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
};

Application::Application(const bool verbose) : mVerbose(verbose) {
  loadGameObjects();
}

Application::~Application() = default;

void Application::run() {
  auto minOffsetAlignment = std::lcm(
          mDevice.mProperties.limits.minUniformBufferOffsetAlignment,
          mDevice.mProperties.limits.nonCoherentAtomSize);

  std::vector<std::unique_ptr<VermicelliBuffer> > uboBuffers(VermicelliSwapChain::MAX_FRAMES_IN_FLIGHT);

  for (auto &uboBuffer: uboBuffers) {
    uboBuffer = std::make_unique<VermicelliBuffer>(
            mDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffer->map();
  }


  VermicelliSimpleRenderSystem simpleRenderSystem{mDevice, mRenderer.getSwapChainRenderPass(), mVerbose};
  VermicelliCamera             camera{};

  if (mVerbose) {
    std::cout << "maxPushConstantSize = " << mDevice.mProperties.limits.maxPushConstantsSize << std::endl;
  }
  bool running = true;

  auto                    viewerObject = VermicelliGameObject::createGameObject();
  VermicelliKeyboardInput cameraController{};

  auto currTime = hiResClock::now();

  while (running) {
    SDL_Event windowEvent;
    auto      eventHappened = SDL_PollEvent(&windowEvent);

    auto  newTime   = hiResClock::now();
    float frameTime = duration(newTime - currTime).count();
    currTime = newTime;
    if (eventHappened) {
      float aspect = mRenderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.0f /*Field of view in degrees*/), aspect, 0.1f, 10.0f);
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

    cameraController.moveInPlaneXZ(frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.mTransform.mTranslation, viewerObject.mTransform.mRotation);
    if (auto commandBuffer = mRenderer.beginFrame()) {
      int       frameIndex = mRenderer.getFrameIndex();
      FrameInfo frameInfo{
              frameIndex,
              frameTime,
              commandBuffer,
              camera
      };
      //update
      GlobalUbo ubo{};
      ubo.mProjectionView = camera.getProjection() * camera.getView();
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      /* TODO:
       * begin offscreen shadow pass
       * render shadow casting objects
       * end offscreen shadow pass
       */

      mRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo, mGameObjects);
      mRenderer.endSwapChainRenderPass(commandBuffer);
      mRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(mDevice.device());
}

void Application::loadGameObjects() {
  std::shared_ptr<VermicelliModel> model = VermicelliModel::createModelFromFile(mDevice, "../models/new_kirb.obj",
                                                                                mVerbose);

  auto gameObject = VermicelliGameObject::createGameObject();
  gameObject.mModel                  = model;
  gameObject.mTransform.mTranslation = {0.0f, 0.0f, 2.5f};
  gameObject.mTransform.mScale       = {0.005f, 0.005f, 0.005f};
  gameObject.mTransform.mRotation    = {0.0f, glm::pi<float>(), 0.0f};

  mGameObjects.push_back(std::move(gameObject));
}

}