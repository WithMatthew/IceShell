#include <vle/core/engine_info.hpp>

#include <iostream>

int main()
{
    std::cout << vle::engine_name() << '\n'
              << "Lesson 00: build and link succeeded.\n"
              << "Next: Lesson 01 - window and event loop.\n";
    return 0;
}
