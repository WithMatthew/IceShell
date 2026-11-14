cmake_minimum_required(VERSION 3.25)

project(lesson00 VERSION 0.1.0 LANGUAGES CXX)

# 使用标准 C++ 方言；此设置作为后续目标的默认值。
set(CMAKE_CXX_EXTENSIONS OFF)

# 将引擎实现编译并归档为静态库。
add_library(vle_core STATIC
    src/engine_info.cpp
)

# PUBLIC：库自身以及链接它的目标都需要这些编译条件。
target_compile_features(vle_core PUBLIC cxx_std_20)

target_include_directories(vle_core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 程序只列出自己的源文件，引擎实现由库提供。
add_executable(vle_sandbox
    src/main.cpp
)

# 链接静态库，同时继承其公开的头文件路径和 C++ 标准要求。
target_link_libraries(vle_sandbox PRIVATE vle_core)
