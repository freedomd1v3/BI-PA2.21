#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "text_filter.hpp"
#include "../notes/note.hpp"

void Text_Filter::request_criteria() {
    std::cout << "Enter a contained text by which to search the notes:" << std::endl
              << '\t';
    std::getline(std::cin, m_Text_Criteria);
    if (!std::cin.good()) {
        throw std::runtime_error("Text_Filter::request_criteria(): Couldn't read a contained text.");
    }
    else if (!m_Text_Criteria.size()) {
        throw std::invalid_argument("Text_Filter::request_criteria(): Contained text can't be empty.");
    }

    std::cout << std::endl
              << "\"Reverse\" means that the note should NOT contain the provided text." << std::endl;
    Filter::request_criteria();
}

bool Text_Filter::operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const {
    return m_Reverse ? !check.second->contains(m_Text_Criteria)
                     : check.second->contains(m_Text_Criteria);
}
