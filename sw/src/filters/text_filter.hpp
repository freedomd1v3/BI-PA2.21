#ifndef TEXT_FILTER_HPP
#define TEXT_FILTER_HPP

#include <string>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "../notes/note.hpp"

/**
 * A filter to search the note by the text it needs to contain.
 */
class Text_Filter: public Filter {
    private:
        std::string m_Text_Criteria;

    public:
        /**
         * Request a criteria by which to filter the notes.
         *
         * Throws std::runtime_error if got problem in stdin
         * or std::invalid_argument if got invalid criteria.
         * In "Text_Filter" asks for a text a note should (or should NOT,
         * depending on "m_Reverse") contain.
         */
        virtual void request_criteria() override;

        virtual bool operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const override;
};

#endif  // TEXT_FILTER_HPP
