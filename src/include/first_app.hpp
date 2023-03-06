#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"

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
        void RenderGameObjects(VkCommandBuffer commandBuffer);

        Window _window{width, height, "Hi There lad"};
        Device _device{_window};
        Renderer _renderer{_window, _device};
        std::vector<GameObject> _gameObjects;
};
}