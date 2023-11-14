#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <filesystem>
#include <cstddef>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "menu.hpp"
#include "note_storage.hpp"
#include "notes/note.hpp"
#include "notes/text.hpp"
#include "notes/shopping_list.hpp"
#include "notes/todo_list.hpp"
#include "filters/filter.hpp"
#include "filters/name_filter.hpp"
#include "filters/creation_date_filter.hpp"
#include "filters/tag_filter.hpp"
#include "filters/directory_filter.hpp"
#include "filters/text_filter.hpp"
#include "exports/export.hpp"
#include "exports/markdown_export.hpp"
#include "heading.hpp"

void Menu::create_note() const {
    std::string current_date = m_Notes_Store.get_file_timestamp();
    std::unique_ptr<Note> new_note;

    // We shouldn't leave creating a note, unless we got an stdin error
    for (;;) {
        std::cout << "Please enter the type of note you'd like to create:" << std::endl
                  << "\t\"Text\" ('T') for a text note;" << std::endl
                  << "\t\"Shopping List\" (\"ST\") for a shopping list;" << std::endl
                  << "\t\"To-do list\" (\"TDL\") for a to-do list;" << std::endl;
        std::string note_type;
        std::getline(std::cin, note_type);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::create_note(): Couldn't read a type of a new note.");
        }

        std::transform(note_type.begin(), note_type.end(),
                       note_type.begin(), ::tolower);

        if (!note_type.compare("text") || !note_type.compare("t")) {
            new_note = std::make_unique<Text>(current_date);
            break;
        }
        else if (!note_type.compare("shopping list") || !note_type.compare("st")) {
            new_note = std::make_unique<Shopping_List>(current_date);
            break;
        }
        else if (!note_type.compare("to-do list") || !note_type.compare("tdl")) {
            new_note = std::make_unique<TODO_List>(current_date);
            break;
        }
        std::cerr << "ERROR: Menu::create_note(): Invalid note type." << std::endl << std::endl;
    }

    std::cout << std::endl;
    new_note->create();
    
    std::string directory;
    for (;;) {
        std::cout << "Please enter, where you want to save the note "
                  << "(empty line to save in a root folder)." << std::endl
                  << "Please notice, that using '.' sign is forbidden." << std::endl
                  << '\t';
        std::getline(std::cin, directory);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::create_note(): Couldn't read directory.");
        }
        else if (directory.find('.') != std::string::npos) {
            std::cerr << "ERROR: Menu::create_note(): Used forbidden character in directory." << std::endl << std::endl;
        }
        else {
            break;
        }
    }

    std::cout << std::endl;
    m_Notes_Store.update(*new_note.get(), directory);
    std::cout << "INFO: Successfully created new note." << std::endl;
}

void Menu::display_note() const {
    if (m_Notes_Store.m_Filtered.size()) {
        std::cout << "There are some notes filtered saved in history." << std::endl
                  << "Do you want to print them? (\"Yes\" / 'Y')" << std::endl
                  << '\t';
        std::string answer;
        std::getline(std::cin, answer);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::display_note(): Couldn't read answer.");
        }

        std::cout << std::endl;
        std::transform(answer.begin(), answer.end(),
                       answer.begin(), ::tolower);
        if (!answer.compare("yes") || !answer.compare("y")) {
            for (const auto & x: m_Notes_Store.m_Filtered) {
                std::cout << x.first << std::endl;
                x.second->print(std::cout);
                std::cout << std::endl << std::endl;
            }
        }
    }

    std::cout << "Please write a path to a note you want to display." << std::endl
              << "If you're not sure, you should use \"List All\" to see all available notes:" << std::endl
              << "Enter empty line to return to the previous screen:" << std::endl
              << '\t';
    std::string path;
    std::getline(std::cin, path);
    if (!std::cin.good()) {
        throw std::runtime_error("Menu::display_note(): Couldn't read path.");
    }
    else if (!path.size()) {
        return;
    }

    std::cout << std::endl;
    // If user tried to enter a path and forgot to add the last '/'
    if (path.back() != '/') {
        path.push_back('/');
    }
    try {
        auto list_to_print = m_Notes_Store.read_recursively(path);
        for (const auto & x: list_to_print) {
            std::cout << x.first << std::endl
                      << x.second->get_summary() << std::endl << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error & e) {
        // Not a dir - trying as a file
        path.pop_back();
        m_Notes_Store.read(path, false)->print(std::cout);
    }
}

