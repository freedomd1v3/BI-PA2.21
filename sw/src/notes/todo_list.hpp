#ifndef TODO_LIST_HPP
#define TODO_LIST_HPP

#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <ostream>
#include "note.hpp"

/**
 * A note of type "to-do list"
 */
class TODO_List: public Note {
    private:
        std::vector<std::pair<std::string, std::string>> m_List;

        /**
         * Checks, whether or not provided record exists in m_List.
         *
         * @param  record A record to check.
         * @return false, if record exists;
         *      true otherwise.
         */
        bool check_uniqueness(const std::string & record);

        /**
         * Edit an existing item.
         *
         * Asks the user which record to edit and then proceeds doing it.
         * Throws std::invalid_argument if got invalid input,
         * std::runtime if got problem in stdin.
         */
        void edit_record();

        /**
         * Delete an existing item.
         *
         * Asks the user which record to remove and then proceeds doing it.
         * Throws std::invalid_argument if got invalid input,
         * std::runtime if got problem in stdin.
         */
        void delete_record();

        /**
         * Add a new record to "TODO_List".
         *
         * Throws std::runtime_error if got a problem in stdin.
         *
         * @return true, if added new record;
         *      false, if didn't (user entered empty line as a name).
         */
        bool add_record();

    public:
        explicit TODO_List(const std::string & current_date);

        /**
         * Edit a note.
         *
         * In "TODO_List" type asks how to edit already existing elements
         * (including deletion) and for new ones.
         */
        virtual void edit() override;

        virtual void save(std::ofstream & os) const override;

        virtual void print(std::ostream & os) const override;

        virtual void read(std::ifstream & os) override;

        virtual std::string get_summary() const override;

        virtual bool contains(const std::string & text) const override;

        virtual std::string get_content() const override;
};

#endif  // TODO_LIST_HPP
