/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/20/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_BUFFER_H__
#define __VERMICELLI_VERMICELLI_BUFFER_H__
#pragma once

#include "vermicelli_device.h"

namespace vermicelli {

class VermicelliBuffer {
public:
  VermicelliBuffer(
          VermicelliDevice &device,
          VkDeviceSize instanceSize,
          uint32_t instanceCount,
          VkBufferUsageFlags usageFlags,
          VkMemoryPropertyFlags memoryPropertyFlags,
          VkDeviceSize minOffsetAlignment = 1);

  ~VermicelliBuffer();

  VermicelliBuffer(const VermicelliBuffer &) = delete;

  VermicelliBuffer &operator=(const VermicelliBuffer &) = delete;

  VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  void unmap();

  void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  void writeToIndex(void *data, int index);

  VkResult flushIndex(int index);

  VkDescriptorBufferInfo descriptorInfoForIndex(int index);

  VkResult invalidateIndex(int index);

  [[nodiscard]] VkBuffer getBuffer() const { return mBuffer; }

  [[nodiscard]] void *getMappedMemory() const { return mMapped; }

  [[nodiscard]] uint32_t getInstanceCount() const { return mInstanceCount; }

  [[nodiscard]] VkDeviceSize getInstanceSize() const { return mInstanceSize; }

  [[nodiscard]] VkDeviceSize getAlignmentSize() const { return mInstanceSize; }

  [[nodiscard]] VkBufferUsageFlags getUsageFlags() const { return mUsageFlags; }

  [[nodiscard]] VkMemoryPropertyFlags getMemoryPropertyFlags() const { return mMemoryPropertyFlags; }

  [[nodiscard]] VkDeviceSize getBufferSize() const { return mBufferSize; }

private:
  static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

  VermicelliDevice &vermicelliDevice;
  void             *mMapped = nullptr;
  VkBuffer       mBuffer = VK_NULL_HANDLE;
  VkDeviceMemory mMemory = VK_NULL_HANDLE;

  VkDeviceSize          mBufferSize;
  uint32_t              mInstanceCount;
  VkDeviceSize          mInstanceSize;
  VkDeviceSize          mAlignmentSize;
  VkBufferUsageFlags    mUsageFlags;
  VkMemoryPropertyFlags mMemoryPropertyFlags;
};

}  // namespace vermicelli

#endif //__VERMICELLI_VERMICELLI_BUFFER_H__