void Menu::search_notes() const {
    m_Notes_Store.m_Filtered = m_Notes_Store.read_recursively("");
    std::vector<std::unique_ptr<Filter>> search_params;
    for (;;) {
        std::cout << std::endl
                  << "Please enter the type of filter you want to search notes by:" << std::endl
                  << "\t\"Name\" ('n'), to search by name;" << std::endl
                  << "\t\"Creation Date\" (\"cd\"), to search by creation date;" << std::endl
                  << "\t\"Tag\" (\"tg\"), to search by tag;" << std::endl
                  << "\t\"Directory\" ('d') to search by directory;" << std::endl
                  << "\t\"Text\" (\"tx\"), to search by text contained in the note;" << std::endl
                  << "Enter empty line to finish adding filters." << std::endl
                  << '\t';
        std::string filter_type;
        std::getline(std::cin, filter_type);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::search_notes_note(): Couldn't read a type of a new filter.");
        }
        else if (!filter_type.size()) {
            break;
        }

        std::transform(filter_type.begin(), filter_type.end(),
                       filter_type.begin(), ::tolower);

        if (!filter_type.compare("name") || !filter_type.compare("n")) {
            search_params.emplace_back(new Name_Filter);
        }
        else if (!filter_type.compare("creation date") || !filter_type.compare("cd")) {
            search_params.emplace_back(new Creation_Date_Filter);
        }
        else if (!filter_type.compare("tag") || !filter_type.compare("tg")) {
            search_params.emplace_back(new Tag_Filter);
        }
        else if (!filter_type.compare("directory") || !filter_type.compare("d")) {
            search_params.emplace_back(new Directory_Filter);
        }
        else if (!filter_type.compare("text") || !filter_type.compare("tx")) {
            search_params.emplace_back(new Text_Filter);
        }
        else {
            std::cerr << "ERROR: Menu::search_notes(): Invalid filter type." << std::endl << std::endl;
            continue;
        }

        for (;;) {
            std::cout << std::endl;
            try {
                search_params.back()->request_criteria();
                break;
            }
            catch (const std::invalid_argument & e) {
                std::cerr << "ERROR: " << e.what() << std::endl;
            }
        }
    }

    for (const auto & x: search_params) {
        for (size_t i = 0; i < m_Notes_Store.m_Filtered.size(); i++) {
            if (!(*x)(m_Notes_Store.m_Filtered.at(i))) {
                // Cast i to long int to bypass "-Wsign-conversion"
                m_Notes_Store.m_Filtered.erase(m_Notes_Store.m_Filtered.begin() + i);
                i--;    // We don't want to increase i here
            }
        }
    }
    std::cout << std::endl
              << "INFO: Notes were successfully filtered." << std::endl
              << "INFO: You can now either list (display) or export them." << std::endl;
}

void Menu::list_all() const {
    auto list_to_print = m_Notes_Store.read_recursively("");
    for (const auto & x: list_to_print) {
        std::cout << x.first << std::endl
                  << x.second->get_summary() << std::endl << std::endl;
    }
}

void Menu::edit_note() const {
    std::cout << "Please write a path to a note you want to edit." << std::endl
              << "If you're not sure, you should use \"List All\" to see all available notes." << std::endl
              << "Enter empty line to return to the previous screen:" << std::endl
              << '\t';
    std::string path;
    std::getline(std::cin, path);
    if (!std::cin.good()) {
        throw std::runtime_error("Menu::edit_note(): Couldn't read path.");
    }
    else if (!path.size()) {
        return;
    }

    std::cout << std::endl;
    std::unique_ptr<Note> to_edit = m_Notes_Store.read(path, false);
    to_edit->edit();
    // Getting the directory of a file's path
    size_t remove_after_last_slash = path.find_last_of('/');
    if (remove_after_last_slash != std::string::npos) {
        path.erase(remove_after_last_slash);
    }
    else {
        path.clear();
    }
    m_Notes_Store.update(*to_edit.get(), path);
    std::cout << "INFO: Successfully saved file with edited note." << std::endl;
}

void Menu::delete_note() const {
    std::cout << "Please write a path to a note (or a directory of notes) you want to delete." << std::endl
              << "If you're not sure, you should use \"List All\" to see all available notes." << std::endl
              << "Enter empty line to return to the previous screen:" << std::endl
              << '\t';
    std::string path;
    std::getline(std::cin, path);
    if (!std::cin.good()) {
        throw std::runtime_error("Menu::delete_note(): Couldn't read path.");
    }
    else if (!path.size()) {
        return;
    }

    std::cout << std::endl;
    try {
        m_Notes_Store.delete_note(path);
    }
    catch (const std::runtime_error & e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return;
    }
    std::cout << "INFO: Successfully deleted a file or directory at the provided path." << std::endl;
}

