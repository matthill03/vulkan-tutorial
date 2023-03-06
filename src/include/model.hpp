#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace vktut {
class Model {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 colour;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };
        

        Model(Device& device, const std::vector<Vertex>& verticies);
        ~Model();

        Model(const Model &) = delete;
        Model &operator =(const Model &) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex>& verticies);

        Device& _device;
        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertextBufferMemory;
        uint32_t _vertexCount;

};
    
} // namespace ember
