#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

int main() {
    std::cout << "Testing JSON" << std::endl;
    json j;
    j["pi"] = 3.141;
    std::cout << j["pi"] << std::endl;
    return 0;
}

