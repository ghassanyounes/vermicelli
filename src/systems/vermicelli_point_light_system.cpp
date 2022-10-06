/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/29/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#include "systems/vermicelli_point_light_system.h"

#include "systems/vermicelli_simple_render_system.h"
#include "vermicelli_functions.h"
#include <glm/gtc/constants.hpp> // PI
#include <stdexcept>
#include <array>

namespace vermicelli {

struct PointLightPushConstants {
    glm::vec4 position{};
    glm::vec4 color{};
    float     radius;
};

VermicelliPointLightSystem::VermicelliPointLightSystem(VermicelliDevice &device, VkRenderPass renderPass,
                                                       VkDescriptorSetLayout globalSetLayout,
                                                       const bool verbose) : mVerbose(verbose), mDevice(device) {
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

VermicelliPointLightSystem::~VermicelliPointLightSystem() {
  vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}

void VermicelliPointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset     = 0;
  pushConstantRange.size       = sizeof(PointLightPushConstants);

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

void VermicelliPointLightSystem::createPipeline(VkRenderPass renderPass) {
  assert(mPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

  PipelineConfigInfo pipelineConfig{};
  VermicelliPipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.mAttributeDescriptions.clear();
  pipelineConfig.mBindingDescriptions.clear();
  pipelineConfig.mRenderPass     = renderPass;
  pipelineConfig.mPipelineLayout = mPipelineLayout;
  mPipeline = std::make_unique<VermicelliPipeline>(mDevice, "shaders/point_light.vert.spv",
                                                   "shaders/point_light.frag.spv", pipelineConfig);
}

void VermicelliPointLightSystem::render(FrameInfo &frameInfo) {
  mPipeline->bind(frameInfo.mCommandBuffer);

  vkCmdBindDescriptorSets(frameInfo.mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                          &frameInfo.mGlobalDescriptorSet, 0,
                          nullptr);

  for (auto &kv: frameInfo.mGameObjects) {
    auto &obj = kv.second;
    if (obj.mPointLight == nullptr) continue;

    PointLightPushConstants push{};
    push.position = glm::vec4(obj.mTransform.mTranslation, 1.0f);
    push.color    = glm::vec4(obj.mColor, obj.mPointLight->mLightIntensity);
    push.radius   = obj.mTransform.mScale.x;

    vkCmdPushConstants(
            frameInfo.mCommandBuffer,
            mPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PointLightPushConstants),
            &push
                      );
    vkCmdDraw(frameInfo.mCommandBuffer, 6, 1, 0, 0);
  }


}

void VermicelliPointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {
  auto      rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.mFrameTime, {0.0f, -1.0f, 0.0f});
  int       lightIndex  = 0;
  for (auto &kv: frameInfo.mGameObjects) {
    auto &obj = kv.second;
    if (obj.mPointLight == nullptr) continue;

    assert(lightIndex < MAX_LIGHTS && "Maximum number of point lights exceeded");

    obj.mTransform.mTranslation = glm::vec3(rotateLight * glm::vec4(obj.mTransform.mTranslation, 1.0f));

    ubo.pointLights[lightIndex].position = glm::vec4(obj.mTransform.mTranslation, 1.0f);
    ubo.pointLights[lightIndex].color    = glm::vec4(obj.mColor, obj.mPointLight->mLightIntensity);
    ++lightIndex;
  }
  ubo.numLights         = lightIndex;
}
}