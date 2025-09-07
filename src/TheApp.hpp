#pragma once
#include "ve_device.hpp"
#include "ve_pipeline.hpp"
#include "ve_window.hpp"
namespace ve {

class TheApp {
    VeWindow _window{WIDTH, HEIGHT, "Hello Vulkan"};
    VeDevice _device{_window};
    VePipeline _pipeline{_device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv",
                         VePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

public:
    static constexpr int WIDTH = 600;
    static constexpr int HEIGHT = 400;

    TheApp() = default;
    ~TheApp() = default;
    void run() const;
};

}  // namespace ve
