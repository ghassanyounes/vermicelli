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
#include "vermicelli_game_object.h"
#include <vulkan/vulkan.h>

namespace vermicelli {

#define MAX_LIGHTS 20

struct PointLight {
    glm::vec4 position{}; // ignore w
    glm::vec4 color{}; // w is intensity
};

struct FrameInfo {
    int                       mFrameIndex;
    float                     mFrameTime;
    VkCommandBuffer           mCommandBuffer;
    VermicelliCamera          &mCamera;
    VkDescriptorSet           mGlobalDescriptorSet;
    VermicelliGameObject::Map &mGameObjects;
};

struct GlobalUbo {
    glm::mat4  mProjection{1.0f};
    glm::mat4  mView{1.0f};
    glm::mat4  mInverseView{1.0f};
    //alignas(16) glm::vec3 mLightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
    glm::vec4  mAmbientColor{color::white, 0.02f};
    PointLight pointLights[MAX_LIGHTS];
    int        numLights;
};

}

#endif //__VERMICELLI_VERMICELLI_FRAME_INFO_H__
