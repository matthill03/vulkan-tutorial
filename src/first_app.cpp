#include "include/first_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>

namespace vktut {

    struct SimplePushConstantData {
        glm::mat2 transform{1.f};
        glm::vec2 offset;
        alignas(16) glm::vec3 colour;
    };

    FirstApp::FirstApp() {
        LoadGameObjects();
        CreatePipelineLayout();
        RecreateSwapChain();
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

    void FirstApp::LoadGameObjects() {
        std::vector<Model::Vertex> verticies = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        auto model = std::make_shared<Model>(_device, verticies);

        auto triangle = GameObject::CreateGameObject();
        triangle.model = model;
        triangle.colour = {0.1f, 0.8f, 0.1f};
        triangle.transform2D.translation.x = 0.2f;
        triangle.transform2D.scale = {2.0f, 0.5f};
        triangle.transform2D.rotation = 0.25 * glm::two_pi<float>();

        _gameObjects.push_back(std::move(triangle));
    }

    void FirstApp::CreatePipelineLayout() {

        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = NULL;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(_device.GetDevice(), &pipelineLayoutInfo, NULL, &_pipelineLayout) != VK_SUCCESS) {
            std::runtime_error("Failed to create pipeline layout");
        }
    }

    void FirstApp::CreatePipeline() {
        assert(_swapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = _swapChain->GetRenderPass();
        pipelineConfig.pipelineLayout = _pipelineLayout;
        _pipeline = std::make_unique<Pipeline>(_device, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void FirstApp::RecreateSwapChain() {
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
            _swapChain = std::make_unique<SwapChain>(_device, extent, std::move(_swapChain));
            if (_swapChain->GetImageCount() != _commandBuffers.size()) {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }
        CreatePipeline();
    }

    void FirstApp::CreateCommandBuffers() {
        _commandBuffers.resize(_swapChain->GetImageCount());
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if (vkAllocateCommandBuffers(_device.GetDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            std::runtime_error("Failed to allocate command buffers");
        }
    }

    void FirstApp::FreeCommandBuffers() {
        vkFreeCommandBuffers(_device.GetDevice(), _device.GetCommandPool(), static_cast<float>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

    void FirstApp::RecordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
                std::runtime_error("Failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = _swapChain->GetRenderPass();
            renderPassInfo.framebuffer = _swapChain->GetFrameBuffer(imageIndex);
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = _swapChain->GetSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport;
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(_swapChain->GetSwapChainExtent().width);
            viewport.height = static_cast<float>(_swapChain->GetSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0,0}, _swapChain->GetSwapChainExtent()};
            vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

            RenderGameObjects(_commandBuffers[imageIndex]);

            vkCmdEndRenderPass(_commandBuffers[imageIndex]);
            if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
    }

    void FirstApp::RenderGameObjects(VkCommandBuffer commandBuffer) {
        _pipeline->Bind(commandBuffer);

        for(auto& object : _gameObjects) {
            object.transform2D.rotation = glm::mod(object.transform2D.rotation + 0.01f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = object.transform2D.translation;
            push.colour = object.colour;
            push.transform = object.transform2D.mat2();

            vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

            object.model->Bind(commandBuffer);
            object.model->Draw(commandBuffer);
        }
    }

    void FirstApp::DrawFrame(){
        uint32_t imageIndex;
        auto result = _swapChain->AcquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to aquire swap chain image");
        }

        RecordCommandBuffer(imageIndex);
        result = _swapChain->SubmitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window.WasWindowResized()) {
            _window.ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image");
        }
    }

} // namespace ember