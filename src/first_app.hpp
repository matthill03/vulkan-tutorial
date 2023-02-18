#pragma once

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"

namespace ember {
class FirstApp {

    public:
        int width = 800;
        int height = 600;
        
        void Run();

    private:
        Window _window{width, height, "Hi There matey"};
        Device _device{_window};
        Pipeline _pipeline{
            _device,
            "src/shaders/simple_shader.vert.spv",
            "src/shaders/simple_shader.frag.spv", 
            Pipeline::DefaultPipelineConfigInfo(width, height)};

};
}