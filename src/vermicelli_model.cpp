/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/8/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include "vermicelli_model.h"
#include <cassert>

namespace vermicelli {

VermicelliModel::VermicelliModel(VermicelliDevice &device, const std::vector<Vertex> &vertices,
                                 bool verbose) : mDevice{device}, mVerbose(verbose) {
  createVertexBuffers(vertices);
}

VermicelliModel::~VermicelliModel() {
  vkDestroyBuffer(mDevice.device(), mVertexBuffer, nullptr);
  vkFreeMemory(mDevice.device(), mVertexBufferMemory, nullptr);
}

void VermicelliModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer     buffers[] = {mVertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void VermicelliModel::draw(VkCommandBuffer commandBuffer) {
  vkCmdDraw(commandBuffer, mVertexCount, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> VermicelliModel::Vertex::getBindingDescriptions() {
  return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}

std::vector<VkVertexInputAttributeDescription> VermicelliModel::Vertex::getAttributeDescriptions() {
  return {
          {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mPosition)}, // position
          {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mColor)}     // color
  };
}

void VermicelliModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
  mVertexCount = static_cast<uint32_t>(vertices.size());
  assert(mVertexCount >= 3 && "Vertex count must be >= 3");
  /// total number of bytes required for our vertex buffer to store all the vertices of the model
  VkDeviceSize bufferSize = sizeof(vertices[0]) * mVertexCount;
  mDevice.createBuffer(
          bufferSize,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
          mVertexBuffer,
          mVertexBufferMemory);
  void *data;
  /// Create a region of host memory mapped to device memory and sets data to point to beginning of mapped memory range
  vkMapMemory(mDevice.device(), mVertexBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(mDevice.device(), mVertexBufferMemory);
}

}