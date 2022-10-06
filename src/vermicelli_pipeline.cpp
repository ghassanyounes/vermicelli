/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include "vermicelli_pipeline.h"
#include "vermicelli_model.h"

namespace vermicelli {

std::vector<char> VermicelliPipeline::readFile(const std::string &filePath) {
  std::ifstream file{filePath, std::ios::ate | std::ios::binary};
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filePath);
  }

  size_t fileSize = static_cast<size_t>(file.tellg());

  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();
  return buffer;
}

void VermicelliPipeline::createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath,
                                                const vermicelli::PipelineConfigInfo &configInfo) {
  assert(configInfo.mPipelineLayout != VK_NULL_HANDLE &&
         "Cannot create graphics pipeline, no mPipelineLayout provided in configInfo!");
  assert(configInfo.mRenderPass != VK_NULL_HANDLE &&
         "Cannot create graphics pipeline, no mRenderPass provided in configInfo!");
  auto vertCode = readFile(vertFilePath);
  auto fragCode = readFile(fragFilePath);

  createShaderModule(vertCode, &mVertShaderModule);
  createShaderModule(fragCode, &mFragShaderModule);

  VkPipelineShaderStageCreateInfo shaderStages[2];
  shaderStages[0].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage               = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module              = mVertShaderModule;
  shaderStages[0].pName               = "main";
  shaderStages[0].flags               = 0;
  shaderStages[0].pNext               = nullptr;
  shaderStages[0].pSpecializationInfo = nullptr;

  shaderStages[1].sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module              = mFragShaderModule;
  shaderStages[1].pName               = "main";
  shaderStages[1].flags               = 0;
  shaderStages[1].pNext               = nullptr;
  shaderStages[1].pSpecializationInfo = nullptr;

  auto                                 &attributeDescriptions = configInfo.mAttributeDescriptions;
  auto                                 &bindingDescriptions   = configInfo.mBindingDescriptions;
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(bindingDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
  vertexInputInfo.pVertexBindingDescriptions      = bindingDescriptions.data();

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount          = 2;
  pipelineInfo.pStages             = shaderStages;
  pipelineInfo.pVertexInputState   = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &configInfo.mInputAssemblyInfo;
  pipelineInfo.pViewportState      = &configInfo.mViewportInfo;
  pipelineInfo.pRasterizationState = &configInfo.mRasterizationInfo;
  pipelineInfo.pMultisampleState   = &configInfo.mMultisampleInfo;
  pipelineInfo.pColorBlendState    = &configInfo.mColorBlendInfo;
  pipelineInfo.pDepthStencilState  = &configInfo.mDepthStencilInfo;
  pipelineInfo.pDynamicState       = &configInfo.mDynamicStateInfo;

  pipelineInfo.layout     = configInfo.mPipelineLayout;
  pipelineInfo.renderPass = configInfo.mRenderPass;
  pipelineInfo.subpass    = configInfo.mSubpass;

  pipelineInfo.basePipelineIndex  = -1;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(mDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create graphics pipeline!");
  }
}

VermicelliPipeline::VermicelliPipeline(vermicelli::VermicelliDevice &device, const std::string &vertFilePath,
                                       const std::string &fragFilePath,
                                       const vermicelli::PipelineConfigInfo &configInfo) : mDevice(device) {
  createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
}

VermicelliPipeline::~VermicelliPipeline() {
  vkDestroyShaderModule(mDevice.device(), mVertShaderModule, nullptr);
  vkDestroyShaderModule(mDevice.device(), mFragShaderModule, nullptr);
  vkDestroyPipeline(mDevice.device(), mGraphicsPipeline, nullptr);
}

void VermicelliPipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode    = reinterpret_cast<const uint32_t *>(code.data());

  if (vkCreateShaderModule(mDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create shader module");
  }
}

void VermicelliPipeline::bind(VkCommandBuffer commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
}

void VermicelliPipeline::defaultPipelineConfigInfo(PipelineConfigInfo &configInfo) {
  configInfo.mInputAssemblyInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  configInfo.mInputAssemblyInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  configInfo.mInputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  configInfo.mViewportInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  configInfo.mViewportInfo.viewportCount = 1;
  configInfo.mViewportInfo.pViewports    = nullptr;
  configInfo.mViewportInfo.scissorCount  = 1;
  configInfo.mViewportInfo.pScissors     = nullptr;

  configInfo.mRasterizationInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  configInfo.mRasterizationInfo.depthClampEnable        = VK_FALSE;
  configInfo.mRasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
  configInfo.mRasterizationInfo.polygonMode             = VK_POLYGON_MODE_FILL;
  configInfo.mRasterizationInfo.lineWidth               = 1.0f;
  // Back face culling can be used to greatly improve performance!
  configInfo.mRasterizationInfo.cullMode                = VK_CULL_MODE_NONE;
  configInfo.mRasterizationInfo.frontFace               = VK_FRONT_FACE_CLOCKWISE;
  configInfo.mRasterizationInfo.depthBiasEnable         = VK_FALSE;
  configInfo.mRasterizationInfo.depthBiasConstantFactor = 0.0f;
  configInfo.mRasterizationInfo.depthBiasClamp          = 0.0f;
  configInfo.mRasterizationInfo.depthBiasSlopeFactor    = 0.0f;

  configInfo.mMultisampleInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  configInfo.mMultisampleInfo.sampleShadingEnable   = VK_FALSE;
  configInfo.mMultisampleInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
  configInfo.mMultisampleInfo.minSampleShading      = 1.0f;           // Optional
  configInfo.mMultisampleInfo.pSampleMask           = nullptr;             // Optional
  configInfo.mMultisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
  configInfo.mMultisampleInfo.alphaToOneEnable      = VK_FALSE;       // Optional

  configInfo.mColorBlendAttachment.colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
          VK_COLOR_COMPONENT_A_BIT;
  configInfo.mColorBlendAttachment.blendEnable         = VK_FALSE;
  configInfo.mColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
  configInfo.mColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
  configInfo.mColorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;              // Optional
  configInfo.mColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
  configInfo.mColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
  configInfo.mColorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;              // Optional

  configInfo.mColorBlendInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  configInfo.mColorBlendInfo.logicOpEnable   = VK_FALSE;
  configInfo.mColorBlendInfo.logicOp         = VK_LOGIC_OP_COPY;  // Optional
  configInfo.mColorBlendInfo.attachmentCount = 1;
  configInfo.mColorBlendInfo.pAttachments    = &configInfo.mColorBlendAttachment;
  configInfo.mColorBlendInfo.blendConstants[0] = 0.0f;  // Optional
  configInfo.mColorBlendInfo.blendConstants[1] = 0.0f;  // Optional
  configInfo.mColorBlendInfo.blendConstants[2] = 0.0f;  // Optional
  configInfo.mColorBlendInfo.blendConstants[3] = 0.0f;  // Optional

  configInfo.mDepthStencilInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  configInfo.mDepthStencilInfo.depthTestEnable       = VK_TRUE;
  configInfo.mDepthStencilInfo.depthWriteEnable      = VK_TRUE;
  configInfo.mDepthStencilInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
  configInfo.mDepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  configInfo.mDepthStencilInfo.minDepthBounds        = 0.0f;
  configInfo.mDepthStencilInfo.maxDepthBounds        = 1.0f;
  configInfo.mDepthStencilInfo.stencilTestEnable     = VK_FALSE;
  configInfo.mDepthStencilInfo.front                 = {};
  configInfo.mDepthStencilInfo.back                  = {};

  configInfo.mDynamicStateEnables                = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  configInfo.mDynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  configInfo.mDynamicStateInfo.pDynamicStates    = configInfo.mDynamicStateEnables.data();
  configInfo.mDynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.mDynamicStateEnables.size());
  configInfo.mDynamicStateInfo.flags             = 0;

  configInfo.mAttributeDescriptions = VermicelliModel::Vertex::getAttributeDescriptions();
  configInfo.mBindingDescriptions   = VermicelliModel::Vertex::getBindingDescriptions();
}

}