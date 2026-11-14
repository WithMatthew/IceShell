#include <vle/engine_info.hpp>

#include <iostream>

int main() {

    const auto engine_name = vle::engine_name();
    std::cout << engine_name << std::endl;
    return 0;
}
