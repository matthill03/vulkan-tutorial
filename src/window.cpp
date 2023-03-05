#include "include/window.hpp"

namespace vktut {

    Window::Window(int width, int height, std::string name) {
        _width = width;
        _height = height;
        _name = name;
        
        InitWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void Window::InitWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        _window = glfwCreateWindow(_width, _height, _name.c_str(), NULL, NULL);
        glfwSetWindowUserPointer(_window, this);
        glfwSetFramebufferSizeCallback(_window, FrameBufferResizedCallback);
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, _window, NULL, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void Window::FrameBufferResizedCallback(GLFWwindow* window, int width, int height) {
        auto emberWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        emberWindow->_frameBufferResized = true;
        emberWindow->_width = width;
        emberWindow->_height = height;
    }

} // namspace sve
