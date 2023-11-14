#ifndef MENU_HPP
#define MENU_HPP

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>
#include <string>
#include "note_storage.hpp"
#include "notes/note.hpp"

/**
 * A basic text menu implementation for interaction with user in terminal.
 */
struct Menu {
    private:
        const size_t m_DIST = 5;

        Note_Storage & m_Notes_Store;

        /**
         * Asks user for the type of note they want to create and create it.
         *
         * Throws std::runtime_error if got stdin error.
         */
        void create_note() const;

        /**
         * Asks the user for the type of note they want to display.
         *
         * If there are some notes filtered in "m_Filtered",
         * asks the user whether or not to display them.
         * Throws std::runtime_error if got stdin error.
         */
        void display_note() const;

        /**
         * Search for the notes by using filters defined in filters/.
         *
         * Resets "m_Filtered" in "m_Notes_Store" and fills it with new results.
         * Filter type reading is case insensitive.
         */
        void search_notes() const;

        /**
         * Prints brief summary of all available notes.
         */
        void list_all() const;

        /**
         * Asks the user which note to edit and how to do it.
         *
         * Throws std::runtime_error if got stdin error.
         */
        void edit_note() const;

        /**
         * Asks the user which note to delete.
         *
         * Throws std::runtime_error if got stdin error.
         */
        void delete_note() const;

        /**
         * Asks the user which note to export,
         *
         * If there are some notes filtered in "m_Filtered",
         * asks the user whether or not to export them.
         * Throws std::runtime_error if got stdin error.
         */
        void export_notes() const;

        /**
         * Asks the user where to export the note, possibly to some standard
         * file format.
         *
         * Throws std::runtime_error if got stdin error.
         * Reading standard file format is case insensitive.
         *
         * @param to_export A note to export.
         */
        void export_note(const std::unique_ptr<Note> & to_export) const;

        /**
         * Asks the user for a path to file with a note to import.
         *
         * Throws std::runtime_error if got stdin error.
         */
        void import_note() const;

    public:
        explicit Menu(Note_Storage & notes_store);

        /**
         * Print heading. Uses object "Heading" to achieve it.
         */
        void print_heading() const;

        enum class User_Choice { CREATE, DISP, SEARCH, LIST_ALL,
                                 EDIT, DELETE,
                                 EXPORT, IMPORT, EXIT };
        /**
         * Asks user for what they want to do.
         *
         * A user can: create a new note,
         *             display an existing one by it's filename,
         *             search for a note with some filters,
         *             list all notes,
         *             edit some note, delete it, export it
         *             or exit.
         * Method throws std::invalid_argument upon catching invalid request.
         * Method is case insensitive.
         *
         * @return User's request.
         */
        User_Choice ask_user() const;

        /**
         * Adds distance between user actions.
         *
         * After user finishes some action in the program,
         * this method adds distance for better readability.
         * Configure how much space to add in variable m_DIST.
         */
        void add_distance() const;

        /**
         * Process the user request.
         *
         * @return true, if successfully processed the request;
         *      false, if got a User_Choice::EXIT request.
         */
        bool process_request(const User_Choice & requested_action);

};

// ---------------------------------------------------------------------------

/**
 *! \relates Menu
 * Get a timestamp in format "YYYY-MM-DD, HH:MM:SS".
 *
 * @return Timestamp in a proper date format.
 */
const std::string get_timestamp();

#endif  // MENU_HPP
