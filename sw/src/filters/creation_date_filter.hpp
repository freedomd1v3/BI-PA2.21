#ifndef CREATION_DATE_FILTER_HPP
#define CREATION_DATE_FILTER_HPP

#include <string>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "../notes/note.hpp"

/**
 * A filter to search the note by it's creation date.
 */
class Creation_Date_Filter: public Filter {
    private:
        std::string m_Creation_Date_Criteria;

        /**
         * Checks, if a user-entered "m_Creation_Date_Criteria" is valid.
         *
         * @return true, if is;
         *      false otherwise.
         */
        bool check_creation_date_validity();

    public:
        /**
         * Request a criteria by which to filter the notes.
         *
         * Throws std::runtime_error if got problem in stdin
         * or std::invalid_argument if got invalid criteria.
         * In "Creation_Date_Filter" asks for a creation date by which
         * to filter the notes.
         */
        virtual void request_criteria() override;

        virtual bool operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const override;
};

#endif  // CREATION_DATE_FILTER_HPP
