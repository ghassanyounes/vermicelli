/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_application.h"
#include "vermicelli_functions.h"
#include <glm/gtc/constants.hpp> // PI
#include <stdexcept>
#include <array>

namespace vermicelli {

Application::Application(const bool verbose) : mVerbose(verbose) {
  loadGameObjects();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

Application::~Application() {
  vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void Application::run() {
  if (mVerbose) {
    std::cout << "maxPushConstantSize = " << mDevice.mProperties.limits.maxPushConstantsSize << std::endl;
  }
  bool running = true;
  while (running) {
    SDL_Event windowEvent;
    while (SDL_PollEvent(&windowEvent)) {
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
      drawFrame();
    }
  }
  vkDeviceWaitIdle(mDevice.device());
}

void Application::createPipelineLayout() {

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset     = 0;
  pushConstantRange.size       = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = 0;
  pipelineLayoutInfo.pSetLayouts            = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;

  if (vkCreatePipelineLayout(mDevice.device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Application::createPipeline() {
  assert(mSwapChain != nullptr && "Cannot create pipeline before swap chain!");
  assert(mPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

  PipelineConfigInfo pipelineConfig{};
  VermicelliPipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.mRenderPass     = mSwapChain->getRenderPass();
  pipelineConfig.mPipelineLayout = mPipelineLayout;
  mPipeline = std::make_unique<VermicelliPipeline>(mDevice, "shaders/simple_shader.vert.spv",
                                                   "shaders/simple_shader.frag.spv", pipelineConfig);
}

void Application::recreateSwapChain() {
  auto extent = mWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = mWindow.getExtent();
    SDL_Event windowEvent;
    SDL_WaitEvent(&windowEvent);
  }

  vkDeviceWaitIdle(mDevice.device());
  if (mSwapChain == nullptr) {
    mSwapChain = std::make_unique<VermicelliSwapChain>(mDevice, extent, mVerbose);
  } else {
    mSwapChain = std::make_unique<VermicelliSwapChain>(mDevice, extent, mVerbose, std::move(mSwapChain));
    if (mSwapChain->imageCount() != mCommandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  //FIXME: If render passes are compatible then do nothing instead of creating a new pipeline
  createPipeline();
}

void Application::createCommandBuffers() {
  mCommandBuffers.resize(mSwapChain->imageCount());
  VkCommandBufferAllocateInfo allocateInfo{};
  allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandPool        = mDevice.getCommandPool();
  allocateInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

  if (vkAllocateCommandBuffers(mDevice.device(), &allocateInfo, mCommandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Unable to allocate command buffers!");
  }
}

void Application::recordCommandBuffer(int imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(mCommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Command buffer failed to begin recording");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass        = mSwapChain->getRenderPass();
  renderPassInfo.framebuffer       = mSwapChain->getFrameBuffer(imageIndex);
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = mSwapChain->getSwapChainExtent();
  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color        = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues    = clearValues.data();

  vkCmdBeginRenderPass(mCommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x        = 0.0f;
  viewport.y        = 0.0f;
  viewport.width    = static_cast<float>(mSwapChain->getSwapChainExtent().width);
  viewport.height   = static_cast<float>(mSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, mSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(mCommandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(mCommandBuffers[imageIndex], 0, 1, &scissor);

  renderGameObjects(mCommandBuffers[imageIndex]);

  vkCmdEndRenderPass(mCommandBuffers[imageIndex]);
  if (vkEndCommandBuffer(mCommandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }

}

void Application::renderGameObjects(VkCommandBuffer commandBuffer) {
  mPipeline->bind(commandBuffer);
  for (auto &obj: mGameObjects) {
    obj.mTransform2d.mRotation = glm::mod(obj.mTransform2d.mRotation + 0.01f, glm::two_pi<float>());

    SimplePushConstantData push{};
    push.offset    = obj.mTransform2d.mTranslation;
    push.color     = obj.mColor;
    push.transform = obj.mTransform2d.mat2();

    vkCmdPushConstants(commandBuffer, mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData),
                       &push);
    obj.mModel->bind(commandBuffer);
    obj.mModel->draw(commandBuffer);
  }
}

void Application::drawFrame() {
  uint32_t imageIndex;
  auto     res = mSwapChain->acquireNextImage(&imageIndex);

  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire next mSwapChain image");
  }

  recordCommandBuffer(imageIndex);
  res = mSwapChain->submitCommandBuffers(&mCommandBuffers[imageIndex], &imageIndex);
  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || mWindow.wasWindowResized()) {
    mWindow.resetWindowResizedFLag();
    recreateSwapChain();
    return;
  }

  if (res != VK_SUCCESS) {
    throw std::runtime_error("Failed to present mSwapChain image");
  }
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

void Application::freeCommandBuffers() {
  vkFreeCommandBuffers(
          mDevice.device(),
          mDevice.getCommandPool(),
          static_cast<uint32_t>(mCommandBuffers.size()),
          mCommandBuffers.data());
  mCommandBuffers.clear();
}

}