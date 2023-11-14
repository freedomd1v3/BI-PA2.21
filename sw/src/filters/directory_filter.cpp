#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "directory_filter.hpp"
#include "../notes/note.hpp"

void Directory_Filter::request_criteria() {
    std::cout << "Enter a directory by which to search the notes." << std::endl
              << "Please note, that using '.' sign is forbidden." << std::endl
              << '\t';
    std::getline(std::cin, m_Directory_Criteria);
    if (!std::cin.good()) {
        throw std::runtime_error("Directory_Filter::request_criteria(): Couldn't read a directory.");
    }
    else if (m_Directory_Criteria.find('.') != std::string::npos
             || m_Directory_Criteria.front() == '/') {
        throw std::invalid_argument("Directory_Filter::request_criteria(): Provided directory is invalid.");
    }

    if (m_Directory_Criteria.back() != '/') {
        m_Directory_Criteria.push_back('/');
    }
    std::cout << std::endl
              << "\"Reverse\" means that the path should not contain the provided directory." << std::endl;
    Filter::request_criteria();
}

bool Directory_Filter::operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const {
    bool result = check.first.find(m_Directory_Criteria) == 0;
    return m_Reverse ? !result
                     : result;
}
