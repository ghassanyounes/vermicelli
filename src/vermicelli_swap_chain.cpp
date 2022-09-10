/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_swap_chain.h"
// std
#include <array>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace vermicelli {

VermicelliSwapChain::VermicelliSwapChain(VermicelliDevice &deviceRef, VkExtent2D windowExtent, const bool verbose)
        : mDevice{deviceRef}, mWindowExtent{windowExtent}, mVerbose{verbose} {
  init();
}

void VermicelliSwapChain::init() {
  createSwapChain();
  createImageViews();
  createRenderPass();
  createDepthResources();
  createFrameBuffers();
  createSyncObjects();
}

VermicelliSwapChain::VermicelliSwapChain(vermicelli::VermicelliDevice &deviceRef, VkExtent2D windowExtent, bool verbose,
                                         std::shared_ptr<VermicelliSwapChain> previous) : mDevice{deviceRef},
                                                                                          mWindowExtent{windowExtent},
                                                                                          mVerbose{verbose},
                                                                                          mPreviousSwapChain{previous} {
  init();

  /// Clean up old swap chain since it's no longer needed
  mPreviousSwapChain = nullptr;
}

VermicelliSwapChain::~VermicelliSwapChain() {
  for (auto imageView: mSwapChainImageViews) {
    vkDestroyImageView(mDevice.device(), imageView, nullptr);
  }
  mSwapChainImageViews.clear();

  if (mSwapChain != nullptr) {
    vkDestroySwapchainKHR(mDevice.device(), mSwapChain, nullptr);
    mSwapChain = nullptr;
  }

  for (int i = 0; i < mDepthImages.size(); i++) {
    vkDestroyImageView(mDevice.device(), mDepthImageViews[i], nullptr);
    vkDestroyImage(mDevice.device(), mDepthImages[i], nullptr);
    vkFreeMemory(mDevice.device(), mDepthImageMemoryVec[i], nullptr);
  }

  for (auto framebuffer: mSwapChainFrameBuffers) {
    vkDestroyFramebuffer(mDevice.device(), framebuffer, nullptr);
  }

  vkDestroyRenderPass(mDevice.device(), mRenderPass, nullptr);

  // cleanup synchronization objects
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(mDevice.device(), mRenderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(mDevice.device(), mImageAvailableSemaphores[i], nullptr);
    vkDestroyFence(mDevice.device(), mInFlightFences[i], nullptr);
  }
}

VkResult VermicelliSwapChain::acquireNextImage(uint32_t *imageIndex) {
  vkWaitForFences(
          mDevice.device(),
          1,
          &mInFlightFences[mCurrentFrame],
          VK_TRUE,
          std::numeric_limits<uint64_t>::max());

  VkResult result = vkAcquireNextImageKHR(
          mDevice.device(),
          mSwapChain,
          std::numeric_limits<uint64_t>::max(),
          mImageAvailableSemaphores[mCurrentFrame],  // must be a not signaled semaphore
          VK_NULL_HANDLE,
          imageIndex);

  return result;
}

