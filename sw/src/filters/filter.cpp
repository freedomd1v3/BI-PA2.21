#include <iostream>
#include <string>
#include "filter.hpp"

void Filter::request_criteria() {
    std::cout << "Enter \"yes\" ('y'), if the search should be direct," << std::endl
              << "it will otherwise be reverse." << std::endl
              << '\t';
    std::string answer;
    std::getline(std::cin, answer);
    if (!std::cin.good()) {
        throw std::runtime_error("Filter::request_criteria): Couldn't read an answer.");
    }

    if (!answer.compare("yes") || !answer.compare("y")) {
        m_Reverse = false;
    }
}
