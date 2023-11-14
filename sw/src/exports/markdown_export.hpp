#ifndef MARKDOWN_EXPORT_HPP
#define MARKDOWN_EXPORT_HPP

#include <string>
#include <memory>
#include <fstream>
#include "export.hpp"
#include "../notes/note.hpp"

/**
 * Export the note to Markdown file format.
 */
struct Markdown_Export: public Export {
    public:
        explicit Markdown_Export(const std::string & path);

        virtual void operator () (const std::unique_ptr<Note> & to_export,
                                  std::ofstream & os) const override;
};

#endif  // MARKDOWN_EXPORT_HPP
