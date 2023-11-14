#include <string>
#include <iostream>
#include <stdexcept>
#include <ios>
#include <limits>
#include <algorithm>
#include <fstream>
#include <vector>
#include "note.hpp"
#include "../menu.hpp"

Note::Note(const std::string & current_date)
    : m_CREATION_TIMESTAMP(current_date) { }

void Note::set_name(const std::string & new_name) {
    m_Changelog.emplace_back(get_timestamp(), "Changed name: " + new_name);
    m_Name = new_name;
}

void Note::edit_tag() {
    std::cout << "Enter ID of a tag to edit: ";
    size_t tag_id;
    if (!(std::cin >> tag_id)) {
        throw std::runtime_error("Note::edit_tag(): Couldn't read note ID.");
    }
    // Flushing stdin (thanks to GeeksForGeeks)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (!(--tag_id < m_Tags.size())) {
        throw std::invalid_argument("Note::edit_tag(): Invalid note ID.");
    }

    std::cout << "Enter new tag text: ";
    std::string new_tag;
    std::getline(std::cin, new_tag);
    if (!std::cin.good()) {
        throw std::runtime_error("Note::edit_tag(): Couldn't read a new tag text.");
    }
    else if (!new_tag.size()) {
        throw std::invalid_argument("Note::edit_tag(): Tag can't be empty.");
    }
    else if (std::find(m_Tags.begin(), m_Tags.end(), new_tag) != m_Tags.end()) {
        throw std::invalid_argument("Note::edit_tag(): Tag already exists.");
    }
    m_Changelog.emplace_back(get_timestamp(), "Changed tag: " + m_Tags.at(tag_id)
                                              + " to: " + new_tag);
    m_Tags.at(tag_id) = new_tag;
}

void Note::delete_tag() {
    std::cout << "Enter ID of a tag to remove: ";
    size_t tag_id;
    if (!(std::cin >> tag_id)) {
        throw std::runtime_error("Note::delete_tag(): Couldn't read note ID.");
    }
    // Flushing stdin (thanks to GeeksForGeeks)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (!(--tag_id < m_Tags.size())) {
        throw std::invalid_argument("Note::delete_tag(): Invalid note ID.");
    }

    m_Changelog.emplace_back(get_timestamp(), "Removed tag: " + m_Tags.at(tag_id));
    // Cast "tag_id" to long int to bypass "-Wsign-conversion"
    m_Tags.erase(m_Tags.begin() + tag_id);
}

bool Note::add_tag() {
    std::string new_tag;
    std::getline(std::cin, new_tag);
    if (!std::cin.good()) {
        throw std::runtime_error("Note::edit(): Couldn't read a new tag.");
    }
    else if (!new_tag.size()) {
        return false;
    }

    m_Tags.push_back(new_tag);
    m_Changelog.emplace_back(get_timestamp(), "Added tag: " + new_tag);
    return true;
}

void Note::edit() {
    std::cout << "Please enter a new name of the note." << std::endl
              << "Enter empty line not to change the name or to leave it empty: ";
    std::string new_name;
    std::getline(std::cin, new_name);
    if (!std::cin.good()) {
        throw std::runtime_error("Note::edit(): Couldn't read a new name of the note.");
    }
    if (new_name.size()) {
        set_name(new_name);
    }

    std::cout << std::endl;
    if (m_Tags.size()) {
        std::cout << "Enter \"edit\" ('e') to change a tag," << std::endl
                  << "\"delete\" ('d') to delete it," << std::endl
                  << "or nothing to finish editing tags." << std::endl;

        size_t cnt = 1;
        for (const auto & x: m_Tags) {
            std::cout << '\t' << cnt++ << ". " << x << std::endl;
        }

        for (;;) {
            std::string action;
            std::getline(std::cin, action);
            if (!std::cin.good()) {
                throw std::runtime_error("Note::edit(): Couldn't read a tag action.");
            }

            else if (!action.size()) {
                break;
            }
            std::transform(action.begin(), action.end(),
                           action.begin(), ::tolower);
            if (!action.compare("edit") || !action.compare("e")) {
                try {
                    edit_tag();
                }
                catch (const std::invalid_argument & e) {
                    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
                    continue;
                }
                std::cout << "Succcessfully edited the tag." << std::endl << std::endl;
                continue;
            }
            else if (!action.compare("delete") || !action.compare("d")) {
                try {
                    delete_tag();
                }
                catch (const std::invalid_argument & e) {
                    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
                    continue;
                }
                std::cout << "Successfully deleted the tag." << std::endl << std::endl;
                continue;
            }
            std::cerr << "ERROR: Note::edit(): Invalid tag action." << std::endl << std::endl;
        }
    }

    std::cout << "Adding tags now..." << std::endl;
    std::cout << "Enter empty string to stop adding tags." << std::endl;
    do {
        std::cout << '\t';
    } while (add_tag());
}

