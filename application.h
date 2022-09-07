/*!********************************************************************************************************************
 * @file    application.h
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-05-05
 * @brief
 *********************************************************************************************************************/

#ifndef __VERMICELLI_APPLICATION_H__
#define __VERMICELLI_APPLICATION_H__
#pragma once

#include "vermicelli_window.h"
#include "vermicelli_pipeline.h"
#include "vermicelli_device.h"
#include "vermicelli_swap_chain.h"

#include <memory>
#include <vector>

namespace vermicelli {

class Application {
  VermicelliWindow                    mWindow{"Vermicelli", mDim};
  VermicelliDevice                    mDevice{mWindow};
  VermicelliSwapChain                 mSwapChain{mDevice, mWindow.getExtent()};
  /*VermicelliPipeline  mPipeline{mDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv",
                                VermicelliPipeline::defaultPipelineConfigInfo(mDim)};*/
  std::unique_ptr<VermicelliPipeline> mPipeline;
  VkPipelineLayout                    mPipelineLayout;
  std::vector<VkCommandBuffer>        mCommandBuffers;

  void createPipelineLayout();

  void createPipeline();

  void createCommandBuffers();

  void drawFrame();

public:
  Application();

  ~Application();

  Application(const Application &) = delete;

  Application &operator=(const Application &) = delete;

  static constexpr glm::u32vec2 mDim{800, 600};

  void run();

};

}

#endif //__VERMICELLI_APPLICATION_H__