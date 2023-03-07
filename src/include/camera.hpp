#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vktut {
class Camera {
    public:

        void SetOthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

        const glm::mat4& GetProjection() const {return _projectionMatrix; }
        const glm::mat4& GetView() const {return _viewMatrix; }

    
    private:
        glm::mat4 _projectionMatrix{1.0f};
        glm::mat4 _viewMatrix{1.0f};
};
}