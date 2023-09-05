cmake_minimum_required(VERSION 3.5)

project(prime-vulkan
    VERSION 0.1.0
    DESCRIPTION "Primer based Vulkan library."
    LANGUAGES CXX)

add_library(prime-vulkan SHARED
    src/base.cpp
    src/extension-properties.cpp
    src/vk-instance.cpp
    src/physical-device.cpp
    src/device.cpp
    src/surface.cpp
    src/queue.cpp
    src/swapchain.cpp
    src/shader-module.cpp
    src/pipeline.cpp
    src/render-pass.cpp
    src/framebuffer.cpp
    src/command-pool.cpp
    src/command-buffer.cpp
    src/semaphore.cpp
    src/fence.cpp
    src/buffer.cpp
    src/memory.cpp
    include/prime-vulkan/base.h
    include/prime-vulkan/extension-properties.h
    include/prime-vulkan/vulkan.h
    include/prime-vulkan/instance.h
    include/prime-vulkan/physical-device.h
    include/prime-vulkan/device.h
    include/prime-vulkan/surface.h
    include/prime-vulkan/queue.h
    include/prime-vulkan/swapchain.h
    include/prime-vulkan/shader-module.h
    include/prime-vulkan/pipeline.h
    include/prime-vulkan/render-pass.h
    include/prime-vulkan/framebuffer.h
    include/prime-vulkan/command-pool.h
    include/prime-vulkan/command-buffer.h
    include/prime-vulkan/semaphore.h
    include/prime-vulkan/fence.h
    include/prime-vulkan/buffer.h
    include/prime-vulkan/memory.h
)

target_include_directories(prime-vulkan
    PRIVATE ./include
)

target_compile_features(prime-vulkan
    PRIVATE cxx_std_17)

target_compile_options(prime-vulkan
    PRIVATE -Wall -Wextra -O2)
string(REPLACE "-O3" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
target_link_options(prime-vulkan
    PRIVATE -O2)

# Link libprimer.
target_link_libraries(prime-vulkan
    PRIVATE primer)

# Version info.
set_target_properties(prime-vulkan PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_custom_target(tests
    SOURCES
    tests/basic/main.cpp
    tests/vertex/application.h
    tests/vertex/application.cpp
    tests/vertex/wayland.h
    tests/vertex/main.cpp
)
