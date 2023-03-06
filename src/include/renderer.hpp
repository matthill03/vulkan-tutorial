#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

// std
#include <memory>
#include <vector>

namespace vktut {
class Renderer {
    public:
        Renderer(Window& window, Device& device);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator =(const Renderer &) = delete;

        VkRenderPass GetSwapChainRenderPass() const {return _swapChain->GetRenderPass();}
        bool IsFrameInPorogress() const {return _isFrameStarted;}

        VkCommandBuffer GetCurrentCommandBuffer() {
            assert(_isFrameStarted && "Cannot get command buffer when frame not in progress");
            return _commandBuffers[_currentFrameIndex];
        }

        int GetFrameIndex() const {
            assert(_isFrameStarted && "Cannt get frame index when frame is not in progress");
            return _currentFrameIndex;
        }

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();

        Window& _window;
        Device& _device;
        std::unique_ptr<SwapChain> _swapChain;
        std::vector<VkCommandBuffer> _commandBuffers;

        uint32_t _currentImageIndex {0};
        int _currentFrameIndex {0};
        bool _isFrameStarted {false};


};
}