VkResult VermicelliSwapChain::submitCommandBuffers(
        const VkCommandBuffer *buffers, const uint32_t *imageIndex) {
  if (mImagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(mDevice.device(), 1, &mImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
  }
  mImagesInFlight[*imageIndex] = mInFlightFences[mCurrentFrame];

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore          waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
  VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores    = waitSemaphores;
  submitInfo.pWaitDstStageMask  = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = buffers;

  VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = signalSemaphores;

  vkResetFences(mDevice.device(), 1, &mInFlightFences[mCurrentFrame]);
  if (vkQueueSubmit(mDevice.graphicsQueue(), 1, &submitInfo, mInFlightFences[mCurrentFrame]) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = signalSemaphores;

  VkSwapchainKHR swapChains[] = {mSwapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains    = swapChains;

  presentInfo.pImageIndices = imageIndex;

  auto result = vkQueuePresentKHR(mDevice.presentQueue(), &presentInfo);

  mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

  return result;
}

void VermicelliSwapChain::createSwapChain() {
  SwapChainSupportDetails swapChainSupport = mDevice.getSwapChainSupport();

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.mFormats);
  VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.mPresentModes);
  VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.mCapabilities);

  uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;
  if (swapChainSupport.mCapabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.mCapabilities.maxImageCount) {
    imageCount = swapChainSupport.mCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = mDevice.surface();

  createInfo.minImageCount    = imageCount;
  createInfo.imageFormat      = surfaceFormat.format;
  createInfo.imageColorSpace  = surfaceFormat.colorSpace;
  createInfo.imageExtent      = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices              = mDevice.findPhysicalQueueFamilies();
  uint32_t           queueFamilyIndices[] = {indices.mGraphicsFamily, indices.mPresentFamily};

  if (indices.mGraphicsFamily != indices.mPresentFamily) {
    createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices   = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;      // Optional
    createInfo.pQueueFamilyIndices   = nullptr;  // Optional
  }

  createInfo.preTransform   = swapChainSupport.mCapabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = presentMode;
  createInfo.clipped     = VK_TRUE;

  createInfo.oldSwapchain = mPreviousSwapChain == nullptr ? VK_NULL_HANDLE : mPreviousSwapChain->mSwapChain;

  if (vkCreateSwapchainKHR(mDevice.device(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  // we only specified a minimum number of images in the swap chain, so the implementation is
  // allowed to create a swap chain with more. That's why we'll first query the final number of
  // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
  // retrieve the handles.
  vkGetSwapchainImagesKHR(mDevice.device(), mSwapChain, &imageCount, nullptr);
  mSwapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(mDevice.device(), mSwapChain, &imageCount, mSwapChainImages.data());

  mSwapChainImageFormat = surfaceFormat.format;
  mSwapChainExtent      = extent;
}

void VermicelliSwapChain::createImageViews() {
  mSwapChainImageViews.resize(mSwapChainImages.size());
  for (size_t i = 0; i < mSwapChainImages.size(); i++) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = mSwapChainImages[i];
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = mSwapChainImageFormat;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(mDevice.device(), &viewInfo, nullptr, &mSwapChainImageViews[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }
  }
}

void VermicelliSwapChain::createRenderPass() {
  VkAttachmentDescription depthAttachment{};
  depthAttachment.format         = findDepthFormat();
  depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format         = getSwapChainImageFormat();
  colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
  dependency.srcAccessMask = 0;
  dependency.srcStageMask =
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstSubpass = 0;
  dependency.dstStageMask  =
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask =
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments    = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo                 renderPassInfo = {};
  renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments    = attachments.data();
  renderPassInfo.subpassCount    = 1;
  renderPassInfo.pSubpasses      = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies   = &dependency;

  if (vkCreateRenderPass(mDevice.device(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void VermicelliSwapChain::createFrameBuffers() {
  mSwapChainFrameBuffers.resize(imageCount());
  for (size_t i = 0; i < imageCount(); i++) {
    std::array<VkImageView, 2> attachments = {mSwapChainImageViews[i], mDepthImageViews[i]};

    VkExtent2D              swapChainExtent = getSwapChainExtent();
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = mRenderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = swapChainExtent.width;
    framebufferInfo.height          = swapChainExtent.height;
    framebufferInfo.layers          = 1;

    if (vkCreateFramebuffer(
            mDevice.device(),
            &framebufferInfo,
            nullptr,
            &mSwapChainFrameBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void VermicelliSwapChain::createDepthResources() {
  VkFormat   depthFormat     = findDepthFormat();
  VkExtent2D swapChainExtent = getSwapChainExtent();

  mDepthImages.resize(imageCount());
  mDepthImageMemoryVec.resize(imageCount());
  mDepthImageViews.resize(imageCount());

  for (int i = 0; i < mDepthImages.size(); i++) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = swapChainExtent.width;
    imageInfo.extent.height = swapChainExtent.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = depthFormat;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags         = 0;

    mDevice.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mDepthImages[i],
            mDepthImageMemoryVec[i]);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = mDepthImages[i];
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = depthFormat;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(mDevice.device(), &viewInfo, nullptr, &mDepthImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }
  }
}

void VermicelliSwapChain::createSyncObjects() {
  mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  mImagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(mDevice.device(), &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) !=
        VK_SUCCESS ||
        vkCreateSemaphore(mDevice.device(), &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) !=
        VK_SUCCESS ||
        vkCreateFence(mDevice.device(), &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}

VkSurfaceFormatKHR VermicelliSwapChain::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat: availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR VermicelliSwapChain::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes) const {
  for (const auto &availablePresentMode: availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
      //if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      // Mailbox can be better, but it was going too fast, so I switched it to vsync (fifo)
      if (mVerbose) {
        std::cout << "Present mode: "
                  << ((availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) ? "Mailbox" : "V-Sync") << std::endl;
      }
      return availablePresentMode;
    }
  }
  if (mVerbose) {
    std::cout << "Present mode: V-Sync" << std::endl;
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VermicelliSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = mWindowExtent;
    actualExtent.width  = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}

VkFormat VermicelliSwapChain::findDepthFormat() {
  return mDevice.findSupportedFormat(
          {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
          VK_IMAGE_TILING_OPTIMAL,
          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

}  // namespace vermicelli