#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
#include <memory>
#include "name_filter.hpp"
#include "../notes/note.hpp"

void Name_Filter::request_criteria() {
    std::cout << "Enter a name by which to search the notes:" << std::endl
              << '\t';
    std::getline(std::cin, m_Name_Criteria);
    if (!std::cin.good()) {
        throw std::runtime_error("Name_Filter::request_criteria(): Couldn't read a name.");
    }
    else if (!m_Name_Criteria.size()) {
        throw std::invalid_argument("Name_Filter::request_crteria(): Name can't be empty.");
    }

    std::cout << std::endl
              << "\"Reverse\" means that the note should not contain the provided name." << std::endl;
    Filter::request_criteria();
}

bool Name_Filter::operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const {
    // We don't want a name to be exact, it's enough for the name
    // to contain (or NOT contain, depending on "m_Reverse") the criteria
    return m_Reverse ? check.second->get_name().find(m_Name_Criteria) == std::string::npos
                     : check.second->get_name().find(m_Name_Criteria) != std::string::npos;
}
