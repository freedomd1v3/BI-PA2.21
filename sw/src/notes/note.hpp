#ifndef NOTE_HPP
#define NOTE_HPP

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <ostream>

/**
 * A base abstract polymorphic class for all other note types.
 * Contains basic methods required in all of them.
 */
class Note {
    private:
        const std::string m_CREATION_TIMESTAMP;
        std::vector<std::string> m_Tags;

        /**
         * Set a name of the note and add change to the changelog.
         *
         * @param new_name A new name of the note.
         */
        void set_name(const std::string & new_name);

        /**
         * Edit a tag.
         *
         * Asks the user which tag to edit and then proceeds doing it.
         * Throws std::invalid_argument if got invalid input,
         * std::runtime_error if got problem in stdin.
         */
        void edit_tag();

        /**
         * Delete a tag.
         *
         * Asks the user which tag to remove and then proceeds doing it.
         * Throws std::invalid_argument if got invalid input,
         * std::runtime_error if got problem in stdin.
         */
        void delete_tag();

        /**
         * Add a new tag.
         *
         * Throws std::runtime_error if got an stdin error.
         *
         * @return true, if added new tag;
         *      false, if didn't (user entered empty line).
         */
        bool add_tag();

    protected:
        std::string m_Name;
        // 1st element is timestamp, 2nd element is a change.
        std::vector<std::pair<std::string, std::string>> m_Changelog;

    public:
        explicit Note(const std::string & current_date);
        virtual ~Note() = default;

        /**
         * Edit a note.
         *
         * Asks the user how to edit the note:
         * in a base class ask for a new name.
         * Throws std::runtime_error if got input error.
         */
        virtual void edit();

        /**
         * Create a note.
         *
         * Adds the creation to the changelog and then calls edit() method.
         */
        void create();

        /**
         * Get a note's file name (creation timestamp).
         *
         * @return File name.
         */
        const std::string & get_file_name() const;

        /**
         * Save a note.
         *
         * Save a note to a file (provided std::ofstream &).
         * In base class saves creation timestamp, name, tags and changelog.
         *
         * @param os An std::ofstream & to save the note to.
         */
        virtual void save(std::ofstream & os) const;

        /**
         * Print a note to the provided std::ostream.
         *
         * Prints a full note and it's changelog to the provided std::ostream.
         * In a base class, prints changelog.
         *
         * @param os An output stream to print the note.
        */
        virtual void print(std::ostream & os) const;

        /**
         * Finish initializing the note (finish reading the file).
         *
         * Throws std::runtime_error if got problems in input file.
         *
         * @param os An input stream where to read the note from
         */
        virtual void read(std::ifstream & os);

        /**
         * Get a brief summary of the note.
         *
         * Prints the type of the note and it's name (if a note has one),
         * so that the user could understand what the note is about.
         *
         * @return Summary
         */
        virtual std::string get_summary() const = 0;

        /**
         * Get a name. Is useful in filters and exporting to other formats.
         * 
         * @return Const reference to "m_Name".
         */
        const std::string & get_name() const;

        /**
         * Get a creation date. Is useful in filters.
         *
         * @return Const reference to first element in first pair in
         *         "m_Changelog".
         */
        const std::string & get_creation_date() const;

        /**
         * Get tags. Is useful in filters.
         *
         * @return Const reference to std::vector of tags.
         */
        const std::vector<std::string> & get_tags() const;

        /**
         * Checks whether or not the note contains the provided text.
         *
         * @param  text A text to check
         * @return true, if does;
         *      false otherwise.
         */
        virtual bool contains(const std::string & text) const = 0;

        /**
         * Get content of the note (useful for exports to standard
         * file formats).
         *
         * @return A note content.
         */
        virtual std::string get_content() const = 0;
};

#endif  // NOTE_HPP
