#include "include/first_app.hpp"

namespace vktut {
    FirstApp::FirstApp() {
        LoadGameObjects();
    }

    FirstApp::~FirstApp() {
    }

    void FirstApp::Run() {
        RenderSystem renderSystem {_device, _renderer.GetSwapChainRenderPass()};

        Camera camera{};
        // camera.SetViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
        camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

        auto viewerObject = GameObject::CreateGameObject();
        KeyboardMovementController cameraController{};

        float aspect = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto newTime = std::chrono::high_resolution_clock::now();

        float frameTime = 0.0f;

        while (!_window.ShouldClose()) {
            glfwPollEvents();

            newTime = std::chrono::high_resolution_clock::now();
            frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.MoveInPlaneXZ(_window.GetGlfwWindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            aspect = _renderer.GetAspectRatio();
            // camera.SetOthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);


            if (auto commandBuffer = _renderer.BeginFrame()) {
                _renderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(commandBuffer, _gameObjects, camera);
                _renderer.EndSwapChainRenderPass(commandBuffer);
                _renderer.EndFrame();
            }

        }

        vkDeviceWaitIdle(_device.GetDevice());
    }

    std::unique_ptr<Model> CreateCubeModel(Device& device, glm::vec3 offset) {
        Model::Builder modelBuilder{};

        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }
        
        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
        
        return std::make_unique<Model>(device, modelBuilder);
    }

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<Model> model = CreateCubeModel(_device, {0.0f, 0.0f, 0.0f});

        auto cube = GameObject::CreateGameObject();
        cube.model = model;
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = {0.5f, 0.5f, 0.5f};

        _gameObjects.push_back(std::move(cube));
    }

} // namespace ember