void Menu::export_notes() const {
    if (m_Notes_Store.m_Filtered.size()) {
        std::cout << "There are some notes filtered saved in history." << std::endl
                  << "Do you want to export them? (\"Yes\" / 'Y')" << std::endl
                  << '\t';
        std::string answer;
        std::getline(std::cin, answer);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::export_notes(): Couldn't read answer.");
        }

        std::transform(answer.begin(), answer.end(),
                       answer.begin(), ::tolower);
        if (!answer.compare("yes") || !answer.compare("y")) {
            for (const auto & x: m_Notes_Store.m_Filtered) {
                std::cout << std::endl
                          << x.first << std::endl
                          << x.second->get_summary() << std::endl
                          << "\tWould you like to export this note? (\"Yes\" / 'Y')" << std::endl
                          << '\t';
                std::getline(std::cin, answer);
                if (!std::cin.good()) {
                    throw std::runtime_error("Menu::export_notes(): Couldn't read answer.");
                }

                std::cout << std::endl;
                std::transform(answer.begin(), answer.end(),
                               answer.begin(), ::tolower);
                if (!answer.compare("yes") || !answer.compare("y")) {
                    export_note(x.second);
                }
            }
            std::cout << std::endl;
        }
    }

    std::cout << "Please write a path to a note you want to export.." << std::endl
              << "If you're not sure, you should use \"List All\" to see all available notes:" << std::endl
              << "Enter empty line to return to the previous screen:" << std::endl
              << '\t';
    std::string path;
    std::getline(std::cin, path);
    if (!std::cin.good()) {
        throw std::runtime_error("Menu::export_notes(): Couldn't read path to a note.");
    }
    else if (!path.size()) {
        return;
    }

    std::cout << std::endl;
    if (path.back() == '/') {
        // Prevent the user from accidentally adding '/' to the note's path
        path.pop_back();
    }
    std::unique_ptr<Note> to_export;
    try {
        to_export = m_Notes_Store.read(path, false);
    }
    catch (const std::runtime_error & e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return;
    }

    export_note(to_export);
}

void Menu::export_note(const std::unique_ptr<Note> & to_export) const {
    std::string path;
    for (;;) {
        std::cout << "Enter an absolute path where you want to export the note." << std::endl
                  << '\t';
        std::getline(std::cin, path);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::export_note(): Couldn't read path where to export the note to.");
        }
        else if (path.front() != '/') {
            std::cerr << "ERROR: Menu::export_note(): Path must be absolute." << std::endl;
        }
        else {
            break;
        }
    }
    
    std::cout << std::endl
              << "Do you want to export note to some standard file format? (\"Yes\" / 'Y')" << std::endl
              << '\t';
    std::string answer;
    std::getline(std::cin, answer);
    if (!std::cin.good()) {
        throw std::runtime_error("Menu::export_note(): Couldn't read answer.");
    }

    std::cout << std::endl;
    if (!answer.compare("yes") || !answer.compare("y")) {
        std::unique_ptr<Export> file_format;
        for (;;) {
            std::cout << "Enter a format you want to export the note to:" << std::endl
                      << "\t\"Markdown\" ('M') for Markdown;" << std::endl
                      << '\t';
            std::getline(std::cin, answer);
            if (!std::cin.good()) {
                throw std::runtime_error("Menu::export_note(): Couldn't read answer.");
            }

            std::transform(answer.begin(), answer.end(),
                           answer.begin(), ::tolower);
            if (!answer.compare("markdown") || !answer.compare("m")) {
                file_format = std::make_unique<Markdown_Export>(path);
            }
            else {
                std::cerr << "ERROR: Menu::export_note(): Invalid file format." << std::endl << std::endl;
                continue;
            }
            break;
        }

        std::cout << std::endl;
        try {
            m_Notes_Store.export_note_standard_format(file_format, to_export);
            std::cout << "INFO: Successfully exported the note." << std::endl;
        }
        catch (const std::runtime_error & e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }
    else {
        try {
            m_Notes_Store.export_note(path, to_export);
            std::cout << "INFO: Successfully exported the note." << std::endl;
        }
        catch (const std::runtime_error & e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }

}

void Menu::import_note() const {
    std::cout << "Enter the path to a file with a note you want to import.";
    std::string path;
    for (;;) {
        std::cout << std::endl
                  << '\t';
        std::getline(std::cin, path);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::import_note(): Couldn't read path.");
        }
        else if (path.front() != '/') {
            std::cerr << "ERROR: Menu::import_note(): Path must be absolute." << std::endl;
            continue;
        }
        break;
    }

    std::cout << std::endl;
    std::unique_ptr<Note> to_import;
    try {
        to_import = m_Notes_Store.read(path, true);
    }
    catch (const std::runtime_error & e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return;
    }

    for (;;) {
        std::cout << "Please enter, where you want to save the note "
                  << "(empty line to save in a root folder)." << std::endl
                  << "Please notice, that using '.' sign is forbidden." << std::endl
                  << '\t';
        std::getline(std::cin, path);
        if (!std::cin.good()) {
            throw std::runtime_error("Menu::export_note(): Couldn't read directory.");
        }
        else if (path.find('.') != std::string::npos) {
            std::cerr << "ERROR: Menu::export_note(): Used forbidden character in directory." << std::endl << std::endl;
        }
        else {
            break;
        }
    }
    std::cout << std::endl;
    m_Notes_Store.update(*to_import.get(), path);
    std::cout << "INFO: Successfully imported a note." << std::endl;
}

