# vermicelli

---
![version](https://img.shields.io/badge/dynamic/json?logo=linux&color=green&label=Latest%20Build&prefix=v&query=version&suffix=alpha&url=https%3A%2F%2Fraw.githubusercontent.com%2Fghassanyounes%2Fvermicelli%2Fmaster%status.json)

### An engine built on Vulkan and SDL!

Working with Vulkan and SDL! - Current stage: following online tutorials until I get the hang of things

[Here's the tutorial I'm following](https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR)

Code includes some comments about the differences in SDL2 and glfw when it comes to following the tutorial as well.

<!-- Todo: Implement shields.io on here to show build status, version, etc-->

## Install guide:

---

### Linux Build instructions ![Linux Build Status]()

- Build dependencies
  [![]()](https://cmake.org/) [![]()](https://www.vulkan.org/) [![]()](https://libsdl.org/) [![]()](https://github.com/libSDL2pp/libSDL2pp)
  - For example,

```shell
sudo pacman -S clang
sudo pacman -S cmake
sudo pacman -S vulkan-devel
sudo pacman -S sdl2
sudo pacman -S sdl2_image
sudo pacman -S sdl2_mixer
sudo pacman -S sdl2_ttf
sudo pacman -S glm
sudo pacman -S shaderc
```

- CMake will pull the following repositories from GitHub:
  [![]()](https://github.com/g-truc/glm) [![]()](https://github.com/liliolett/cmake-spirv) [![]()](https://github.com/fmtlib/fmt)
  - If you are on Arch linux, you can install SDL2pp through the AUR, e.g.

```shell
trizen -S sdl2pp
```