add_executable(vle_sandbox main.cpp)
target_link_libraries(vle_sandbox PRIVATE vle::core)
target_link_libraries(vle_sandbox PRIVATE Vulkan::Vulkan)

if(MSVC)
  target_compile_options(vle_sandbox PRIVATE /W4 /permissive- /utf-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(vle_sandbox PRIVATE -Wall -Wextra -Wpedantic)
endif()
