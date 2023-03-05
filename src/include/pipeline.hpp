#pragma once

#include "device.hpp"
#include <string>
#include <vector>

namespace vktut {

struct PipelineConfigInfo {
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembalyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colourBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colourBlendInfo;
    VkPipelineDepthStencilStateCreateInfo dephtStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};
class Pipeline {
    public:
        Pipeline(
            Device& device, 
            const std::string& vertFilePath, 
            const std::string& fragFilePath, 
            const PipelineConfigInfo& configInfo);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        void operator=(const Pipeline&) = delete;

        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

        void Bind(VkCommandBuffer commandBuffer);
    
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