#include <string>
#include <iostream>
#include <stdexcept>
#include <ios>
#include <limits>
#include <utility>
#include <algorithm>
#include <fstream>
#include <ostream>
#include "note.hpp"
#include "todo_list.hpp"
#include "../menu.hpp"

TODO_List::TODO_List(const std::string & current_date)
    : Note(current_date) { }

bool TODO_List::check_uniqueness(const std::string & record) {
    for (const auto & x: m_List) {
        if (x.first == record) {
            return false;
        }
    }
    return true;
}

void TODO_List::edit_record() {
    std::cout << "Enter number of record to edit: ";
    size_t record_num;
    if (!(std::cin >> record_num)) {
        throw std::runtime_error("TODO_List::edit_record(): Couldn't read record number.");
    }
    // Flushing stdin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (!(--record_num < m_List.size())) {
        throw std::invalid_argument("TODO_List::edit_record(): Invalid record number.");
    }

    std::cout << "Enter new task: ";
    std::string new_record;
    std::getline(std::cin, new_record);
    if (!std::cin.good()) {
        throw std::runtime_error("TODO_List::edit_record(): Couldn't read a new record text.");
    }
    else if (!new_record.size()) {
        throw std::invalid_argument("TODO_List::edit_record(): Record can't be empty.");
    }
    else if (!check_uniqueness(new_record)) {
        throw std::invalid_argument("TODO_List::edit_record(): Record must be unique.");
    }

    std::cout << "Enter new deadline: ";
    std::string new_deadline;
    std::getline(std::cin, new_deadline);
    if (!std::cin.good()) {
        throw std::runtime_error("TODO_List::edit_record(): Couldn't read a new deadline.");
    }
    else if (!new_record.size()) {
        throw std::invalid_argument("TODO_List::edit_record(): Deadline can't be empty.");
    }
    m_Changelog.emplace_back(get_timestamp(), "Changed task: " + m_List.at(record_num).first
                                              + " with deadline: " + m_List.at(record_num).second
                                              + " to: " + new_record
                                              + " with deadline: " + new_deadline);
    m_List.at(record_num) = std::make_pair(new_record, new_deadline);
}

void TODO_List::delete_record() {
    std::cout << "Enter number of record to delete: ";
    size_t record_num;
    if (!(std::cin >> record_num)) {
        throw std::runtime_error("TODO_List::delete_record(): Couldn't read record number.");
    }
    // Flushing stdin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (!(--record_num << m_List.size())) {
        throw std::invalid_argument("TODO_List::delete_record(): Invalid record number.");
    }

    m_Changelog.emplace_back(get_timestamp(), "Removed task: " + m_List.at(record_num).first
                                              + " with deadline: " + m_List.at(record_num).second);
    // Cast "tag_id" to long int to bypass "-Wsign-conversion"
    m_List.erase(m_List.begin() + record_num);
}

bool TODO_List::add_record() {
    std::cout << "Enter new task name: ";
    std::string record_name;
    std::getline(std::cin, record_name);
    if (!std::cin.good()) {
        throw std::runtime_error("TODO_List::edit(): Couldn't read a name of a new record.");
    }

    if (!record_name.size()) {
        return false;
    }
    else if (!check_uniqueness(record_name)) {
        throw std::invalid_argument("TODO_List::add_new(): Provided record isn't unique.");
    }

    std::cout << "Enter new task deadline: " << std::endl;
    for (;;) {
        std::cout << '\t';
        std::string record_deadline;
        std::getline(std::cin, record_deadline);
        if (!std::cin.good()) {
            throw std::runtime_error("TODO_List::edit(): Couldn't read a deadline of a new record.");
        }

        if (!record_deadline.size()) {
            std::cerr << "ERROR: TODO_List::edit(): Deadline of a record can't be empty." << std::endl;
            continue;
        }

        m_List.emplace_back(record_name, record_deadline);
        m_Changelog.emplace_back(get_timestamp(), "Added new record: "
                                                  + record_name
                                                  + " with deadline: "
                                                  + record_deadline);
        return true;
    }
}

