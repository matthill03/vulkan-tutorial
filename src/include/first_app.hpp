#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <memory>
#include <vector>
#include <array>
#include <chrono>

#define MAX_FRAME_TIME {0.5f}

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