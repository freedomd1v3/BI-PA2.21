#include <string>
#include <cstddef>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <utility>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "note_storage.hpp"
#include "notes/note.hpp"
#include "notes/text.hpp"
#include "notes/shopping_list.hpp"
#include "notes/todo_list.hpp"
#include "exports/export.hpp"

const std::string Note_Storage::get_file_timestamp() const {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    // Convert time to local time
    std::tm localTime = *std::localtime(&time);

    // Format time into a string
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y_%m_%d__%H_%M_%S");
    return ss.str();
}

void Note_Storage::update(const Note & to_insert, std::string & dir) {
    namespace fs = std::filesystem;
    if (!fs::exists(m_NOTES_PATH + dir)
        && !fs::create_directories(m_NOTES_PATH + dir)) {
        throw std::runtime_error("Note_Storage::update(): Couldn't create a directory.");
    }

    // Trying to create 2 notes with the same name in less than a second
    // will result in overwriting of the existing file
    std::ofstream note_file;
    if (dir.size() && dir.back() != '/') {
        dir.push_back('/');
    }
    note_file.open(m_NOTES_PATH + dir + to_insert.get_file_name(), std::ios::trunc);
    if (!note_file.is_open()) {
        std::cout << m_NOTES_PATH + dir + to_insert.get_file_name() << std::endl;
        throw std::runtime_error("Note_Storage::update(): Couldn't create a note file.");
    }
    to_insert.save(note_file);
    note_file.close();
    if (!note_file.good()) {
        throw std::runtime_error("Note_Storage::update(): Write error.");
    }
}

std::vector<std::pair<std::string, std::unique_ptr<Note>>>
Note_Storage::read_recursively(const std::string & dir) const {
    // Perhaps move this to the global section?
    namespace fs = std::filesystem;

    std::vector<std::pair<std::string, std::unique_ptr<Note>>> to_return;
    for (const auto & entry: fs::recursive_directory_iterator(m_NOTES_PATH + dir)) {
        if (fs::is_regular_file(entry.path())) {
            // Erasing "m_NOTES_PATH" from the path for output
            std::string file_relative_path = entry.path();
            file_relative_path.erase(0, m_NOTES_PATH.size());
            std::unique_ptr<Note> file;
            try {
                file = read(file_relative_path, false);
            }
            catch (const std::runtime_error & e) {
                std::cerr << file_relative_path << std::endl
                          << "\tERROR: " << e.what() << std::endl << std::endl;
                continue;
            }

            // Thanks to Stack Overflow for tip about std::move()
            to_return.emplace_back(file_relative_path, std::move(file));
        }
    }
    return to_return;
}

std::unique_ptr<Note> Note_Storage::read(std::string path,
                                         const bool to_import) const {
    const std::string corrupted = "Note_Storage::read(): File is corrupted.",
                      damaged = "Note_Storage::read(): File is damaged.";

    if (!to_import) {
        // A path is relative to "m_NOTES_PATH"
        path.insert(0, m_NOTES_PATH);
    }
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Note_Storage::read(): Couldn't open file.");
    }

    std::string type, skip, creation_timestamp;
    std::getline(file, type);
    if (!file.good()) {
        throw std::runtime_error(damaged);
    }
    std::getline(file, skip);
    if (!file.good()) {
        throw std::runtime_error(damaged);
    }
    if (skip.size()) {
        throw std::runtime_error(corrupted);
    }
    std::getline(file, creation_timestamp);
    if (!file.good()) {
        throw std::runtime_error(damaged);
    }
    std::getline(file, skip);
    if (skip.size()) {
        throw std::runtime_error(corrupted);
    }
    if (!file.good()) {
        throw std::runtime_error(damaged);
    }

    std::unique_ptr<Note> note_read;
    if (!type.compare("text")) {
        note_read = std::make_unique<Text>(creation_timestamp);
    }
    else if (!type.compare("shopping list")) {
        note_read = std::make_unique<Shopping_List>(creation_timestamp);
    }
    else if (!type.compare("to-do list")) {
        note_read = std::make_unique<TODO_List>(creation_timestamp);
    }
    else {
        throw std::runtime_error(corrupted);
    }

    note_read->read(file);
    return note_read;
}

bool Note_Storage::dir_exists(const std::string & path) const {
    namespace fs = std::filesystem;

    return fs::is_directory(m_NOTES_PATH + path);
}

void Note_Storage::delete_note(const std::string & path) {
    namespace fs = std::filesystem;

    if (!fs::remove_all(m_NOTES_PATH + path)) {
        throw std::runtime_error("Note_Storage::delete_note(): Couldn't delete a note or directory.");
    }
    // Removing note from filtered history.
    for (size_t i = 0; i < m_Filtered.size(); i++) {
        if (m_Filtered.at(i).first == path) {
            // Cast i to long int to bypass "-Wsign-conversion"
            m_Filtered.erase(m_Filtered.begin() + i);
            break;
        }
    }
}

void Note_Storage::export_note(const std::string & path,
                               const std::unique_ptr<Note> & note) const {
    namespace fs = std::filesystem;

    if (fs::exists(path)) {
        throw std::runtime_error("Note_Storage::export_note(): Something already exists at provided path.");
    }
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Note_Storage::export_note(): Couldn't create a file to export the note.");
    }
    note->save(file);
    file.close();
    if (!file.good()) {
        throw std::runtime_error("Note_Storage::export_note(): File writing error.");
    }
}

void Note_Storage::export_note_standard_format(const std::unique_ptr<Export> & export_method,
                                               const std::unique_ptr<Note> & to_export) {
    namespace fs = std::filesystem;

    if (fs::exists(export_method->m_PATH)) {
        throw std::runtime_error("Note_Storage::export_note(): Something already exists at provided path.");
    }
    std::ofstream file(export_method->m_PATH);
    if (!file.is_open()) {
        throw std::runtime_error("Note_Storage::export_note(): Couldn't create a file to export the note.");
    }
    (*export_method)(to_export, file);
    file.close();
    if (!file.good()) {
        throw std::runtime_error("Note_Storage::export_note(): File writing error.");
    }
}
