cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
project(vk-learning)

file(GLOB ve_SRC CONFIGURE_DEPENDS
        "src/*.hpp"
        "src/*.cpp"
)

add_executable(vk-learning ${ve_SRC})

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
