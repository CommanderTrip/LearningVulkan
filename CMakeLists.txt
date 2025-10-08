cmake_minimum_required(VERSION 3.13)
project(glLearning)

set(CMAKE_CXX_STANDARD 20)

file(GLOB glLearning_SRC CONFIGURE_DEPENDS
        "src/*.hpp"
        "src/*.cpp"
        "src/**/*.hpp"
        "src/**/*.cpp"
)

add_executable(${PROJECT_NAME} ${glLearning_SRC})


# Handles the platform specific windowing and input
find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

# Gets the OpenGL specific driver implementations
find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)