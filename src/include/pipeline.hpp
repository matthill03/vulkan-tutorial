#pragma once

#include "device.hpp"
#include <string>
#include <vector>

namespace ember {

struct PipelineConfigInfo {
    
};
class Pipeline {
    public:
        Pipeline(
            Device& device, 
            const std::string& vertFilePath, 
            const std::string& fragFilePath, 
            const PipelineConfigInfo& configInfo);
        ~Pipeline() {};

        Pipeline(const Pipeline&) = delete;
        void operator=(const Pipeline&) = delete;

        static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);
    
    private:
        static std::vector<char> ReadFile(const std::string filePath);

        void CreateGraphicsPipeline(
            const std::string& vertFilePath, 
            const std::string& fragFilePath, 
            const PipelineConfigInfo& configInfo);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        Device& _device;
        VkPipeline _graphicsPipeline;
        VkShaderModule _vertShaderModule;
        VkShaderModule _fragShaderModule;
};

} // namespace ember