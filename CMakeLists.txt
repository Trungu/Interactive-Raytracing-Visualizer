cmake_minimum_required(VERSION 3.20)

project(Raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(math_core
    src/math/Vec3.cpp
    src/geometry/Ray.cpp
    src/geometry/Plane.cpp
    src/geometry/Intersection.cpp
    src/geometry/Reflection.cpp
    src/geometry/receiver.cpp
    src/tracing/Trace.cpp
    src/tracing/RayEmitter.cpp
    src/animation/PathAnimation.cpp
    src/animation/Playback.cpp
    src/scene/Room.cpp
    
)

target_include_directories(math_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(Acoustic-Raytracer
    src/main.cpp
    src/rendering/ShaderProgram.cpp
    src/rendering/Renderer.cpp
    src/rendering/Camera.cpp
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(Acoustic-Raytracer PRIVATE
    math_core
    glfw
    OpenGL::GL
    glm::glm
)

include(CTest)

if(BUILD_TESTING)
    add_executable(intersection_tests tests/intersection_tests.cpp)
    target_link_libraries(intersection_tests PRIVATE math_core)

    add_test(NAME intersections COMMAND intersection_tests)

endif()
