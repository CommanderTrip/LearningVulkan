#include "ve_pipeline.hpp"

#include <fstream>
#include <iostream>

namespace ve {

VePipeline::VePipeline(const std::string& vertexShaderCodePath, const std::string& fragmentShaderCodePath) {
    createGraphicsPipeline(vertexShaderCodePath, fragmentShaderCodePath);
}

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
                                        const std::string& fragmentShaderCodePath) {
    std::vector<char> vertexShaderCode = readFile(vertexShaderCodePath);
    std::vector<char> fragmentShaderCode = readFile(fragmentShaderCodePath);

    std::cout << "Vertex shader code size " << vertexShaderCode.size() << "\n";
    std::cout << "Fragment shader code size " << fragmentShaderCode.size() << "\n";
}

}  // namespace ve