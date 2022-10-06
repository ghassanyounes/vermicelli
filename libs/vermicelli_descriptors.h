/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/29/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_DESCRIPTORS_H__
#define __VERMICELLI_VERMICELLI_DESCRIPTORS_H__
#pragma once

#include "vermicelli_device.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vermicelli {
class VermicelliDescriptorSetLayout {
public:
  class Builder {
  public:
    Builder(VermicelliDevice &mDevice) : mDevice{mDevice} {}

    Builder &addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);

    std::unique_ptr<VermicelliDescriptorSetLayout> build() const;

  private:
    VermicelliDevice                                           &mDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> mBuilderBindings{};
  };

  VermicelliDescriptorSetLayout(
          VermicelliDevice &mDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);

  ~VermicelliDescriptorSetLayout();

  VermicelliDescriptorSetLayout(const VermicelliDescriptorSetLayout &) = delete;

  VermicelliDescriptorSetLayout &operator=(const VermicelliDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return mDescriptorSetLayout; }

private:
  VermicelliDevice                                           &mDevice;
  VkDescriptorSetLayout                                      mDescriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> mBindings;

  friend class VermicelliDescriptorWriter;
};

class VermicelliDescriptorPool {
public:
  class Builder {
  public:
    Builder(VermicelliDevice &mDevice) : mDevice{mDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);

    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);

    Builder &setMaxSets(uint32_t count);

    std::unique_ptr<VermicelliDescriptorPool> build() const;

  private:
    VermicelliDevice                  &mDevice;
    std::vector<VkDescriptorPoolSize> mPoolSizes{};
    uint32_t                          mMaxSets   = 1000;
    VkDescriptorPoolCreateFlags       mPoolFlags = 0;
  };

  VermicelliDescriptorPool(
          VermicelliDevice &mDevice,
          uint32_t maxSets,
          VkDescriptorPoolCreateFlags poolFlags,
          const std::vector<VkDescriptorPoolSize> &poolSizes);

  ~VermicelliDescriptorPool();

  VermicelliDescriptorPool(const VermicelliDescriptorPool &) = delete;

  VermicelliDescriptorPool &operator=(const VermicelliDescriptorPool &) = delete;

  bool allocateDescriptor(
          const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

private:
  VermicelliDevice &mDevice;
  VkDescriptorPool mDescriptorPool;

  friend class VermicelliDescriptorWriter;
};

class VermicelliDescriptorWriter {
public:
  VermicelliDescriptorWriter(VermicelliDescriptorSetLayout &setLayout, VermicelliDescriptorPool &pool);

  VermicelliDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);

  VermicelliDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);

  void overwrite(VkDescriptorSet &set);

private:
  VermicelliDescriptorSetLayout     &mSetLayout;
  VermicelliDescriptorPool          &mPool;
  std::vector<VkWriteDescriptorSet> mWrites;
};
}

#endif //__VERMICELLI_VERMICELLI_DESCRIPTORS_H__
