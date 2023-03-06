#pragma once

#include "model.hpp"

#include <memory>

namespace vktut {

struct Transform2DComponent {
    glm::vec2 translation; // position offset
    glm::vec2 scale{1.0f, 1.0f};
    float rotation;

    glm::mat2 mat2() {
        const float sinRotation = glm::sin(rotation);
        const float cosRotation = glm::cos(rotation);

        glm::mat2 rotationMatrix {{cosRotation, sinRotation}, {-sinRotation, cosRotation}};
        glm::mat2 scaleMatrix {{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotationMatrix * scaleMatrix;
    }
};

class GameObject {
    public:

        using id_t = unsigned int;

        static GameObject CreateGameObject() {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        GameObject(const GameObject &) = delete;
        GameObject &operator=(const GameObject&) = delete;
        GameObject(GameObject &&) = default;
        GameObject &operator=(GameObject&&) = default;

        id_t GetId() {return _id;}

        std::shared_ptr<Model> model{};
        glm::vec3 colour{};
        Transform2DComponent transform2D{};


    private:
        GameObject(id_t objectId) : _id{objectId} {}

        id_t _id;

};
    
} // namespace vktut
