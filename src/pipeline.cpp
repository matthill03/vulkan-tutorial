#include "include/pipeline.hpp"
#include "include/model.hpp"

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

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(_device.GetDevice(), _vertShaderModule, NULL);
        vkDestroyShaderModule(_device.GetDevice(), _fragShaderModule, NULL);
        vkDestroyPipeline(_device.GetDevice(), _graphicsPipeline, NULL);
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


    void Pipeline::CreateGraphicsPipeline(
        const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) {
            assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no pipelineLayout provided in configInfo");
            assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no renderPass provided in configInfo");

            auto vertCode = ReadFile(vertFilePath);
            auto fragCode = ReadFile(fragFilePath);

            CreateShaderModule(vertCode, &_vertShaderModule);
            CreateShaderModule(fragCode, &_fragShaderModule);

            VkPipelineShaderStageCreateInfo shaderStateInfo[2];
            shaderStateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStateInfo[0].module = _vertShaderModule;
            shaderStateInfo[0].pName = "main";
            shaderStateInfo[0].flags = 0;
            shaderStateInfo[0].pNext = NULL;
            shaderStateInfo[0].pSpecializationInfo = NULL;

            shaderStateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStateInfo[1].module = _fragShaderModule;
            shaderStateInfo[1].pName = "main";
            shaderStateInfo[1].flags = 0;
            shaderStateInfo[1].pNext = NULL;
            shaderStateInfo[1].pSpecializationInfo = NULL;

            auto bindingDescriptions = Model::Vertex::GetBindingDescription();
            auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
            vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStateInfo;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &configInfo.inputAssembalyInfo;
            pipelineInfo.pViewportState = &configInfo.viewportInfo;
            pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
            pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
            pipelineInfo.pColorBlendState = &configInfo.colourBlendInfo;
            pipelineInfo.pDepthStencilState = &configInfo.dephtStencilInfo;
            pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

            pipelineInfo.layout = configInfo.pipelineLayout;
            pipelineInfo.renderPass = configInfo.renderPass;
            pipelineInfo.subpass = configInfo.subpass;

            pipelineInfo.basePipelineIndex = -1;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

            if (vkCreateGraphicsPipelines(_device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &_graphicsPipeline) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create graphics pipeline successfully");
            }

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

    void Pipeline::Bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
    }

    void Pipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {

        configInfo.inputAssembalyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.inputAssembalyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.inputAssembalyInfo.primitiveRestartEnable = VK_FALSE;

        configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        configInfo.viewportInfo.viewportCount = 1;
        configInfo.viewportInfo.pViewports = nullptr;
        configInfo.viewportInfo.scissorCount = 1;
        configInfo.viewportInfo.pScissors = nullptr;

        configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.rasterizationInfo.lineWidth = 1.0f;
        configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

        configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        configInfo.multisampleInfo.minSampleShading = 1.0f;
        configInfo.multisampleInfo.pSampleMask = NULL;
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

        configInfo.colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        configInfo.colourBlendAttachment.blendEnable = VK_FALSE;
        configInfo.colourBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        configInfo.colourBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        configInfo.colourBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        configInfo.colourBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        configInfo.colourBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        configInfo.colourBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        configInfo.colourBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.colourBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.colourBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        configInfo.colourBlendInfo.attachmentCount = 1;
        configInfo.colourBlendInfo.pAttachments = &configInfo.colourBlendAttachment;
        configInfo.colourBlendInfo.blendConstants[0] = 0.0f;
        configInfo.colourBlendInfo.blendConstants[1] = 0.0f;
        configInfo.colourBlendInfo.blendConstants[2] = 0.0f;
        configInfo.colourBlendInfo.blendConstants[3] = 0.0f;

        configInfo.dephtStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.dephtStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.dephtStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.dephtStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.dephtStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.dephtStencilInfo.minDepthBounds = 0.0f;
        configInfo.dephtStencilInfo.maxDepthBounds = 1.0f;
        configInfo.dephtStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.dephtStencilInfo.front = {};
        configInfo.dephtStencilInfo.back = {};

        configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
        configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
        configInfo.dynamicStateInfo.flags = 0;
    }

} // namespace ember