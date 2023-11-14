#ifndef SHOPPING_LIST_HPP
#define SHOPPING_LIST_HPP

#include <vector>
#include <string>
#include <fstream>
#include <ostream>
#include "note.hpp"

/**
 * A note of type "shopping list"
 */
class Shopping_List: public Note {
    private:
        std::vector<std::string> m_List;

        /**
         * Edit an existing item.
         *
         * Asks the user how to edit the record and then proceeds doing it.
         * Throws std::invalid_argument if got invalid input,
         * std::runtime_error if got problem in stdin.
         */
        void edit_record();

        /**
         * Delete an existing item.
         *
         * Asks the user which record to remove and then proceeds doing it.
         * Throws std::invalid_argument if got invalid input,
         * std::runtime_error if got problem in stdin.
         */
        void delete_record();

        /**
         * Add a new record to "Shopping_List".
         *
         * Throws std::runtime_error if got an input error.
         *
         * @return true, if added new record;
         *      false, if didn't (user entered empty line).
         */
        bool add_record();

    public:
        explicit Shopping_List(const std::string & current_date);

        /**
         * Edit a note.
         *
         * In "Shopping_List" type asks how to edit already existing elements
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

#endif  // SHOPPING_LIST_HPP
