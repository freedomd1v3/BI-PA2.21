#ifndef HEADING_HPP
#define HEADIND_HPP

#include <string_view>

/**
 * A class, containing heading and a method to print it. Used by Menu.
 */
class Heading {
    private:
        std::string_view m_appname = "notepad", m_author = "sagymass";

    public:
        /**
         * Print the heading to std::cout.
         *
         * Customize the author's name in m_AUTHOR.
         */
        void print() const;
};

#endif  // HEADING_HPP
