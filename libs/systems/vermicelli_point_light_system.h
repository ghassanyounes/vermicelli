/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/29/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_POINT_LIGHT_SYSTEM_H__
#define __VERMICELLI_VERMICELLI_POINT_LIGHT_SYSTEM_H__
#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include "vermicelli_pipeline.h"
#include "vermicelli_device.h"
#include "vermicelli_game_object.h"
#include "vermicelli_camera.h"
#include "vermicelli_frame_info.h"
#include <memory>
#include <vector>

namespace vermicelli {

class VermicelliPointLightSystem {
  bool                                mVerbose;
  VermicelliDevice                    &mDevice;
  std::unique_ptr<VermicelliPipeline> mPipeline;
  VkPipelineLayout                    mPipelineLayout;

  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

  void createPipeline(VkRenderPass renderPass);

public:
  explicit VermicelliPointLightSystem(VermicelliDevice &device, VkRenderPass renderPass,
                                      VkDescriptorSetLayout globalSetLayout, bool verbose);

  ~VermicelliPointLightSystem();

  VermicelliPointLightSystem(const VermicelliPointLightSystem &) = delete;

  VermicelliPointLightSystem &operator=(const VermicelliPointLightSystem &) = delete;

  void update(FrameInfo &frameInfo, GlobalUbo &ubo);

  void render(FrameInfo &frameInfo);

};

}

#endif //__VERMICELLI_VERMICELLI_POINT_LIGHT_SYSTEM_H__
