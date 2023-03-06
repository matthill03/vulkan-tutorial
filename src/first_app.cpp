#include "include/first_app.hpp"
#include "include/render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>

namespace vktut {
    FirstApp::FirstApp() {
        LoadGameObjects();
    }

    FirstApp::~FirstApp() {
    }

    void FirstApp::Run() {
        RenderSystem renderSystem {_device, _renderer.GetSwapChainRenderPass()};

        while (!_window.ShouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = _renderer.BeginFrame()) {
                _renderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(commandBuffer, _gameObjects);
                _renderer.EndSwapChainRenderPass(commandBuffer);
                _renderer.EndFrame();
            }

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

} // namespace ember