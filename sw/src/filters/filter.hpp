#ifndef FILTER_HPP
#define FILTER_HPP

#include <utility>
#include <string>
#include <memory>
#include "../notes/note.hpp"

/**
 * A base abstract class to work with filters
 */
class Filter {
    protected:
        // Whether or not results should be in reverse.
        bool m_Reverse = true;

    public:
        virtual ~Filter() = default;

        /**
         * Request a criteria by which to filter the notes.
         *
         * Throws std::runtime_error if got problem in stdin
         * or std::invalid_argument if got invalid criteria.
         */
        virtual void request_criteria();

        /**
         * Search in notes and signalize those suitable by this filter.
         *
         * Check whether or not the note applies for the filter or not.
         * Should be used on a list of notes to check them
         * and find those suitable, but can also be used on an
         * individual note as well.
         *
         * @param  check A pair with a note and it's directory to check.
         * @return true, if does apply;
         *      false otherwise.
         */
        virtual bool operator () (const std::pair<std::string, std::unique_ptr<Note>> & check) const = 0;
};

#endif	// FILTER_HPP