void Note::create() {
    m_Changelog.emplace_back(get_timestamp(), "Created note.");
    edit();
}

const std::string & Note::get_file_name() const {
    return m_CREATION_TIMESTAMP;
}

void Note::save(std::ofstream & os) const {
    os << m_CREATION_TIMESTAMP << std::endl << std::endl
       << m_Name << std::endl << std::endl;
    for (const auto & x: m_Tags) {
        os << x << std::endl;
    }
    os << std::endl;
    for (const auto & x: m_Changelog) {
        os << x.first << std::endl
           << '\t' << x.second << std::endl;
    }
    // We have a base class here, file isn't complete yet
    os << std::endl << std::endl;
}

void Note::print(std::ostream & os) const {
    os << std::endl
       << "Tags:" << std::endl;
    for (const auto & x: m_Tags) {
        os << '\t' << x
           << std::endl;
    }

    os << std::endl
       << "Changelog:";
    for (const auto & x: m_Changelog) {
        os << std::endl
           << '\t' << x.first << " - " << x.second;
    }
    os << std::endl;
}

void Note::read(std::ifstream & os) {
    const std::string corrupted = "Note::read(): File is corrupted.",
                      damaged = "Note::read(): File is damaged.";

    std::string skip;
    std::getline(os, m_Name);
    if (!os.good()) {
        throw std::runtime_error(damaged);
    }
    std::getline(os, skip);
    if (!os.good()) {
        throw std::runtime_error(damaged);
    }
    if (skip.size()) {
        throw std::runtime_error(corrupted);
    }

    size_t cnt = 0;
    // Reading tags
    for (;;) {
        std::getline(os, skip);
        if (!os.good()) {
            throw std::runtime_error(damaged);
        }
        else if (!skip.size()) {
            break;
        }
        m_Tags.push_back(skip);
        cnt++;
    }

    cnt = 0;
    // Reading changelog (there must be at least 1 record)
    for (;;) {
        std::getline(os, skip);
        if (!os.good()) {
            throw std::runtime_error(damaged);
        }
        else if (!skip.size()) {
            break;
        }
        // Got timestamp, getting change itself now
        std::string change;
        std::getline(os, change);
        // Tab and some character are the very minimum of a valid change
        const size_t MINIMAL_CHANGE_SIZE = 2;
        if (!(change.size() >= MINIMAL_CHANGE_SIZE) || change.front() != '\t') {
            throw std::runtime_error(corrupted);
        }
        change.erase(change.begin());
        m_Changelog.emplace_back(skip, change);
        cnt++;
    }
    if (!cnt) {
        throw std::runtime_error(corrupted);
    }
    std::getline(os, skip);
    if (!os.good()) {
        throw std::runtime_error(damaged);
    }
    else if (skip.size()) {
        throw std::runtime_error(corrupted);
    }

    if (!os.good()) {
        throw std::runtime_error(damaged);
    }
}

const std::string & Note::get_name() const {
    return m_Name;
}

const std::string & Note::get_creation_date() const {
    return m_Changelog.front().first;
}

const std::vector<std::string> & Note::get_tags() const {
    return m_Tags;
}
