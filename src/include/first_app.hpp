#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

// std
#include <memory>
#include <vector>

namespace ember {
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
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();

        Window _window{width, height, "Hi There matey"};
        Device _device{_window};
        SwapChain _swapChain {_device, _window.GetExtent()};
        std::unique_ptr<Pipeline> _pipeline;
        VkPipelineLayout _pipelineLayout;
        std::vector<VkCommandBuffer> _commandBuffers;
        std::unique_ptr<Model> _model;


};
}