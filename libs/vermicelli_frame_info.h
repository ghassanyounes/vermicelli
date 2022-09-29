/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/20/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_FRAME_INFO_H__
#define __VERMICELLI_VERMICELLI_FRAME_INFO_H__
#pragma once

#include "vermicelli_camera.h"
#include <vulkan/vulkan.h>

namespace vermicelli {

struct FrameInfo {
    int             mFrameIndex;
    float           mFrameTime;
    VkCommandBuffer mCommandBuffer;
    VermicelliCamera &mCamera;
};

}

#endif //__VERMICELLI_VERMICELLI_FRAME_INFO_H__
