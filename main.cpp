#include <iostream>
#include <stdexcept>
#include <SDL2pp/SDL2pp.hh>
#include "application.h"

int main() {
  SDL2pp::SDL sdl(SDL_INIT_VIDEO);
  std::cout << "Hello, World!" << std::endl;
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
