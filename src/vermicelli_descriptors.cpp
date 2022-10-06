/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/29/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#include "vermicelli_descriptors.h"

namespace vermicelli {
// *************** Descriptor Set Layout Builder *********************

VermicelliDescriptorSetLayout::Builder &VermicelliDescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
  assert(mBuilderBindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding         = binding;
  layoutBinding.descriptorType  = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags      = stageFlags;
  mBuilderBindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<VermicelliDescriptorSetLayout> VermicelliDescriptorSetLayout::Builder::build() const {
  return std::make_unique<VermicelliDescriptorSetLayout>(mDevice, mBuilderBindings);
}

// *************** Descriptor Set Layout *********************

VermicelliDescriptorSetLayout::VermicelliDescriptorSetLayout(
        VermicelliDevice &mDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> mBindings)
        : mDevice{mDevice}, mBindings{mBindings} {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  for (auto                                 kv: mBindings) {
    setLayoutBindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings    = setLayoutBindings.data();

  if (vkCreateDescriptorSetLayout(
          mDevice.device(),
          &descriptorSetLayoutInfo,
          nullptr,
          &mDescriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

VermicelliDescriptorSetLayout::~VermicelliDescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(mDevice.device(), mDescriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

VermicelliDescriptorPool::Builder &VermicelliDescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
  mPoolSizes.push_back({descriptorType, count});
  return *this;
}

VermicelliDescriptorPool::Builder &VermicelliDescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
  mPoolFlags = flags;
  return *this;
}

VermicelliDescriptorPool::Builder &VermicelliDescriptorPool::Builder::setMaxSets(uint32_t count) {
  mMaxSets = count;
  return *this;
}

std::unique_ptr<VermicelliDescriptorPool> VermicelliDescriptorPool::Builder::build() const {
  return std::make_unique<VermicelliDescriptorPool>(mDevice, mMaxSets, mPoolFlags, mPoolSizes);
}

// *************** Descriptor Pool *********************

VermicelliDescriptorPool::VermicelliDescriptorPool(
        VermicelliDevice &mDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes)
        : mDevice{mDevice} {
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes    = poolSizes.data();
  descriptorPoolInfo.maxSets       = maxSets;
  descriptorPoolInfo.flags         = poolFlags;

  if (vkCreateDescriptorPool(mDevice.device(), &descriptorPoolInfo, nullptr, &mDescriptorPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

VermicelliDescriptorPool::~VermicelliDescriptorPool() {
  vkDestroyDescriptorPool(mDevice.device(), mDescriptorPool, nullptr);
}

bool VermicelliDescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool     = mDescriptorPool;
  allocInfo.pSetLayouts        = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
  // a new pool whenever an old pool fills up. But this is beyond our current scope
  if (vkAllocateDescriptorSets(mDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void VermicelliDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
  vkFreeDescriptorSets(
          mDevice.device(),
          mDescriptorPool,
          static_cast<uint32_t>(descriptors.size()),
          descriptors.data());
}

void VermicelliDescriptorPool::resetPool() {
  vkResetDescriptorPool(mDevice.device(), mDescriptorPool, 0);
}

// *************** Descriptor Writer *********************

VermicelliDescriptorWriter::VermicelliDescriptorWriter(VermicelliDescriptorSetLayout &setLayout,
                                                       VermicelliDescriptorPool &pool)
        : mSetLayout{setLayout}, mPool{pool} {}

VermicelliDescriptorWriter &VermicelliDescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
  assert(mSetLayout.mBindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &bindingDescription = mSetLayout.mBindings[binding];

  assert(
          bindingDescription.descriptorCount == 1 &&
          "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType  = bindingDescription.descriptorType;
  write.dstBinding      = binding;
  write.pBufferInfo     = bufferInfo;
  write.descriptorCount = 1;

  mWrites.push_back(write);
  return *this;
}

VermicelliDescriptorWriter &VermicelliDescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo *imageInfo) {
  assert(mSetLayout.mBindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &bindingDescription = mSetLayout.mBindings[binding];

  assert(
          bindingDescription.descriptorCount == 1 &&
          "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType  = bindingDescription.descriptorType;
  write.dstBinding      = binding;
  write.pImageInfo      = imageInfo;
  write.descriptorCount = 1;

  mWrites.push_back(write);
  return *this;
}

bool VermicelliDescriptorWriter::build(VkDescriptorSet &set) {
  bool success = mPool.allocateDescriptor(mSetLayout.getDescriptorSetLayout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void VermicelliDescriptorWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write: mWrites) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(mPool.mDevice.device(), mWrites.size(), mWrites.data(), 0, nullptr);
}

}