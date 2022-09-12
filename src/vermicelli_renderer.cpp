/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/12/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_renderer.h"
#include "vermicelli_functions.h"
#include <stdexcept>
#include <array>

namespace vermicelli {

VermicelliRenderer::VermicelliRenderer(VermicelliWindow &window, VermicelliDevice &device, const bool verbose)
        : mWindow(window), mDevice(device), mVerbose(verbose) {
  recreateSwapChain();
  createCommandBuffers();
}

VermicelliRenderer::~VermicelliRenderer() {
  freeCommandBuffers();
}

void VermicelliRenderer::recreateSwapChain() {
  auto extent = mWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = mWindow.getExtent();
    SDL_Event windowEvent;
    ///< This is essentially creating a callback for each window event - see `vermicelli_window.cpp` for more info.
    SDL_WaitEvent(&windowEvent);
  }

  vkDeviceWaitIdle(mDevice.device());
  if (mSwapChain == nullptr) {
    mSwapChain = std::make_unique<VermicelliSwapChain>(mDevice, extent, mVerbose);
  } else {
    std::shared_ptr<VermicelliSwapChain> oldSwapChain = std::move(mSwapChain);
    mSwapChain = std::make_unique<VermicelliSwapChain>(mDevice, extent, mVerbose, oldSwapChain);

    if (!oldSwapChain->compareSwapFormats(*mSwapChain.get())) {
      throw std::runtime_error("Swap chain image/depth format has changed");
      // Fixme: Create callback function notifying the app that a new incompatible render pass has been created
    }
  }

}

void VermicelliRenderer::createCommandBuffers() {
  mCommandBuffers.resize(VermicelliSwapChain::MAX_FRAMES_IN_FLIGHT);
  VkCommandBufferAllocateInfo allocateInfo{};
  allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandPool        = mDevice.getCommandPool();
  allocateInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

  if (vkAllocateCommandBuffers(mDevice.device(), &allocateInfo, mCommandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("Unable to allocate command buffers!");
  }
}

void VermicelliRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(
          mDevice.device(),
          mDevice.getCommandPool(),
          static_cast<uint32_t>(mCommandBuffers.size()),
          mCommandBuffers.data());
  mCommandBuffers.clear();
}

VkCommandBuffer VermicelliRenderer::beginFrame() {
  assert(!mIsFrameStarted && "Cannot call beginFrame while already in progress");
  auto res = mSwapChain->acquireNextImage(&mCurrentImageIndex);

  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire next mSwapChain image");
  }
  mIsFrameStarted = true;
  auto                     cmdBuffer = getCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Command buffer failed to begin recording");
  }
  return cmdBuffer;
}

void VermicelliRenderer::endFrame() {
  assert(mIsFrameStarted && "Cannot call endFrame while frame is not in progress");
  auto cmdBuffer = getCommandBuffer();
  if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Failed to record command buffer");
  }

  auto res = mSwapChain->submitCommandBuffers(&cmdBuffer, &mCurrentImageIndex);
  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || mWindow.wasWindowResized()) {
    mWindow.resetWindowResizedFLag();
    recreateSwapChain();
  } else if (res != VK_SUCCESS) {
    throw std::runtime_error("Failed to present mSwapChain image");
  }
  mIsFrameStarted    = false;
  mCurrentFrameIndex = (mCurrentFrameIndex + 1) % VermicelliSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VermicelliRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(mIsFrameStarted && "Cannot call beginSwapChainRenderPass while frame is not in progress");
  assert(commandBuffer == getCommandBuffer() && "Can't begin render pass on a command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass        = mSwapChain->getRenderPass();
  renderPassInfo.framebuffer       = mSwapChain->getFrameBuffer(mCurrentImageIndex);
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = mSwapChain->getSwapChainExtent();
  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color        = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues    = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x        = 0.0f;
  viewport.y        = 0.0f;
  viewport.width    = static_cast<float>(mSwapChain->getSwapChainExtent().width);
  viewport.height   = static_cast<float>(mSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, mSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

}

void VermicelliRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) const {
  assert(mIsFrameStarted && "Cannot call endSwapChainRenderPass while frame is not in progress");
  assert(commandBuffer == getCommandBuffer() && "Can't end render pass on a command buffer from a different frame");

  vkCmdEndRenderPass(commandBuffer);
}

}