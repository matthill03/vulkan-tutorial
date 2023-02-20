#include "include/first_app.hpp"

//std
#include <array>

namespace ember {

    FirstApp::FirstApp() {
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(_device.GetDevice(), _pipelineLayout, NULL);
    }

    void FirstApp::Run() {
        while (!_window.ShouldClose()) {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(_device.GetDevice());
    }

    void FirstApp::CreatePipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = NULL;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = NULL;

        if (vkCreatePipelineLayout(_device.GetDevice(), &pipelineLayoutInfo, NULL, &_pipelineLayout) != VK_SUCCESS) {
            std::runtime_error("Failed to create pipeline layout");
        }
    }

    void FirstApp::CreatePipeline() {
        auto pipelineConfig = Pipeline::DefaultPipelineConfigInfo(_swapChain.GetWidth(), _swapChain.GetHeight());
        pipelineConfig.renderPass = _swapChain.GetRenderPass();
        pipelineConfig.pipelineLayout = _pipelineLayout;
        _pipeline = std::make_unique<Pipeline>(_device, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void FirstApp::CreateCommandBuffers() {
        _commandBuffers.resize(_swapChain.GetImageCount());
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if (vkAllocateCommandBuffers(_device.GetDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            std::runtime_error("Failed to allocate command buffers");
        }

        for (int i = 0; i < _commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                std::runtime_error("Failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = _swapChain.GetRenderPass();
            renderPassInfo.framebuffer = _swapChain.GetFrameBuffer(i);
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = _swapChain.GetSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            _pipeline->Bind(_commandBuffers[i]);
            vkCmdDraw(_commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(_commandBuffers[i]);
            if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
        }
    }

    void FirstApp::DrawFrame(){
        uint32_t imageIndex;
        auto result = _swapChain.AcquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image");
        }

        result = _swapChain.SubmitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image");
        }
    }

} // namespace ember