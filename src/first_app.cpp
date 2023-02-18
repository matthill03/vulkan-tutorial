#include "first_app.hpp"

namespace ember {
    
    void FirstApp::Run() {
        while (!_window.ShouldClose()) {
            glfwPollEvents();
        }
    }
}