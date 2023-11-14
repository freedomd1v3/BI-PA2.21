#include <string>
#include <memory>
#include <fstream>
#include "export.hpp"
#include "markdown_export.hpp"

Markdown_Export::Markdown_Export(const std::string & path)
    : Export(path) { }

void Markdown_Export::operator () (const std::unique_ptr<Note> & to_export,
                                   std::ofstream & os) const {
    // TODO
    os << "# " << to_export->get_name() << std::endl
       << std::endl
       << "*Created at: " << to_export->get_creation_date() << "*  " << std::endl
       << std::endl;

    const std::string TO_SAVE = to_export->get_content();
    for (const auto & x: TO_SAVE) {
        if (x == '\n') {
            os << "  " << std::endl;
        }
        else {
            os << x;
        }
    }
}
