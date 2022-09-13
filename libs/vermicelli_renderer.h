/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/12/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_RENDERER_H__
#define __VERMICELLI_VERMICELLI_RENDERER_H__
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include "vermicelli_window.h"
#include "vermicelli_device.h"
#include "vermicelli_swap_chain.h"
#include <memory>
#include <vector>
#include <cassert>

namespace vermicelli {

class VermicelliRenderer {
  VermicelliWindow                     &mWindow;
  bool                                 mVerbose;
  VermicelliDevice                     &mDevice;
  std::unique_ptr<VermicelliSwapChain> mSwapChain;
  std::vector<VkCommandBuffer>         mCommandBuffers;
  uint32_t                             mCurrentImageIndex;
  int                                  mCurrentFrameIndex = 0;
  bool                                 mIsFrameStarted    = false;

  void createCommandBuffers();

  void freeCommandBuffers();

  void recreateSwapChain();

public:
  explicit VermicelliRenderer(VermicelliWindow &window, VermicelliDevice &device, bool verbose);

  ~VermicelliRenderer();

  VermicelliRenderer(const VermicelliRenderer &) = delete;

  VermicelliRenderer &operator=(const VermicelliRenderer &) = delete;

  VkCommandBuffer beginFrame();

  void endFrame();

  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

  void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

  [[nodiscard]] bool isFrameInProgress() const { return mIsFrameStarted; }

  [[nodiscard]] VkCommandBuffer getCommandBuffer() const {
    assert(mIsFrameStarted && "Cannot get command buffer if frame is not in progress.");
    return mCommandBuffers[mCurrentFrameIndex];
  }

  [[nodiscard]] int getFrameIndex() const {
    assert(mIsFrameStarted && "Cannot get command buffer if frame is not in progress.");
    return mCurrentFrameIndex;
  }

  [[nodiscard]] VkRenderPass getSwapChainRenderPass() const { return mSwapChain->getRenderPass(); }

  [[nodiscard]] float getAspectRatio() const { return mSwapChain->extentAspectRatio(); }
};

}

#endif //__VERMICELLI_VERMICELLI_RENDERER_H__
