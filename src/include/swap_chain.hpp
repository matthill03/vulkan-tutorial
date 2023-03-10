#pragma once

#include "device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

namespace vktut {

class SwapChain {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  SwapChain(Device &deviceRef, VkExtent2D windowExtent);
  SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previousSwapChain);
  ~SwapChain();

  SwapChain(const SwapChain &) = delete;
  SwapChain& operator=(const SwapChain &) = delete;

  VkFramebuffer GetFrameBuffer(int index) { return _swapChainFramebuffers[index]; }
  VkRenderPass GetRenderPass() { return _renderPass; }
  VkImageView GetImageView(int index) { return _swapChainImageViews[index]; }
  size_t GetImageCount() { return _swapChainImages.size(); }
  VkFormat GetSwapChainImageFormat() { return _swapChainImageFormat; }
  VkExtent2D GetSwapChainExtent() { return _swapChainExtent; }
  uint32_t GetWidth() { return _swapChainExtent.width; }
  uint32_t GetHeight() { return _swapChainExtent.height; }

  float ExtentAspectRatio() {
    return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
  }
  VkFormat FindDepthFormat();

  VkResult AcquireNextImage(uint32_t *imageIndex);
  VkResult SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

  bool CompareSwapFormats(const SwapChain& swapChain) const {
    return swapChain._swapChainDepthFormat == _swapChainDepthFormat && swapChain._swapChainImageFormat == _swapChainImageFormat;
  }

 private:
  void Init();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateDepthResources();
  void CreateRenderPass();
  void CreateFramebuffers();
  void CreateSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat _swapChainImageFormat;
  VkFormat _swapChainDepthFormat;
  VkExtent2D _swapChainExtent;

  std::vector<VkFramebuffer> _swapChainFramebuffers;
  VkRenderPass _renderPass;

  std::vector<VkImage> _depthImages;
  std::vector<VkDeviceMemory> _depthImageMemorys;
  std::vector<VkImageView> _depthImageViews;
  std::vector<VkImage> _swapChainImages;
  std::vector<VkImageView> _swapChainImageViews;

  Device& _device;
  VkExtent2D _windowExtent;

  VkSwapchainKHR _swapChain;
  std::shared_ptr<SwapChain> _oldSwapChain;

  std::vector<VkSemaphore> _imageAvailableSemaphores;
  std::vector<VkSemaphore> _renderFinishedSemaphores;
  std::vector<VkFence> _inFlightFences;
  std::vector<VkFence> _imagesInFlight;
  size_t _currentFrame = 0;
};

}  // namespace lve
