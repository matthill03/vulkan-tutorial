#include "include/model.hpp"

#include <cassert>
#include <cstring>

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

    std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescription() {
        std::vector<VkVertexInputBindingDescription> bindingDescription(1);
        bindingDescription[0].binding = 0;
        bindingDescription[0].stride = sizeof(Vertex);
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription; 
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDesctiptions(2);

        attributeDesctiptions[0].binding = 0;
        attributeDesctiptions[0].location = 0;
        attributeDesctiptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDesctiptions[0].offset = offsetof(Vertex, position);

        attributeDesctiptions[1].binding = 0;
        attributeDesctiptions[1].location = 1;
        attributeDesctiptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDesctiptions[1].offset = offsetof(Vertex, colour);
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
}