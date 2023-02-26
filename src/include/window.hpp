#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"
#include <string>
#include <stdexcept>

namespace ember {

class Window {

    public:
        Window(int width, int height, std::string name);
        Window(const Window &) = delete;
        Window &operator =(const Window &) = delete;
        ~Window();

        bool ShouldClose() {return glfwWindowShouldClose(_window);};
        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        VkExtent2D GetExtent() { return {static_cast<uint32_t>(_width), static_cast<uint32_t>(_height)}; };

        bool WasWindowResized() {return _frameBufferResized; };
        void ResetWindowResizedFlag() {_frameBufferResized = false; };

    private:
        int _width;
        int _height;
        bool _frameBufferResized = false;

        std::string _name;
        GLFWwindow* _window;

        void InitWindow();
        static void FrameBufferResizedCallback(GLFWwindow* window, int width, int height);


};
}
