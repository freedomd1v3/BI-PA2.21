#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "tag_filter.hpp"
#include "../notes/note.hpp"

void Tag_Filter::request_criteria() {
    std::cout << "Enter a tag by which to search the notes:" << std::endl
              << '\t';
    std::getline(std::cin, m_Tag_Criteria);
    if (!std::cin.good()) {
        throw std::runtime_error("Tag_Filter::request_criteria(): Couldn't read a tag.");
    }
    else if (!m_Tag_Criteria.size()) {
        throw std::invalid_argument("Tag_Filter::request_criteria(): Tag can't be empty.");
    }

    std::cout << std::endl
              << "\"Reverse\" means that the note should not contain the provided tag." <<std::endl;
    Filter::request_criteria();
}

bool Tag_Filter::operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const {
    bool result = std::find(check.second->get_tags().begin(), check.second->get_tags().end(),
                            m_Tag_Criteria) != check.second->get_tags().end();
    return m_Reverse ? !result
                     : result;
}
