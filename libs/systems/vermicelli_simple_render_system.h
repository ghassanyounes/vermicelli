/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/12/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_SIMPLE_RENDER_SYSTEM_H__
#define __VERMICELLI_VERMICELLI_SIMPLE_RENDER_SYSTEM_H__
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

class VermicelliSimpleRenderSystem {
  bool                                mVerbose;
  VermicelliDevice                    &mDevice;
  std::unique_ptr<VermicelliPipeline> mPipeline;
  VkPipelineLayout                    mPipelineLayout;

  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

  void createPipeline(VkRenderPass renderPass);

public:
  explicit VermicelliSimpleRenderSystem(VermicelliDevice &device, VkRenderPass renderPass,
                                        VkDescriptorSetLayout globalSetLayout, bool verbose);

  ~VermicelliSimpleRenderSystem();

  VermicelliSimpleRenderSystem(const VermicelliSimpleRenderSystem &) = delete;

  VermicelliSimpleRenderSystem &operator=(const VermicelliSimpleRenderSystem &) = delete;

  void renderGameObjects(FrameInfo &frameInfo);

};

}


#endif //__VERMICELLI_VERMICELLI_SIMPLE_RENDER_SYSTEM_H__
