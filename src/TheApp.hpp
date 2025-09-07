#pragma once
#include "ve_pipeline.hpp"
#include "ve_window.hpp"

namespace ve {

class TheApp {
    VeWindow _window{WIDTH, HEIGHT, "Hello Vulkan"};
    VePipeline _pipeline{"../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv"};

public:
    static constexpr int WIDTH = 600;
    static constexpr int HEIGHT = 400;

    TheApp() = default;
    ~TheApp() = default;
    void run() const;
};

}  // namespace ve
