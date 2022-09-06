//
// Created by bigtiddypiratebf on 9/6/22.
//

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <optional>
#include "application.h"

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

void HelloTriangleApplication::run() {
  initWindow();
  std::cout << "reached init vulkan" << std::endl;
  initVulkan();
  std::cout << "reached main loop" << std::endl;
  mainLoop();
  std::cout << "Reached cleanup" << std::endl;
  cleanup();
}

void HelloTriangleApplication::initWindow() {
  SDL_Vulkan_LoadLibrary(nullptr);
}

void HelloTriangleApplication::mainLoop() {
  bool      running = true;
  SDL_Event windowEvent;
  while (SDL_PollEvent(&windowEvent)) {
    if (windowEvent.type == SDL_QUIT) {
      running = false;
      break;
    }
  }
}

void HelloTriangleApplication::cleanup() {
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);
  SDL_Vulkan_UnloadLibrary();
  SDL_Quit();

  SDL_Log("Cleaned up with errors: %s", SDL_GetError());
}

void HelloTriangleApplication::initVulkan() {
  createInstance();
  std::cout << "finished createInstance()" << std::endl;
  setupDebugMessenger();
  std::cout << "finished setupDebugMessenger()" << std::endl;
  pickPhysicalDevice();
  std::cout << "finished pickPhysicalDevice()" << std::endl;
  createLogicalDevice();
  std::cout << "finished createLogicalDevice()" << std::endl;
}

void HelloTriangleApplication::createInstance() {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName   = APP_NAME;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName        = "No Engine";
  appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion         = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (enableValidationLayers) {
    createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

void HelloTriangleApplication::createLogicalDevice() {

  std::cout << "Searching for queue families" << std::endl;
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  SDL_Log("Initialized with errors: %s", SDL_GetError());

  std::cout << "found queue families" << std::endl;
  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount       = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.pQueueCreateInfos    = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = 0;

  std::cout << "checking if validation layers are enabled" << std::endl;
  if (enableValidationLayers) {
    createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
  std::cout << "attempting to create logical device" << std::endl;
  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  } else {
    std::cout << "Logical device created successfully" << std::endl;
  }

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity =
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType     =
          VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}

void HelloTriangleApplication::setupDebugMessenger() {
  if (!enableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

std::vector<const char *> HelloTriangleApplication::getRequiredExtensions() {
  uint32_t   sdlExtensionCount = 0;
  const char **sdlExtensions;
  SDL_Vulkan_GetInstanceExtensions(window.Get(), &sdlExtensionCount, nullptr);
  sdlExtensions = new const char *[sdlExtensionCount];
  SDL_Vulkan_GetInstanceExtensions(window.Get(), &sdlExtensionCount, sdlExtensions);
  std::vector<const char *> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void HelloTriangleApplication::pickPhysicalDevice() {
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  uint32_t         deviceCount    = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  } else {
    std::cout << "succeed in finding suitable gpu with vulkan support" << std::endl;
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  std::cout << "scanning devices for suitability" << std::endl;
  for (const auto &device: devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      std::cout << "suitable device found " << std::endl;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  } else {
    std::cout << "succeed in finding suitable gpu " << std::endl;
  }
}

bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device);

  return indices.isComplete();
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  SDL_Vulkan_CreateSurface(window.Get(), instance, &surface);

  indices.graphicsFamily = UINT32_MAX;
  indices.presentFamily  = UINT32_MAX;
  VkBool32                     support;
  uint32_t                     i = 0;
  for (VkQueueFamilyProperties queueFamily: queueFamilies) {
    if (indices.graphicsFamily == UINT32_MAX && queueFamily.queueCount > 0 &&
        queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    if (indices.presentFamily == UINT32_MAX) {
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &support);
      if (support) {
        indices.presentFamily = i;
      }
    }
    ++i;
  }

  return indices;
}

bool HelloTriangleApplication::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName: validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties: availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}
