//
// Created by bigtiddypiratebf on 9/6/22.
//

#ifndef VERMICELLI_APPLICATION_H
#define VERMICELLI_APPLICATION_H

#endif //VERMICELLI_APPLICATION_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>


#define APP_NAME "Hello, Triangle"

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
      return graphicsFamily.has_value() || presentFamily.has_value();
    }
};

class HelloTriangleApplication {
public:
  void run();

private:

  ///<< Fixme: implement resizable windows
  SDL2pp::Window           window         = SDL2pp::Window(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                           WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
  VkInstance               instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE;
  VkDevice                 device;
  VkQueue                  graphicsQueue;
  VkQueue                  presentQueue;
  VkSurfaceKHR             surface;

  void initWindow();

  void mainLoop();

  void cleanup();

  void createInstance();

  void initVulkan();

  void createLogicalDevice();

  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  void setupDebugMessenger();

  std::vector<const char *> getRequiredExtensions();

  void pickPhysicalDevice();

  bool isDeviceSuitable(VkPhysicalDevice device);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  bool checkValidationLayerSupport();
};

