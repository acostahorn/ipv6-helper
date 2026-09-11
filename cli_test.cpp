#include "ipv6Parser.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string input;
    if (argc > 1) {
        input = argv[1];
    } else {
        std::cout << "Enter IPv6 string: ";
        std::cin >> input;
    }

    auto result = IPv6Parser::parseShorthand(input);

    if (result.has_value()) {
        std::cout << "\nSuccessfully parsed!" << std::endl;
    } else {
        std::cout << "\nFailed to parse IPv6 address." << std::endl;
    }
    return 0;
}
