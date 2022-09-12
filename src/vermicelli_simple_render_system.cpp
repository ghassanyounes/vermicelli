/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/12/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#include "vermicelli_simple_render_system.h"
#include "vermicelli_functions.h"
#include <glm/gtc/constants.hpp> // PI
#include <stdexcept>
#include <array>

namespace vermicelli {

struct SimplePushConstantData {
    glm::mat2             transform{1.f};
    glm::vec2             offset;
    alignas(16) glm::vec3 color;
};

VermicelliSimpleRenderSystem::VermicelliSimpleRenderSystem(VermicelliDevice &device, VkRenderPass renderPass,
                                                           const bool verbose) : mVerbose(verbose), mDevice(device) {
  createPipelineLayout();
  createPipeline(renderPass);
}

VermicelliSimpleRenderSystem::~VermicelliSimpleRenderSystem() {
  vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void VermicelliSimpleRenderSystem::createPipelineLayout() {

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset     = 0;
  pushConstantRange.size       = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = 0;
  pipelineLayoutInfo.pSetLayouts            = nullptr;
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

void VermicelliSimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer,
                                                     std::vector<VermicelliGameObject> &gameObjects) {
  mPipeline->bind(commandBuffer);
  for (auto &obj: gameObjects) {
    obj.mTransform2d.mRotation = glm::mod(obj.mTransform2d.mRotation + 0.01f, glm::two_pi<float>());

    SimplePushConstantData push{};
    push.offset    = obj.mTransform2d.mTranslation;
    push.color     = obj.mColor;
    push.transform = obj.mTransform2d.mat2();

    vkCmdPushConstants(commandBuffer, mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData),
                       &push);
    obj.mModel->bind(commandBuffer);
    obj.mModel->draw(commandBuffer);
  }
}
}