#include <iostream>
#include "heading.hpp"

void Heading::print() const {
    using std::cout, std::endl;

    cout << "Welcome to " << m_appname
         << ", written by " << m_author << '!' << endl << endl;
}
