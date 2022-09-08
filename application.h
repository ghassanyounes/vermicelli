/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#ifndef __VERMICELLI_APPLICATION_H__
#define __VERMICELLI_APPLICATION_H__
#pragma once

#include "vermicelli_window.h"
#include "vermicelli_pipeline.h"
#include "vermicelli_device.h"
#include "vermicelli_swap_chain.h"
#include "vermicelli_model.h"

#include <memory>
#include <vector>

namespace vermicelli {

class Application {
  VermicelliWindow                     mWindow{"Vermicelli", mDim};
  bool                                 mVerbose;
  VermicelliDevice                     mDevice{mWindow, mVerbose};
  std::unique_ptr<VermicelliSwapChain> mSwapChain;
  std::unique_ptr<VermicelliPipeline>  mPipeline;
  VkPipelineLayout                     mPipelineLayout;
  std::vector<VkCommandBuffer>         mCommandBuffers;
  std::unique_ptr<VermicelliModel>     mModel;

  void createPipelineLayout();

  void createPipeline();

  void createCommandBuffers();

  void freeCommandBuffers();

  void drawFrame();

  void loadModels();

  void recreateSwapChain();

  void recordCommandBuffer(int imageIndex);

public:
  explicit Application(bool verbose);

  ~Application();

  Application(const Application &) = delete;

  Application &operator=(const Application &) = delete;

  static constexpr glm::u32vec2 mDim{800, 600};

  void run();

};

}

#endif //__VERMICELLI_APPLICATION_H__