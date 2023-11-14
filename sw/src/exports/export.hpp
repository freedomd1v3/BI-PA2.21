#ifndef EXPORT_HPP
#define EXPORT_HPP

#include <string>
#include <memory>
#include <fstream>
#include "../notes/note.hpp"

/**
 * A base abstract class to export notes to standard file formats.
 */
struct Export {
    public:
        explicit Export(const std::string & path);

        /**
         * Export the provided note to the standard file format.
         *
         * @param to_export A note to export.
         * @param os        A file to export the note to.
         */
        virtual void operator () (const std::unique_ptr<Note> & to_export,
                                  std::ofstream & os) const = 0;

        const std::string m_PATH;
};

#endif	// EXPORT_HPP
