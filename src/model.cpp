#include "include/model.hpp"

#include <cassert>
#include <cstring>

namespace vktut {

    Model::Model(Device& device, const std::vector<Vertex>& verticies) : _device{device} {
        CreateVertexBuffers(verticies);
    }

    Model::~Model() {
        vkDestroyBuffer(_device.GetDevice(), _vertexBuffer, NULL);
        vkFreeMemory(_device.GetDevice(), _vertextBufferMemory, NULL);
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
        attributeDesctiptions[0].format = VK_FORMAT_R32G32_SFLOAT;
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
    }

    void Model::Draw(VkCommandBuffer commandBuffer) {
        vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
    }

    void Model::CreateVertexBuffers(const std::vector<Vertex>& verticies) {
        _vertexCount = static_cast<uint32_t>(verticies.size());
        assert(_vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(verticies[0]) * _vertexCount ;

        _device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _vertexBuffer, _vertextBufferMemory);
        
        void* data;
        vkMapMemory(_device.GetDevice(), _vertextBufferMemory, 0, bufferSize, 0, &data);

        memcpy(data, verticies.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device.GetDevice(), _vertextBufferMemory);
    }
}