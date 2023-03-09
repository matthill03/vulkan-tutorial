#include "include/model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "include/utils.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
    template<>
    struct hash<vktut::Model::Vertex> {
        size_t operator()(vktut::Model::Vertex const &vertex) const {
            size_t seed = 0;
            vktut::HashCombine(seed, vertex.position, vertex.colour, vertex.normal, vertex.texcoord);
            return seed;
        }
    };
}

namespace vktut {

    Model::Model(Device& device, const Model::Builder& builder) : _device{device} {
        CreateVertexBuffers(builder.vertices);
        CreateIndexBuffers(builder.indices);
    }

    Model::~Model() {
        vkDestroyBuffer(_device.GetDevice(), _vertexBuffer, NULL);
        vkFreeMemory(_device.GetDevice(), _vertexBufferMemory, NULL);

        if (_hasIndexBuffer) {
            vkDestroyBuffer(_device.GetDevice(), _indexBuffer, NULL);
            vkFreeMemory(_device.GetDevice(), _indexBufferMemory, NULL);
        }
    }

    std::unique_ptr<Model> Model::CreateModelFromFile(Device& device, const std::string filepath) {
        Builder builder{};

        builder.LoadModel(filepath);
        std::cout << "Vertex Count: " << builder.vertices.size() << "\n";

        return std::make_unique<Model>(device, builder);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescription() {
        std::vector<VkVertexInputBindingDescription> bindingDescription(1);
        bindingDescription[0].binding = 0;
        bindingDescription[0].stride = sizeof(Vertex);
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription; 
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDesctiptions{};

        attributeDesctiptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDesctiptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, colour)});
        attributeDesctiptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDesctiptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texcoord)});

        return attributeDesctiptions;
    }

    void Model::Bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (_hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::Draw(VkCommandBuffer commandBuffer) {
        if (_hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
        }
    }

    void Model::CreateVertexBuffers(const std::vector<Vertex>& verticies) {
        _vertexCount = static_cast<uint32_t>(verticies.size());
        assert(_vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(verticies[0]) * _vertexCount ;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        _device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        
        void* data;
        vkMapMemory(_device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

        memcpy(data, verticies.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device.GetDevice(), stagingBufferMemory);

        _device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

        _device.CopyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(_device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(_device.GetDevice(), stagingBufferMemory, nullptr);
    }

     void Model::CreateIndexBuffers(const std::vector<uint32_t>& indices) {
        _indexCount = static_cast<uint32_t>(indices.size());
        _hasIndexBuffer = _indexCount > 0;

        if (!_hasIndexBuffer) {
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount ;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        _device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        
        void* data;
        vkMapMemory(_device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device.GetDevice(), stagingBufferMemory);

        _device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

        _device.CopyBuffer(stagingBuffer, _indexBuffer, bufferSize);

        vkDestroyBuffer(_device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(_device.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Model::Builder::LoadModel(const std::string& filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warning;
        std::string error;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filepath.c_str())) {
            throw std::runtime_error(warning + error);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices) {
                Vertex vertex{};

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.colour = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                    
                }

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                
                if (index.texcoord_index >= 0) {
                    vertex.texcoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
                
            }
        }
    }
}