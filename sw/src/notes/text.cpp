#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <ostream>
#include "note.hpp"
#include "text.hpp"
#include "../menu.hpp"

Text::Text(const std::string & current_date)
    : Note(current_date) { }

void Text::edit() {
    Note::edit();

    bool changed_text = false;
    for (;;) {
        std::cout << "Please enter a new text for the note:";
        if (m_Text.size()) {
            std::cout << " (use empty line not to edit the text)";
        }
        std::cout << std::endl;
        std::string new_text;
        std::getline(std::cin, new_text);
        if (!std::cin.good()) {
            throw std::runtime_error("Text::edit(): Couldn't read a new text for the note.");
        }
        else if (!new_text.size()) {
            if (!m_Text.size()) {
                std::cerr << "ERROR: Text::edit(): Text of the note can't be empty." << std::endl << std::endl;
            }
            else {
                break;
            }
        }
        else {
            m_Text = new_text;
            changed_text = true;
            break;
        }
    }
    if (changed_text) {
        m_Changelog.emplace_back(get_timestamp(), "Changed text: " + m_Text);
    }
    std::cout << std::endl;
}

void Text::save(std::ofstream & os) const {
    os << "text" << std::endl << std::endl;
    Note::save(os);
    os << m_Text << std::endl;
}

void Text::print(std::ostream & os) const {
    os << "Text note ";
    if (m_Name.size()) {
        os << '"' << m_Name << "\" ";
    }
    os << "created at: " << m_Changelog.front().first << std::endl;
    os << m_Text << std::endl;

    Note::print(os);
}

void Text::read(std::ifstream & os) {
    Note::read(os);

    std::getline(os, m_Text);
    if (!os.good()) {
        throw std::runtime_error("Text::read(): File is damaged.");
    }
    else if (!m_Text.size()) {
        throw std::runtime_error("Text::read(): File is corrupted.");
    }
}

std::string Text::get_summary() const {
    std::string summary = "Text";
    if (m_Name.size()) {
        summary.append(": " + m_Name);
    }
    return summary;
}

bool Text::contains(const std::string & text) const {
    return m_Text.find(text) != std::string::npos;
}

std::string Text::get_content() const {
    std::string to_return = "Text note with content:";
    to_return.append("\n\t" + m_Text);
    return to_return;
}
