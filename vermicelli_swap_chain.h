/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#ifndef __VERMICELLI_VERMICELLI_SWAP_CHAIN_H__
#define __VERMICELLI_VERMICELLI_SWAP_CHAIN_H__
#pragma once

#include "vermicelli_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace vermicelli {

class VermicelliSwapChain {
public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  VermicelliSwapChain(VermicelliDevice &deviceRef, VkExtent2D windowExtent, bool verbose);

  ~VermicelliSwapChain();

  VermicelliSwapChain(const VermicelliSwapChain &) = delete;

  void operator=(const VermicelliSwapChain &) = delete;

  VkFramebuffer getFrameBuffer(int index) { return mSwapChainFrameBuffers[index]; }

  VkRenderPass getRenderPass() { return mRenderPass; }

  VkImageView getImageView(int index) { return mSwapChainImageViews[index]; }

  size_t imageCount() { return mSwapChainImages.size(); }

  VkFormat getSwapChainImageFormat() { return mSwapChainImageFormat; }

  VkExtent2D getSwapChainExtent() { return mSwapChainExtent; }

  [[nodiscard]] glm::u32vec2 dim() const { return {mSwapChainExtent.width, mSwapChainExtent.height}; }

  [[nodiscard]] float extentAspectRatio() const {
    return static_cast<float>(mSwapChainExtent.width) / static_cast<float>(mSwapChainExtent.height);
  }

  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex);

  VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

private:
  void createSwapChain();

  void createImageViews();

  void createDepthResources();

  void createRenderPass();

  void createFrameBuffers();

  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
          const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
          const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat   mSwapChainImageFormat;
  VkExtent2D mSwapChainExtent;

  std::vector<VkFramebuffer> mSwapChainFrameBuffers;
  VkRenderPass               mRenderPass;

  std::vector<VkImage>        mDepthImages;
  std::vector<VkDeviceMemory> mDepthImageMemoryVec;
  std::vector<VkImageView>    mDepthImageViews;
  std::vector<VkImage>        mSwapChainImages;
  std::vector<VkImageView>    mSwapChainImageViews;

  VermicelliDevice &mDevice;
  VkExtent2D       mWindowExtent;

  VkSwapchainKHR mSwapChain;

  std::vector<VkSemaphore> mImageAvailableSemaphores;
  std::vector<VkSemaphore> mRenderFinishedSemaphores;
  std::vector<VkFence>     mInFlightFences;
  std::vector<VkFence>     mImagesInFlight;
  size_t                   mCurrentFrame = 0;
  bool                     mVerbose;
};

}  // namespace vermicelli
#endif //__VERMICELLI_VERMICELLI_SWAP_CHAIN_H__
