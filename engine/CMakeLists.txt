add_library(vle_core STATIC
    src/core/engine_info.cpp
)
add_library(vle::core ALIAS vle_core)

target_include_directories(vle_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(vle_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(vle_core PRIVATE /W4 /permissive- /utf-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(vle_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
