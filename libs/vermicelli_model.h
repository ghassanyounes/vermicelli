/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/8/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_MODEL_H__
#define __VERMICELLI_VERMICELLI_MODEL_H__
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "vermicelli_device.h"
#include <glm/glm.hpp>
#include <vector>

namespace vermicelli {
class VermicelliModel {
  bool             mVerbose;
  VermicelliDevice &mDevice;
  VkBuffer         mVertexBuffer;
  VkDeviceMemory   mVertexBufferMemory;
  uint32_t         mVertexCount;

public:
  struct Vertex {
      glm::vec2 mPosition;
      glm::vec3 mColor;

      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
  };

  VermicelliModel(VermicelliDevice &device, const std::vector<Vertex> &vertices, bool verbose);

  ~VermicelliModel();

  VermicelliModel(const VermicelliModel &) = delete;

  VermicelliModel &operator=(const VermicelliModel &) = delete;

  void bind(VkCommandBuffer commandBuffer);

  void draw(VkCommandBuffer commandBuffer);

private:

  void createVertexBuffers(const std::vector<Vertex> &vertices);
};
}

#endif //__VERMICELLI_VERMICELLI_MODEL_H__
