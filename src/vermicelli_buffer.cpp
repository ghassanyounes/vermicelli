/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/20/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_buffer.h"

#include <cassert>
#include <cstring>

namespace vermicelli {

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize VermicelliBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0) {
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
  }
  return instanceSize;
}

VermicelliBuffer::VermicelliBuffer(
        VermicelliDevice &device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : vermicelliDevice{device},
          mInstanceSize{instanceSize},
          mInstanceCount{instanceCount},
          mUsageFlags{usageFlags},
          mMemoryPropertyFlags{memoryPropertyFlags} {
  mAlignmentSize = getAlignment(instanceSize, minOffsetAlignment);
  mBufferSize    = mAlignmentSize * instanceCount;
  device.createBuffer(mBufferSize, usageFlags, memoryPropertyFlags, mBuffer, mMemory);
}

VermicelliBuffer::~VermicelliBuffer() {
  unmap();
  vkDestroyBuffer(vermicelliDevice.device(), mBuffer, nullptr);
  vkFreeMemory(vermicelliDevice.device(), mMemory, nullptr);
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
VkResult VermicelliBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
  assert(mBuffer && mMemory && "Called map on buffer before create");
  return vkMapMemory(vermicelliDevice.device(), mMemory, offset, size, 0, &mMapped);
}

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void VermicelliBuffer::unmap() {
  if (mMapped) {
    vkUnmapMemory(vermicelliDevice.device(), mMemory);
    mMapped = nullptr;
  }
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void VermicelliBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
  assert(mMapped && "Cannot copy to unmapped buffer");

  if (size == VK_WHOLE_SIZE) {
    memcpy(mMapped, data, mBufferSize);
  } else {
    char *memOffset = (char *) mMapped;
    memOffset += offset;
    memcpy(memOffset, data, size);
  }
}

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
VkResult VermicelliBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = mMemory;
  mappedRange.offset = offset;
  mappedRange.size   = size;
  return vkFlushMappedMemoryRanges(vermicelliDevice.device(), 1, &mappedRange);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
VkResult VermicelliBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = mMemory;
  mappedRange.offset = offset;
  mappedRange.size   = size;
  return vkInvalidateMappedMemoryRanges(vermicelliDevice.device(), 1, &mappedRange);
}

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
VkDescriptorBufferInfo VermicelliBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
  return VkDescriptorBufferInfo{
          mBuffer,
          offset,
          size,
  };
}

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void VermicelliBuffer::writeToIndex(void *data, int index) {
  writeToBuffer(data, mInstanceSize, index * mAlignmentSize);
}

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult VermicelliBuffer::flushIndex(int index) { return flush(mAlignmentSize, index * mAlignmentSize); }

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo VermicelliBuffer::descriptorInfoForIndex(int index) {
  return descriptorInfo(mAlignmentSize, index * mAlignmentSize);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
VkResult VermicelliBuffer::invalidateIndex(int index) {
  return invalidate(mAlignmentSize, index * mAlignmentSize);
}

}  // namespace vermicelli