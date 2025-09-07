#pragma once
#include <string>
#include <vector>

namespace ve {

class VePipeline {
    static std::vector<char> readFile(const std::string& filePath);
    void createGraphicsPipeline(const std::string& vertexShaderCodePath, const std::string& fragmentShaderCodePath);

public:
    VePipeline(const std::string& vertexShaderCodePath, const std::string& fragmentShaderCodePath);
};

}  // namespace ve