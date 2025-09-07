#include "ve_pipeline.hpp"

#include <fstream>
#include <iostream>

namespace ve {

VePipeline::VePipeline(VeDevice& device, const std::string& vertexShaderCodePath,
                       const std::string& fragmentShaderCodePath, const PipelineConfigInfo& configInfo)
    : _device(device) {
    createGraphicsPipeline(vertexShaderCodePath, fragmentShaderCodePath, configInfo);
}

VePipeline::~VePipeline() {}

std::vector<char> VePipeline::readFile(const std::string& filePath) {
    // 'ate' starts at the end of the file to quickly get size
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) throw std::runtime_error("Failed to open file");

    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void VePipeline::createGraphicsPipeline(const std::string& vertexShaderCodePath,
                                        const std::string& fragmentShaderCodePath,
                                        const PipelineConfigInfo& configInfo) {
    std::vector<char> vertexShaderCode = readFile(vertexShaderCodePath);
    std::vector<char> fragmentShaderCode = readFile(fragmentShaderCodePath);

    std::cout << "Vertex shader code size " << vertexShaderCode.size() << "\n";
    std::cout << "Fragment shader code size " << fragmentShaderCode.size() << "\n";
}
void VePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(_device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
}

PipelineConfigInfo VePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo configInfo{};
    return configInfo;
}

}  // namespace ve