Menu::Menu(Note_Storage & notes_store)
    : m_Notes_Store(notes_store) { }

void Menu::print_heading() const {
    Heading().print();
}

Menu::User_Choice Menu::ask_user() const {
    std::cout << "Please enter what you'd like to do:" << std::endl
              << "\t\"Create\" ('C') to create a new note;" << std::endl
              << "\t\"Display\" (\"DP\") to display information about note by it's filename;" << std::endl
              << "\t\"Search\" ('S') to search for a note with some filters;" << std::endl
              << "\t\"List All\" ('LA') to get brief description of all existing notes;" << std::endl
              << "\t\"Edit\" (\"ED\") to edit an existing note;" << std::endl
              << "\t\"Delete\" (\"DD\") to delete an existing note;" << std::endl
              << "\t\"Export\" (\"EX\") to export an existing note to a file;" << std::endl
              << "\t\"Import\" ('I') to import a note from a file;" << std::endl
              << "\t\"Quit\" ('Q') to close application." << std::endl;

    std::string action;
    std::getline(std::cin, action);
    if (!std::cin.good()) {
        throw std::runtime_error("Menu::ask_user(): Couldn't read an action.");
    }
    std::cout << std::endl;

    std::transform(action.begin(), action.end(),
                   action.begin(), ::tolower);
    if (!action.compare("create") || !action.compare("c")) {
        return User_Choice::CREATE;
    }
    else if (!action.compare("display") || !action.compare("dp")) {
        return User_Choice::DISP;
    }
    else if (!action.compare("search") || !action.compare("s")) {
        return User_Choice::SEARCH;
    }
    else if (!action.compare("list all") || !action.compare("la")) {
        return User_Choice::LIST_ALL;
    }
    else if (!action.compare("edit") || !action.compare("ed")) {
        return User_Choice::EDIT;
    }
    else if (!action.compare("delete") || !action.compare("dd")) {
        return User_Choice::DELETE;
    }
    else if (!action.compare("export") || !action.compare("ex")) {
        return User_Choice::EXPORT;
    }
    else if (!action.compare("import") || !action.compare("i")) {
        return User_Choice::IMPORT;
    }
    else if (!action.compare("quit") || !action.compare("q")) {
        return User_Choice::EXIT;
    }
    else {
        throw std::invalid_argument("Menu::ask_user(): User entered invalid action.");
    }
}

void Menu::add_distance() const {
    for (size_t i = 0; i < m_DIST; i++) {
        std::cout << std::endl;
    }
}

bool Menu::process_request(const User_Choice & requested_action) {
    switch (requested_action) {
        case User_Choice::CREATE:
            create_note();
            break;
        case User_Choice::DISP:
            try {
                display_note();
            }
            catch (const std::runtime_error & e) {
                std::cerr << "ERROR: " << e.what() << std::endl;
            }
            break;
        case User_Choice::SEARCH:
            search_notes();
            break;
        case User_Choice::LIST_ALL:
            list_all();
            break;
        case User_Choice::EDIT:
            edit_note();
            break;
        case User_Choice::DELETE:
            delete_note();
            break;
        case User_Choice::EXPORT:
            export_notes();
            break;
        case User_Choice::IMPORT:
            import_note();
            break;
        case User_Choice::EXIT:
            return false;
    }

    return true;
}

// ---------------------------------------------------------------------------

const std::string get_timestamp() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    // Convert time to local time
    std::tm localTime = *std::localtime(&time);

    // Format time into a string
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y-%m-%d, %H:%M:%S");
    return ss.str();
}
