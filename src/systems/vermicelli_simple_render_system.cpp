/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/12/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#include "systems/vermicelli_simple_render_system.h"
#include "vermicelli_functions.h"
#include <glm/gtc/constants.hpp> // PI
#include <stdexcept>
#include <array>

namespace vermicelli {

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
};

VermicelliSimpleRenderSystem::VermicelliSimpleRenderSystem(VermicelliDevice &device, VkRenderPass renderPass,
                                                           VkDescriptorSetLayout globalSetLayout,
                                                           const bool verbose) : mVerbose(verbose), mDevice(device) {
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

VermicelliSimpleRenderSystem::~VermicelliSimpleRenderSystem() {
  vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void VermicelliSimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset     = 0;
  pushConstantRange.size       = sizeof(SimplePushConstantData);

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts            = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;

  if (vkCreatePipelineLayout(mDevice.device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void VermicelliSimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(mPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

  PipelineConfigInfo pipelineConfig{};
  VermicelliPipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.mRenderPass     = renderPass;
  pipelineConfig.mPipelineLayout = mPipelineLayout;
  mPipeline = std::make_unique<VermicelliPipeline>(mDevice, "shaders/simple_shader.vert.spv",
                                                   "shaders/simple_shader.frag.spv", pipelineConfig);
}

void VermicelliSimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo) {
  mPipeline->bind(frameInfo.mCommandBuffer);

  vkCmdBindDescriptorSets(frameInfo.mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                          &frameInfo.mGlobalDescriptorSet, 0,
                          nullptr);

  for (auto &kv: frameInfo.mGameObjects) {
    auto &obj = kv.second;
    if (obj.mModel == nullptr) continue;
    SimplePushConstantData push{};
    push.modelMatrix  = obj.mTransform.mat4();
    push.normalMatrix = obj.mTransform.normalMatrix();

    vkCmdPushConstants(frameInfo.mCommandBuffer, mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData),
                       &push);
    obj.mModel->bind(frameInfo.mCommandBuffer);
    obj.mModel->draw(frameInfo.mCommandBuffer);
  }
}
}