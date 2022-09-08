/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#ifndef __VERMICELLI_VERMICELLI_DEVICE_H__
#define __VERMICELLI_VERMICELLI_DEVICE_H__

#include "vermicelli_window.h"

#include <string>
#include <vector>

namespace vermicelli {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR        mCapabilities;
    std::vector<VkSurfaceFormatKHR> mFormats;
    std::vector<VkPresentModeKHR>   mPresentModes;
};

struct QueueFamilyIndices {
    uint32_t mGraphicsFamily;
    uint32_t mPresentFamily;
    bool     mGraphicsFamilyHasValue = false;
    bool     mPresentFamilyHasValue  = false;

    [[nodiscard]] bool isComplete() const { return mGraphicsFamilyHasValue && mPresentFamilyHasValue; }
};

class VermicelliDevice {
  void createInstance();

  void setupDebugMessenger();

  void createSurface();

  void pickPhysicalDevice();

  void createLogicalDevice();

  void createCommandPool();

  // helper functions
  bool isDeviceSuitable(VkPhysicalDevice device);

  std::vector<const char *> getRequiredExtensions();

  bool checkValidationLayerSupport();

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  void hasSDL2RequiredInstanceExtensions(bool verbose);

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  VkInstance               mInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;
  VkPhysicalDevice         mPhysicalDevice = VK_NULL_HANDLE;
  VermicelliWindow         &mWindow;
  VkCommandPool            mCommandPool;
  bool                     mVerbose;

  VkDevice     mDevice_;
  VkSurfaceKHR mSurface_;
  VkQueue      mGraphicsQueue_;
  VkQueue      mPresentQueue_;

  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
public:
#ifdef NDEBUG
  const bool mEnableValidationLayers = false;
#else
  const bool mEnableValidationLayers = true;
#endif

  explicit VermicelliDevice(VermicelliWindow &window, const bool verbose);

  ~VermicelliDevice();

  // Not copyable or movable
  VermicelliDevice(const VermicelliDevice &) = delete;

  void operator=(const VermicelliDevice &) = delete;

  VermicelliDevice(VermicelliDevice &&) = delete;

  VermicelliDevice &operator=(VermicelliDevice &&) = delete;

  VkCommandPool getCommandPool() { return mCommandPool; }

  VkDevice device() { return mDevice_; }

  VkSurfaceKHR surface() { return mSurface_; }

  VkQueue graphicsQueue() { return mGraphicsQueue_; }

  VkQueue presentQueue() { return mPresentQueue_; }

  SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(mPhysicalDevice); }

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(mPhysicalDevice); }

  VkFormat findSupportedFormat(
          const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

  // Buffer Helper Functions
  void createBuffer(
          VkDeviceSize size,
          VkBufferUsageFlags usage,
          VkMemoryPropertyFlags properties,
          VkBuffer &buffer,
          VkDeviceMemory &bufferMemory);

  VkCommandBuffer beginSingleTimeCommands();

  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  void copyBufferToImage(
          VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

  void createImageWithInfo(
          const VkImageCreateInfo &imageInfo,
          VkMemoryPropertyFlags properties,
          VkImage &image,
          VkDeviceMemory &imageMemory);

  VkPhysicalDeviceProperties mProperties;
};
}

#endif //__VERMICELLI_VERMICELLI_DEVICE_H__
