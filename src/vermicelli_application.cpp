/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_application.h"
#include "vermicelli_camera.h"
#include "systems/vermicelli_simple_render_system.h"
#include "systems/vermicelli_point_light_system.h"
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

Application::Application(const bool verbose) : mVerbose(verbose) {
  mGlobalPool = VermicelliDescriptorPool::Builder(mDevice)
          .setMaxSets(VermicelliSwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VermicelliSwapChain::MAX_FRAMES_IN_FLIGHT)
          .build();
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

  auto globalSetLayout = VermicelliDescriptorSetLayout::Builder(mDevice)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
          .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(VermicelliSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int                     i = 0; i < globalDescriptorSets.size(); ++i) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    VermicelliDescriptorWriter(*globalSetLayout, *mGlobalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
  }

  VermicelliSimpleRenderSystem simpleRenderSystem{mDevice, mRenderer.getSwapChainRenderPass(),
                                                  globalSetLayout->getDescriptorSetLayout(), mVerbose};
  VermicelliPointLightSystem   pointLightSystem{mDevice, mRenderer.getSwapChainRenderPass(),
                                                globalSetLayout->getDescriptorSetLayout(), mVerbose};
  VermicelliCamera             camera{};

  if (mVerbose) {
    std::cout << "maxPushConstantSize = " << mDevice.mProperties.limits.maxPushConstantsSize << std::endl;
  }
  bool running = true;

  auto viewerObject = VermicelliGameObject::createGameObject();
  viewerObject.mTransform.mTranslation.z = -5.5f;
  viewerObject.mTransform.mTranslation.y = -2.0f;
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
      camera.setPerspectiveProjection(glm::radians(50.0f /*Field of view in degrees*/), aspect, 0.01f, 100.0f);
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
              camera,
              globalDescriptorSets[frameIndex],
              mGameObjects
      };
      //update
      GlobalUbo ubo{};
      ubo.mProjection  = camera.getProjection();
      ubo.mView        = camera.getView();
      ubo.mInverseView = camera.getInverseView();
      pointLightSystem.update(frameInfo, ubo);
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      /* TODO:
       * begin offscreen shadow pass
       * render shadow casting objects
       * end offscreen shadow pass
       */

      mRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo);
      pointLightSystem.render(frameInfo);
      mRenderer.endSwapChainRenderPass(commandBuffer);
      mRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(mDevice.device());
}

void Application::loadGameObjects() {
  std::shared_ptr<VermicelliModel> model = VermicelliModel::createModelFromFile(mDevice, "../models/new_kirb.obj",
                                                                                mVerbose);

  auto kirby = VermicelliGameObject::createGameObject();
  kirby.mModel                  = model;
  kirby.mTransform.mTranslation = {-0.15f, 0.0f, 0.075f};
  kirby.mTransform.mScale       = {0.005f, 0.005f, 0.005f};
  kirby.mTransform.mRotation    = {0.0f, glm::pi<float>(), 0.0f};

  mGameObjects.emplace(kirby.getID(), std::move(kirby));

  model = VermicelliModel::createModelFromFile(mDevice, "../models/icosahedron.obj", mVerbose);

  auto cube = VermicelliGameObject::createGameObject();
  cube.mModel                  = model;
  cube.mTransform.mTranslation = {0.0f, -5.0f, 0.0f};
  cube.mTransform.mScale       = {0.1f, 0.1f, 0.1f};
  cube.mTransform.mRotation    = glm::vec3{0.0f, 0.0f, 0.0f};

  mGameObjects.emplace(cube.getID(), std::move(cube));

  model = VermicelliModel::createModelFromFile(mDevice, "../models/quad.obj", mVerbose);

  auto                   floor = VermicelliGameObject::createGameObject();
  floor.mModel                  = model;
  floor.mTransform.mTranslation = {0.0f, 0.0f, 0.0f};
  floor.mTransform.mScale       = {15.0f, 1.0f, 15.0f};

  mGameObjects.emplace(floor.getID(), std::move(floor));

  { // Placed in a nested block to reuse it if needed
    auto pointLight = VermicelliGameObject::makePointLight(2.0f, 1.0f, color::white);
    pointLight.mTransform.mTranslation = glm::vec3(0.0f, -10.0f, 0.0f);
    mGameObjects.emplace(pointLight.getID(), std::move(pointLight));
  }
  std::vector<glm::vec3> rainbow{
          color::pink,
          color::magenta,
          color::red,
          color::brown,
          color::orange,
          color::yellow,
          color::lime,
          color::green,
          color::olive,
          color::teal,
          color::cyan,
          color::blue,
          color::indigo,
          color::violet,
          color::purple
  };
  for (int               i      = 0; i < rainbow.size(); ++i) {
    auto pointLight  = VermicelliGameObject::makePointLight(1.0f, 0.1f, rainbow[i]);
    auto rotateLight = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>()) / rainbow.size(), {0.0f, -1.0f, 0.0f});
    pointLight.mTransform.mTranslation = glm::vec3(rotateLight * glm::vec4(-2.5f, -4.5f, -2.5f, 1.0f));
    mGameObjects.emplace(pointLight.getID(), std::move(pointLight));
  }
}

}