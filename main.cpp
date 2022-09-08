/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    2022-09-07
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#define APP_NAME "Vermicelli"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <getopt.h>

#include "application.h"

using std::cout, std::cerr, std::endl;

static int           verbose_flag   = 0;
static struct option long_options[] = {
        /* These options set a flag. */
        {"verbose", no_argument, &verbose_flag, 1},
        {"brief",   no_argument, &verbose_flag, 0},
        /* These options don’t set a flag.
        We distinguish them by their indices. */
        {"help",    no_argument, 0,             'h'},
        //{"append",  no_argument,       0, 'b'},
        {0, 0,                   0,             0}
};

/**
 * @brief The main function of the engine, runs the application and catches any exceptions.
 * @param argc
 *  Number of command line arguments
 * @param argv
 *  Array of arguments
 * @return
 *  On success, EXIT_SUCCESS (0)
 *  On failure, EXIT_FAILURE (1)
 *  as defined by <cstdlib>
 */
int main(int argc, char *argv[]) {
  int c;
  while (true) {
    int option_index = 0;
    c = getopt_long(argc, argv, ":h", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
      case 0:
        if (long_options[option_index].flag) {
          break;
        }
        cout << "Option " << long_options[option_index].name << (optarg ? "with option " : "") << (optarg ? optarg : "")
             << endl;
      case 'h':
        cout << "Vermicelli, a Vulkan Renderer and Engine built on SDL2" << endl;
        return EXIT_SUCCESS;
      case '?':
        cout << "Option -" << static_cast<char>(optopt) << " is unknown." << endl
             << "Please see the help menu (-h) or the Vermicelli man pages for help" << endl;
        return EXIT_FAILURE;
      default:
        break;
    }
  }

  SDL2pp::SDL sdl(SDL_INIT_VIDEO);

  vermicelli::Application app{static_cast<bool>(verbose_flag)};

  try {
    app.run();
  } catch (const std::exception &exception) {
    cerr << exception.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
