cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
project(vk-learning)

file(GLOB pve_SRC CONFIGURE_DEPENDS
        "src/*.hpp"
        "src/*.cpp"
        "src/**/*.hpp"
        "src/**/*.cpp"
)

add_executable(vk-learning ${pve_SRC}
        src/engine/DeviceSelection.cpp
        src/engine/DeviceSelection.hpp)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
