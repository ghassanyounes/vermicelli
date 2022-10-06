/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
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

    std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions{};
    std::vector<VkVertexInputBindingDescription>   mBindingDescriptions{};
    VkPipelineViewportStateCreateInfo              mViewportInfo;
    VkPipelineInputAssemblyStateCreateInfo         mInputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo         mRasterizationInfo;
    VkPipelineMultisampleStateCreateInfo           mMultisampleInfo;
    VkPipelineColorBlendAttachmentState            mColorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo            mColorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo          mDepthStencilInfo;
    std::vector<VkDynamicState>                    mDynamicStateEnables;
    VkPipelineDynamicStateCreateInfo               mDynamicStateInfo;
    VkPipelineLayout                               mPipelineLayout = nullptr;
    VkRenderPass                                   mRenderPass     = nullptr;
    uint32_t                                       mSubpass        = 0;
};

class VermicelliPipeline {
  VermicelliDevice &mDevice;
  VkPipeline       mGraphicsPipeline;
  VkShaderModule   mVertShaderModule;
  VkShaderModule   mFragShaderModule;

  static std::vector<char> readFile(const std::string &filePath);

  void createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath,
                              const PipelineConfigInfo &configInfo);

  void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

public:
  VermicelliPipeline(VermicelliDevice &device, const std::string &vertFilePath, const std::string &fragFilePath,
                     const PipelineConfigInfo &configInfo);

  ~VermicelliPipeline();

  VermicelliPipeline(const VermicelliPipeline &) = delete;

  VermicelliPipeline operator=(const VermicelliPipeline &) = delete;

  void bind(VkCommandBuffer commandBuffer);

  static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
};
}

#endif //__VERMICELLI_VERMICELLI_PIPELINE_H__
