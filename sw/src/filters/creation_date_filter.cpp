#include <sstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "creation_date_filter.hpp"
#include "../notes/note.hpp"

bool Creation_Date_Filter::check_creation_date_validity() {
    std::istringstream iss (m_Creation_Date_Criteria);
    char dash;
    int year;
    iss >> year >> dash;
    if (year < 0 || dash != '-') {
        return false;
    }
    int month;
    iss >> month >> dash;
    if (month < 0 || month > 12 || dash != '-') {
        return false;
    }
    int day;
    iss >> day;
    // That's enough.
    if (day < 0 || day > 31) {
        return false;
    }
    return true;
}

void Creation_Date_Filter::request_criteria() {
    std::cout << "Enter a creation date in format YYYY-MM-DD" << std::endl
              << "by which to search the notes:" << std::endl
              << '\t';
    std::getline(std::cin, m_Creation_Date_Criteria);
    if (!std::cin.good()) {
        throw std::runtime_error("Creation_Date_Filter::request_criteria(): Couldn't read a creation date.");
    }
    else if (!check_creation_date_validity()) {
        throw std::invalid_argument("Creation_Date_Filter::request_criteria(): Creation date isn't valid.");
    }

    std::cout << std::endl
              << "\"Reverse\" means that the note should be created EARLIER" << std::endl
              << "that the provided creation date." << std::endl;
    Filter::request_criteria();
}

bool Creation_Date_Filter::operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const {
    return m_Reverse ? check.second->get_creation_date().compare(m_Creation_Date_Criteria) < 0
                     : check.second->get_creation_date().compare(m_Creation_Date_Criteria) > 0;
}
