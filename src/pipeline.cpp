#include "include/pipeline.hpp"

// std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace ember {

    Pipeline::Pipeline(Device& device, 
            const std::string& vertFilePath, 
            const std::string& fragFilePath, 
            const PipelineConfigInfo& configInfo) : _device{device} {

        CreateGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
    }


    std::vector<char> Pipeline::ReadFile(const std::string filePath) {
        std::ifstream file{filePath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(_device.GetDevice(), &createInfo, NULL, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }

    }

    PipelineConfigInfo Pipeline::DefaultPipelineConfigInfo(uint32_t width, uint32_t height) {
        PipelineConfigInfo configInfo{};

        return configInfo;
    }


    void Pipeline::CreateGraphicsPipeline(
        const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) {
            auto vertCode = ReadFile(vertFilePath);
            auto fragCode = ReadFile(fragFilePath);

            std::cout << "Vertex Shader Size: " << vertCode.size() << '\n';
            std::cout << "Fragment Shader Size: " << fragCode.size() << '\n';

    }
} // namespace ember