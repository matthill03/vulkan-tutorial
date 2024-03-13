#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_platform.h>

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>

// Set validation that will be used if NDEBUG is not set
const std::vector<const char*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"

};

// Use pre-processor if to set validation layers on or not
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif // NDEBUG

VkResult CreateDebugUtilsMessengerEXT(
  VkInstance instance, 
  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

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

  // Define the custom debug and message handler
  VkDebugUtilsMessengerEXT _debugMessenger;

  // Define the physical device handle
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

  // Define struct for all required queue families
  struct QueueFamilyIndicies {
    std::optional<uint32_t> graphicsFamily;

    // Function for checking if all needed families are found
    bool isComplete() {
      return graphicsFamily.has_value();
    }
  };

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
    setupDebugMessenger();
    pickPhysicalDevice();
  }

  // Initialze createInstance()
  void createInstance() {
    // Check if request validation layers are available
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("ERROR: Validation layers requested, but not available!");
    }

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
    auto extensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Define create info for debug and message callback
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};

    // enable validation layers specified at the top of the file
    if (enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();

      // Populate and set debug and messenger callback struct
      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
      createInfo.enabledLayerCount = 0;

      createInfo.pNext = nullptr;
    }

    // Create a Vulkan Instance + check for successful creation
    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
      throw std::runtime_error("ERROR: Failed to create Vulkan instance!");
    }
  }

  // Initialise function to populate message call back create info struct
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
  }

  // Initialise function to setup debug messenger callback
  void setupDebugMessenger() {
    // Check if validation layers are enabled
    if (!enableValidationLayers) return;

    // Create the create info struct and populate it
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    
    // Setup debug messenger callback for Vulkan instance
    if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    } 

  }

  // Initialise function for find all available queue families
  QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndicies indices;

    // Find number of available queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // Find all available queue families
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Check if family has needed queue in it
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
        std::cout << "Graphics Family found, at index: " << i << std::endl;
      }

      // early exit if all queues are found
      if (indices.isComplete()) {
        break;
      }

      i++;
    }

    return indices;
  }

  // Initialse function to find a suitable GPU
  void pickPhysicalDevice() {
    // Find number of all the devices available
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

    // If no Vulkan supported GPUs found exit
    if (deviceCount == 0) {
      throw std::runtime_error("Failed to find GPUs with Vulkan support!!");
    }

    // Allocate all available devices into list
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    // Find a suitable GPU with conditions we set
    for (const auto& device : devices) {
      if (isDeviceSuitable(device)) {
        _physicalDevice = device;
        break;
      }
    }    

    // If no suitable GPU, throw error
    if (_physicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("Failed to find a suitable GPU!!");
    }
  }

  // Initialse function to check if physical device is suitable
  bool isDeviceSuitable(VkPhysicalDevice device) {
    // Get all device properties and features supported
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Print out device names
    std::cout << "DEVICE: " << deviceProperties.deviceName << std::endl;

    // Check if physical device supports needed queue families
    QueueFamilyIndicies indices = findQueueFamilies(device);

    return indices.isComplete();
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

    // If validation layers are enabled clean up messenger callback
    if (enableValidationLayers) {
       DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
    }

    // Destry Vulkan instance
    vkDestroyInstance(_instance, nullptr);

    // Remove _window from memory
    glfwDestroyWindow(_window);

    // Terminate GLFW instance
    glfwTerminate();
  }

  // Initalize checkValidationLayerSupport()
  bool checkValidationLayerSupport() {
    // Define var for num of layers available
    uint32_t layerCount;
    // Find cound of all available layers
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // Get all the available layers name and store in vector
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Check all selected layers to see if they are in the available layers
    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        std::cout << layerName << " -> " << layerProperties.layerName << std::endl;;
        if (strcmp(layerName, layerProperties.layerName) == 0){
          layerFound = true;
          break;
        }
      }

      // return false if layer is not available
      if (!layerFound) {
        return false;
      }
    }

    // return true if all layers are available
    return true;
  }

  // Initialise custom message callback function
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    
    // Print message from the pCallbackData parameter
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    // Return abort message if callback gets this far
    return VK_FALSE;
  } 

  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
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
