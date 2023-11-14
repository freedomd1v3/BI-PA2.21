#ifndef NOTE_STORAGE_HPP
#define NOTE_STORAGE_HPP

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "notes/note.hpp"
#include "exports/export.hpp"

/**
 * A class to store the notes and work with their files.
 */
class Note_Storage {
    private:
        // A relative directory where to insert notes in a folder
        // where is the projects' binary file located.
        const std::string m_NOTES_PATH = "examples/";

    public:
        // This should not be private, as it will be accessed by Menu
        // and possibly by other objects.
        std::vector<std::pair<std::string, std::unique_ptr<Note>>> m_Filtered;

        /**
         * Get a timestamp suitable for file saving (YYYY_MM_DD__HH_MM_SS).
         * Thanks for ChatGPT.
         *
         * @return Timestamp for file saving.
         */
        const std::string get_file_timestamp() const;

        /**
         * Save a file containing note. If doesn't exist, create new one.
         *
         * Try to create a file containing a "to_insert" note in
         * a directory "dir", relative to "m_NOTES_PATH".
         * If failed, throws an exception std::runtime_error.
         *
         * @param to_insert A note to insert;
         *        dir       A relative to "m_NOTES_PATH" directory to insert a note.
         */
        void update(const Note & to_insert, std::string & dir);

        /**
         * A method, that reads all notes in a directory,
         * including sub-directories.
         *
         * @param  dir A root folder where to start reading notes.
         * @return A std::vector of pairs of successfully read notes:
         *         first element is a note's path, relative to "m_NOTES_PATH";
         *         second element is a pointer to a note itself.
         */
        std::vector<std::pair<std::string, std::unique_ptr<Note>>> read_recursively(const std::string & dir) const;

        /**
         * Read a note from storage.
         *
         * Reads a file at path "path", allocates memory for it and returns
         * a note.
         *
         * @param  path     A path where a note is. If to_import == true,
         *                  a path is relative to "m_NOTES_PATH",
         *                  otherwise it's absolute.
         * @param to_import Whether or not we should import the note
         *                  from external source.
         * @return A note.
         */
        std::unique_ptr<Note> read(std::string path,
                                   const bool to_import) const;

        /**
         * Check if a folder exists in a path, relative to "m_NOTES_PATH".
         *
         * @param  path A path, relative to "m_NOTES_PATH" to check,
         *              if exists as a directory.
         * @return true, if exists;
         *      false otherwise.
         */
        bool dir_exists(const std::string & path) const;

        /**
         * Delete a note.
         *
         * Throws std::runtime_error if couldn't delete one.
         *
         * @param path A relative to "m_NOTES_PATH" path where a note is.
         */
        void delete_note(const std::string & path);

        /**
         * Export a note.
         *
         * Throws std::runtime_error if got error.
         *
         * @param path An absolute path where to export the note.
         * @param note A note to export
         */
        void export_note(const std::string & path,
                         const std::unique_ptr<Note> & note) const;

        /**
         * Export the note to some standard format.
         *
         * Throws std::runtime_error if got error.
         *
         * @param export_format A file format to export the note to.
         * @param to_export     A note to export.
         */
        void export_note_standard_format(const std::unique_ptr<Export> & export_method, const std::unique_ptr<Note> & to_export);
};

#endif  // NOTE_STORAGE_HPP
