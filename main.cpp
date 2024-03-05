#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <cstdlib>

// Create class for the application
class VulkanTutorialApplication {
// Define the public member funcitons
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

// Define the private member functions and variables
private:
  // Define consts for width and height of the window
  const uint32_t _WIDTH    = 800;
  const uint32_t _HEIGHT   = 600;

  // Define pointer to glfw window object
  GLFWwindow* _window;

  // Define Vulkan instance variable
  VkInstance _instance;

  // Initialze initWindow()
  void initWindow() {
    // Initialze GLFW
    glfwInit();

    // Tell GLFW to not use OpenGL context and to not allow resizing of window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Initialze the window variable
    _window = glfwCreateWindow(_WIDTH, _HEIGHT, "Vulkan", nullptr, nullptr);

  }

  // Initialze initVulkan()
  void initVulkan() {
    createInstance();
  }

  // Initialze createInstance()
  void createInstance() {
    // Define application info struct and initialize with info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Define and initialize struct for Vulkan driver extension and validation layer info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Specify GLFW interface extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // Specify validation layers to enable
    createInfo.enabledLayerCount = 0;

    // Create a Vulkan Instance + check for successful creation
    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("ERROR: Failed to create Vulkan instance!");
    }
  }

  // Initialze mainLoop()
  void mainLoop() {
    // Keep app running until error or window is closed
    while (!glfwWindowShouldClose(_window)) {
      // Event loop
      glfwPollEvents();
    }
  }

  // Initialze cleanup()
  void cleanup() {
    // Destry Vulkan instance
    vkDestroyInstance(_instance, nullptr);

    // Remove _window from memory
    glfwDestroyWindow(_window);

    // Terminate GLFW instance
    glfwTerminate();
  }
};

int main () {
  // Create applcation class instance
  VulkanTutorialApplication app;

  // Catch any exceptions from application and print to screen
  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  // app finished successfully
  return EXIT_SUCCESS;
} 
