#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>

namespace vktut {
class Model {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 colour;
            glm::vec3 normal;
            glm::vec2 texcoord;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return position == other.position && colour == other.colour && normal == other.normal && texcoord == other.texcoord;
            }
        };
        
        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void LoadModel(const std::string& filepath);
        };

        Model(Device& device, const Model::Builder& builder);
        ~Model();

        Model(const Model &) = delete;
        Model &operator =(const Model &) = delete;

        static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string filepath);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex>& verticies);
        void CreateIndexBuffers(const std::vector<uint32_t>& indices);

        Device& _device;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        uint32_t _vertexCount;

        bool _hasIndexBuffer = false;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;
        uint32_t _indexCount;

};
    
} // namespace ember