void TODO_List::edit() {
    Note::edit();

    if (m_List.size()) {
        std::cout << "Editing already existing records now..." << std::endl
                  << "Type \"delete\" ('d') to delete the record." << std::endl
                  << "Type \"edit\" ('e') to edit the record." << std::endl
                  << "Enter empty line to finish editing the list." << std::endl;
                  /* << "First enter the name, and then the deadline." << std::endl
                  << "Neither can have empty names." << std::endl
                  << "If you want to delete the record, enter \"delete\" ('d') "
                  << " while editing the name." << std::endl; */

        size_t cnt = 1;
        for (const auto & x: m_List) {
            std::cout << '\t' << cnt++ << ". " << x.first << ": " << x.second << std::endl;
        }

        for (;;) {
            std::string action;
            std::getline(std::cin, action);
            if (!std::cin.good()) {
                throw std::runtime_error("TODO_List::edit(): Couldn't read a record editing action.");
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
            std::cerr << "ERROR: TODO_List::edit(): Invalid record editing action." << std::endl << std::endl;
        }
    }

    std::cout << "Adding new records..." << std::endl
              << "Please enter the name and a deadline of a new record." << std::endl
              << "To stop adding the records, enter empty name. Deadlines can't be empty." << std::endl
              << "Records must also be unique." << std::endl;
    while (add_record()) {
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void TODO_List::save(std::ofstream & os) const {
    os << "to-do list" << std::endl << std::endl;
    Note::save(os);
    for (const auto & x: m_List) {
        os << x.first << std::endl
           << '\t' << x.second << std::endl;
    }
}

void TODO_List::print(std::ostream & os) const {
    os << "To-do list ";
    if (m_Name.size()) {
        os << '"' << m_Name << "\" ";
    }
    os << "created at: " << m_Changelog.front().first << std::endl;
    size_t cnt = 1;
    for (const auto & x: m_List) {
        os << '\t' << cnt++ << ". " << x.first << std::endl
           << "\t\t" << x.second << std::endl;
    }

    Note::print(os);
}

void TODO_List::read(std::ifstream & os) {
    Note::read(os);

    const std::string corrupted = "TODO_List::read(): File is corrupted.",
                      damaged = "TODO_List::read(): File is damaged.";

    for (;;) {
        std::string todo;
        std::getline(os, todo);
        if (os.bad()) {
            throw std::runtime_error(damaged);
        }
        else if (os.eof()) {
            break;
        }
        else if (!check_uniqueness(todo)) {
            // All records must be unique
            throw std::runtime_error(corrupted);
        }

        // Got a new to-do record, now reading the deadline
        std::string deadline;
        // '\t' and some character is a minimum
        const size_t MINIMAL_DEADLINE_SIZE = 2;
        std::getline(os, deadline);
        if (!os.good()) {
            throw std::runtime_error(damaged);
        }
        else if (!(deadline.size() >= MINIMAL_DEADLINE_SIZE)
                 || deadline.front() != '\t') {
            throw std::runtime_error(corrupted);
        }
        // Getting rid of first tab character
        deadline.erase(deadline.begin());

        m_List.emplace_back(todo, deadline);
    }
}

std::string TODO_List::get_summary() const {
    std::string summary = "To-do list";
    if (m_Name.size()) {
        summary.append(": " + m_Name);
    }
    return summary;
}

bool TODO_List::contains(const std::string & text) const {
    for (const auto & x: m_List) {
        if (x.first.find(text) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string TODO_List::get_content() const {
    std::string to_return = "To-do list with content:";
    to_return.append("\n");
    size_t cnt = 1;
    for (const auto & x: m_List) {
        to_return.append('\t' + std::to_string(cnt++) + ". " + x.first + '\n');
        to_return.append("\t\t" + x.second + '\n');
    }
    return to_return;
}
