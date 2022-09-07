/*!********************************************************************************************************************
 * @file    vermicelli_pipeline.cpp
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-05-05
 * @brief
 *********************************************************************************************************************/

#ifndef __VERMICELLI_VERMICELLI_PIPELINE_H__
#define __VERMICELLI_VERMICELLI_PIPELINE_H__
#pragma once

#include <string>
#include <vector>
#include "vermicelli_device.h"

namespace vermicelli {

struct PipelineConfigInfo {
    PipelineConfigInfo() = default;

    PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

    PipelineConfigInfo(const PipelineConfigInfo &) = delete;

    VkViewport                             viewport;
    VkRect2D                               scissor;
    VkPipelineViewportStateCreateInfo      viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo   multisampleInfo;
    VkPipelineColorBlendAttachmentState    colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
    VkPipelineLayout                       pipelineLayout = nullptr;
    VkRenderPass                           renderPass     = nullptr;
    uint32_t                               subpass        = 0;
};

class VermicelliPipeline {
  VermicelliDevice &mDevice;
  VkPipeline     mGraphicsPipeline;
  VkShaderModule mVertShaderModule;
  VkShaderModule mFragShaderModule;

  static std::vector<char> readFile(const std::string &filePath);

  void createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath,
                              const PipelineConfigInfo &configInfo);

  void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

public:
  VermicelliPipeline(VermicelliDevice &device, const std::string &vertFilePath, const std::string &fragFilePath,
                     const PipelineConfigInfo &configInfo);

  ~VermicelliPipeline();

  VermicelliPipeline(const VermicelliPipeline &) = delete;

  void operator=(const VermicelliPipeline &) = delete;

  void bind(VkCommandBuffer commandBuffer);

  static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo, glm::u32vec2 dim);
};
}

#endif //__VERMICELLI_VERMICELLI_PIPELINE_H__
