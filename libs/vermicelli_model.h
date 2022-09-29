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
#include "vermicelli_buffer.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace vermicelli {
class VermicelliModel {
  bool                              mVerbose;
  VermicelliDevice                  &mDevice;
  std::unique_ptr<VermicelliBuffer> mVertexBuffer;
  uint32_t                          mVertexCount;
  bool                              mHasIndexBuffer = false;
  std::unique_ptr<VermicelliBuffer> mIndexBuffer;
  uint32_t                          mIndexCount;

public:
  struct Vertex {
      glm::vec3 mPosition{};
      glm::vec3 mColor{};
      glm::vec3 mNormal{};
      glm::vec2 mUV{};

      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

      bool operator==(const Vertex &rhs) const {
        return mPosition == rhs.mPosition && mColor == rhs.mColor && mNormal == rhs.mNormal && mUV == rhs.mUV;
      }
  };

  struct Builder {
      std::vector<Vertex>   mVertices{};
      std::vector<uint32_t> mIndices{};

      void loadModel(const std::string &filePath);
  };

  VermicelliModel(VermicelliDevice &device, const VermicelliModel::Builder &builder, bool verbose);

  ~VermicelliModel();

  VermicelliModel(const VermicelliModel &) = delete;

  VermicelliModel &operator=(const VermicelliModel &) = delete;

  static std::unique_ptr<VermicelliModel>
  createModelFromFile(VermicelliDevice &device, const std::string &filePath, bool verbose = false);

  void bind(VkCommandBuffer commandBuffer);

  void draw(VkCommandBuffer commandBuffer) const;

private:

  void createVertexBuffers(const std::vector<Vertex> &vertices);

  void createIndexBuffers(const std::vector<uint32_t> &indices);
};
}

#endif //__VERMICELLI_VERMICELLI_MODEL_H__
