#include "include/render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>

namespace vktut {

    struct SimplePushConstantData {
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 colour;
    };

    RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass) : _device{device} {
        CreatePipelineLayout();
        CreatePipeline(renderPass);
    }

    RenderSystem::~RenderSystem() {
        vkDestroyPipelineLayout(_device.GetDevice(), _pipelineLayout, NULL);
    }

    void RenderSystem::CreatePipelineLayout() {

        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = NULL;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(_device.GetDevice(), &pipelineLayoutInfo, NULL, &_pipelineLayout) != VK_SUCCESS) {
            std::runtime_error("Failed to create pipeline layout");
        }
    }

    void RenderSystem::CreatePipeline(VkRenderPass renderPass) {
        assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = _pipelineLayout;
        _pipeline = std::make_unique<Pipeline>(_device, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv", pipelineConfig);
    }

    void RenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects) {
        _pipeline->Bind(commandBuffer);

        for(auto& object : gameObjects) {
            object.transform.rotation.y = glm::mod(object.transform.rotation.y + 0.01f, glm::two_pi<float>());
            object.transform.rotation.x = glm::mod(object.transform.rotation.x + 0.005f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.colour = object.colour;
            push.transform = object.transform.mat4();

            vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

            object.model->Bind(commandBuffer);
            object.model->Draw(commandBuffer);
        }
    }

} // namespace ember