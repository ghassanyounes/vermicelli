/*!********************************************************************************************************************
 * @file    main.cpp
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-05-05
 * @brief
 *********************************************************************************************************************/

#define APP_NAME "Vermicelli"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "application.h"

using std::cout, std::cerr, std::endl;

int main(int argc, char *argv[]) {
  SDL2pp::SDL sdl(SDL_INIT_VIDEO);

  vermicelli::Application app{};

  try {
    app.run();
  } catch (const std::exception &exception) {
    cerr << exception.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
