#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "game_object.hpp"

// std
#include <memory>
#include <vector>

namespace vktut {
class FirstApp {

    public:
        int width = 800;
        int height = 600;
        
        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &) = delete;
        FirstApp &operator =(const FirstApp &) = delete;

        void Run();

    private:
        void LoadGameObjects();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        void RenderGameObjects(VkCommandBuffer commandBuffer);

        Window _window{width, height, "Hi There lad"};
        Device _device{_window};
        std::unique_ptr<SwapChain> _swapChain;
        std::unique_ptr<Pipeline> _pipeline;
        VkPipelineLayout _pipelineLayout;
        std::vector<VkCommandBuffer> _commandBuffers;
        std::vector<GameObject> _gameObjects;


};
}