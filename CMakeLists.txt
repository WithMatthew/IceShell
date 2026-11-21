cmake_minimum_required(VERSION 3.25)

project(
  VulkanLearningEngine
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Vulkan REQUIRED)

add_subdirectory(engine)
add_subdirectory(samples/sandbox)
