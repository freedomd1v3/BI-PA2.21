#ifndef DIRECTORY_FILTER_HPP
#define DIRECTORY_FILTER_HPP

#include <string>
#include <utility>
#include <memory>
#include "filter.hpp"
#include "../notes/note.hpp"

/**
 * A filter to search the note by it's directory.
 */
class Directory_Filter: public Filter {
    private:
        std::string m_Directory_Criteria;

    public:
        /**
         * Request a criteria by which to filter the notes.
         *
         * Throws std::runtime_error if got problem in stdin
         * or std::invalid_argument if got invalid criteria.
         * In "Directory_Filter" asks for a directory to filter the notes.
         */
        virtual void request_criteria() override;

        virtual bool operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const override;
};

#endif  // DIRECTORY_FILTER_HPP
