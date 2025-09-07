#pragma once

#include <string>
#include <vector>

#include "ve_device.hpp"

namespace ve {

struct PipelineConfigInfo {};

class VePipeline {
    VeDevice& _device;
    VkPipeline _graphicsPipeline;
    VkShaderModule _vertexShaderModule;
    VkShaderModule _fragmentShaderModule;

    static std::vector<char> readFile(const std::string& filePath);
    void createGraphicsPipeline(const std::string& vertexShaderCodePath, const std::string& fragmentShaderCodePath,
                                const PipelineConfigInfo& configInfo);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

public:
    VePipeline(VeDevice& device, const std::string& vertexShaderCodePath, const std::string& fragmentShaderCodePath,
               const PipelineConfigInfo& configInfo);
    ~VePipeline();

    VePipeline(const VePipeline&) = delete;
    VePipeline& operator=(const VePipeline) = delete;

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
};

}  // namespace ve