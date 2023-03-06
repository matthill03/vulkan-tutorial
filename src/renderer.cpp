#include "include/renderer.hpp"

//std
#include <array>

namespace vktut {
    Renderer::Renderer(Window& window, Device& device) : _window{window}, _device{device} {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    Renderer::~Renderer() {
        FreeCommandBuffers();
    }

    void Renderer::RecreateSwapChain() {
        auto extent = _window.GetExtent();

        while (extent.width == 0 || extent.height == 0) {
            extent = _window.GetExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device.GetDevice());
        if (_swapChain == nullptr) {
            _swapChain = std::make_unique<SwapChain>(_device, extent);
        }
        else {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapChain);
            _swapChain = std::make_unique<SwapChain>(_device, extent, oldSwapChain);

            if (!oldSwapChain->CompareSwapFormats(*_swapChain.get())) {
                throw std::runtime_error("Swap chain image(or depth) format has changed");
            }
        }

        // Coming back here!!!
    }

    void Renderer::CreateCommandBuffers() {
        _commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if (vkAllocateCommandBuffers(_device.GetDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            std::runtime_error("Failed to allocate command buffers");
        }
    }

    void Renderer::FreeCommandBuffers() {
        vkFreeCommandBuffers(_device.GetDevice(), _device.GetCommandPool(), static_cast<float>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

        VkCommandBuffer Renderer::BeginFrame() {
            assert(!_isFrameStarted && "Can't call BeginFrame while already in progress");

            auto result = _swapChain->AcquireNextImage(&_currentImageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                RecreateSwapChain();
                return nullptr;
            }

            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("Failed to aquire swap chain image");
            }

            _isFrameStarted = true;

            auto commandBuffer = GetCurrentCommandBuffer();

            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                std::runtime_error("Failed to begin recording command buffer");
            }

            return commandBuffer;
        }
        
        void Renderer::EndFrame() {
            assert(_isFrameStarted && "Can't call EndFrame while frame is not in progress");
            auto commandBuffer = GetCurrentCommandBuffer();

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }

            auto result = _swapChain->SubmitCommandBuffers(&commandBuffer, &_currentImageIndex);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window.WasWindowResized()) {
                _window.ResetWindowResizedFlag();
                RecreateSwapChain();
            } else if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to present swap chain image");
            }

            _isFrameStarted = false;
            _currentFrameIndex = (_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
        }

        void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
            assert(_isFrameStarted && "Can't BeginSwapChainRenderPass if frame is not in progress");
            assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = _swapChain->GetRenderPass();
            renderPassInfo.framebuffer = _swapChain->GetFrameBuffer(_currentImageIndex);
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = _swapChain->GetSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport;
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(_swapChain->GetSwapChainExtent().width);
            viewport.height = static_cast<float>(_swapChain->GetSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0,0}, _swapChain->GetSwapChainExtent()};
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        }
        void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
            assert(_isFrameStarted && "Can't EndSwapChainRenderPass if frame is not in progress");
            assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

            vkCmdEndRenderPass(commandBuffer);

        }

} // namespace vaktut