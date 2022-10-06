/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/13/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/


#ifndef __VERMICELLI_VERMICELLI_KEYBOARD_INPUT_H__
#define __VERMICELLI_VERMICELLI_KEYBOARD_INPUT_H__
#pragma once

#include "vermicelli_game_object.h"
#include "vermicelli_window.h"

namespace vermicelli {
class VermicelliKeyboardInput {

public:
  enum KeyMappings {
      MoveLeft  = SDL_SCANCODE_A,
      MoveRight = SDL_SCANCODE_D,
      MoveFwd   = SDL_SCANCODE_W,
      MoveBack  = SDL_SCANCODE_S,
      MoveUp    = SDL_SCANCODE_SPACE,
      MoveDown  = SDL_SCANCODE_LSHIFT,
      LookLeft  = SDL_SCANCODE_LEFT,
      LookRight = SDL_SCANCODE_RIGHT,
      LookUp    = SDL_SCANCODE_UP,
      LookDown  = SDL_SCANCODE_DOWN,
      DebugKey  = SDL_SCANCODE_E
  };

  void moveInPlaneXZ(float dt, VermicelliGameObject &gameObject);

  float mMoveSpeed{3.0f};
  float mLookSpeed{1.5f};
};

}

#endif //__VERMICELLI_VERMICELLI_KEYBOARD_INPUT_H__
