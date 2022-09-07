/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "application.h"
#include <stdexcept>
#include <array>

namespace vermicelli {

Application::Application() {
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

Application::~Application() {
  vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void Application::run() {
  bool running = true;
  while (running) {
    SDL_Event windowEvent;
    while (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) {
        running = false;
        break;
      }
      drawFrame();
    }
  }
  vkDeviceWaitIdle(mDevice.device());
}

void Application::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = 0;
  pipelineLayoutInfo.pSetLayouts            = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges    = nullptr;

  if (vkCreatePipelineLayout(mDevice.device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void Application::createPipeline() {
  PipelineConfigInfo pipelineConfig{};
  VermicelliPipeline::defaultPipelineConfigInfo(pipelineConfig, mSwapChain.dim());
  pipelineConfig.renderPass     = mSwapChain.getRenderPass();
  pipelineConfig.pipelineLayout = mPipelineLayout;
  mPipeline = std::make_unique<VermicelliPipeline>(mDevice, "shaders/simple_shader.vert.spv",
                                                   "shaders/simple_shader.frag.spv", pipelineConfig);
}

void Application::createCommandBuffers() {
  mCommandBuffers.resize(mSwapChain.imageCount());
  VkCommandBufferAllocateInfo allocateInfo{};
  allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandPool        = mDevice.getCommandPool();
  allocateInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

  if (vkAllocateCommandBuffers(mDevice.device(), &allocateInfo, mCommandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Unable to allocate command buffers!");
  }

  for (int i = 0; i < mCommandBuffers.size(); ++i) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("Command buffer failed to begin recording");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = mSwapChain.getRenderPass();
    renderPassInfo.framebuffer       = mSwapChain.getFrameBuffer(i);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapChain.getSwapChainExtent();
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color        = {0.4f, 0.855f, 0.46f, 0.71f};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues    = clearValues.data();

    vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    mPipeline->bind(mCommandBuffers[i]);
    // Records a draw command to draw 3 vertices and only 1 instance
    vkCmdDraw(mCommandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(mCommandBuffers[i]);
    if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to record command buffer");
    }

  }
}

void Application::drawFrame() {
  uint32_t imageIndex;
  auto     res = mSwapChain.acquireNextImage(&imageIndex);

  ///<< FIXME: Will crash when resizing window as image resets, will be implemented;
  if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire next swapChain image");
  }

  res = mSwapChain.submitCommandBuffers(&mCommandBuffers[imageIndex], &imageIndex);
  if (res != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swapChain image");
  }
}

}