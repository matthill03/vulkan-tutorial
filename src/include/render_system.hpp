#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"

// std
#include <memory>
#include <vector>

namespace vktut {
class RenderSystem {

    public:
        RenderSystem(Device& device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator =(const RenderSystem &) = delete;

        void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects);


    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);

        Device& _device;

        std::unique_ptr<Pipeline> _pipeline;
        VkPipelineLayout _pipelineLayout;
};
}