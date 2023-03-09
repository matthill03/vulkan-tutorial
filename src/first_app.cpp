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

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<Model> model = Model::CreateModelFromFile(_device, "src/models/smooth_vase.obj");

        auto gameObject = GameObject::CreateGameObject();
        gameObject.model = model;
        gameObject.transform.translation = {0.0f, 0.0f, 2.5f};
        gameObject.transform.scale = glm::vec3(3.0f);

        _gameObjects.push_back(std::move(gameObject));
    }

} // namespace ember