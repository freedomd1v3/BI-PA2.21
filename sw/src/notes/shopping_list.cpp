#include <string>
#include <iostream>
#include <stdexcept>
#include <ios>
#include <limits>
#include <algorithm>
#include <fstream>
#include <ostream>
#include "note.hpp"
#include "shopping_list.hpp"
#include "../menu.hpp"

Shopping_List::Shopping_List(const std::string & current_date)
    : Note(current_date) { }

void Shopping_List::edit_record() {
    std::cout << "Enter number of record to edit: ";
    size_t record_num;
    if (!(std::cin >> record_num)) {
        throw std::runtime_error("Shopping_List::edit_record(): Couldn't read record number.");
    }
    // Flushing stdin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (!(--record_num < m_List.size())) {
        throw std::invalid_argument("Shopping_List::edit_record(): Invalid record number.");
    }

    std::cout << "Enter new item: ";
    std::string new_item;
    std::getline(std::cin, new_item);
    if (!std::cin.good()) {
        throw std::runtime_error("Shopping_List::edit_record(): Couldn't read a new record text.");
    }
    else if (!new_item.size()) {
        throw std::invalid_argument("Shopping_List::edit_record(): Record can't be empty.");
    }
    // Difference from Note::edit_tag() is that here
    // we can have duplicit items
    m_Changelog.emplace_back(get_timestamp(), "Changed item: " + m_List.at(record_num)
                                             + " to: " + new_item);
    m_List.at(record_num) = new_item;
}

void Shopping_List::delete_record() {
    std::cout << "Enter number of record to remove: ";
    size_t record_num;
    if (!(std::cin >> record_num)) {
        throw std::runtime_error("Shopping_List::delete_record(): Couldn't read record number.");
    }
    // Flushing stdin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (!(--record_num < m_List.size())) {
        throw std::invalid_argument("Shopping_List::edit_record(): Invalid record number.");
    }

    m_Changelog.emplace_back(get_timestamp(), "Removed item: " + m_List.at(record_num));
    // Cast "tag_id" to long int to bypass "-Wsign-conversion"
    m_List.erase(m_List.begin() + record_num);
}

bool Shopping_List::add_record() {
    std::string new_item;
    std::getline(std::cin, new_item);
    if (!std::cin.good()) {
        throw std::runtime_error("Shopping_List::add_record(): Couldn't read new item.");
    }

    if (!new_item.size()) {
        return false;
    }

    m_Changelog.emplace_back(get_timestamp(), "Added item: " + new_item);
    m_List.emplace_back(new_item);
    return true;
}

void Shopping_List::edit() {
    Note::edit();

    if (m_List.size()) {
        std::cout << "Editing already existing shopping items now..." << std::endl
                  << "Type \"delete\" ('d') to delete the record." << std::endl
                  << "Type \"edit\" ('e') to edit the record." << std::endl
                  << "Enter empty line to finish editing items." << std::endl;

        size_t cnt = 1;
        for (const auto & x: m_List) {
            std::cout << '\t' << cnt++ << ". " << x << std::endl;
        }

        for (;;) {
            std::string action;
            std::getline(std::cin, action);
            if (!std::cin.good()) {
                throw std::runtime_error("Shopping_List::edit(): Couldn't read a record editing action.");
            }

            else if (!action.size()) {
                break;
            }
            std::transform(action.begin(), action.end(),
                           action.begin(), ::tolower);
            if (!action.compare("delete") || !action.compare("d")) {
                try {
                    delete_record();
                }
                catch (const std::invalid_argument & e) {
                    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
                    continue;
                }
                std::cout << "Successfully deleted the record." << std::endl << std::endl;
                continue;
            }
            else if (!action.compare("edit") || !action.compare("e")) {
                try {
                    edit_record();
                }
                catch (const std::invalid_argument & e) {
                    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
                    continue;
                }
                std::cout << "Successfully edited the record." << std::endl << std::endl;
                continue;
            }
            std::cerr << "ERROR: Shopping_List::edit(): Invalid record editing action." << std::endl << std::endl;
        }
    }

    std::cout << "Adding new shooping elements..." << std::endl
              << "Do not type anything to finish adding records." << std::endl;
    do {
        std::cout << '\t';
    } while (add_record());
}

void Shopping_List::save(std::ofstream & os) const {
    os << "shopping list" << std::endl << std::endl;
    Note::save(os);
    for (const auto & x: m_List) {
        os << x << std::endl;
    }
}

void Shopping_List::print(std::ostream & os) const {
    os << "Shopping list ";
    if (m_Name.size()) {
        os << '"' << m_Name << "\" ";
    }
    os << "created at: " << m_Changelog.front().first << std::endl;
    size_t cnt = 1;
    for (const auto & x: m_List) {
        os << '\t' << cnt++ << ". " << x << std::endl;
    }

    Note::print(os);
}

void Shopping_List::read(std::ifstream & os) {
    Note::read(os);

    for (;;) {
        std::string item;
        std::getline(os, item);
        if (os.bad()) {
            throw std::runtime_error("Shopping_List::read(): File is damaged.");
        }
        else if (os.eof()) {
            break;
        }

        m_List.push_back(item);
    }
}

std::string Shopping_List::get_summary() const {
    std::string summary = "Shopping List";
    if (m_Name.size()) {
        summary.append(": " + m_Name);
    }
    return summary;
}

bool Shopping_List::contains(const std::string & text) const {
    for (const auto & x: m_List) {
        if (x.find(text) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string Shopping_List::get_content() const {
    std::string to_return = "Shopping list with content:";
    to_return.push_back('\n');
    size_t cnt = 1;
    for (const auto & x: m_List) {
        to_return.push_back('\t');
        to_return.append(std::to_string(cnt++) + ". " + x + '\n');
    }
    return to_return;
}
