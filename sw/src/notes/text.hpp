#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <fstream>
#include <ostream>
#include "note.hpp"

/**
 * Text note
 */
class Text: public Note {
    private:
        std::string m_Text;

    public:
        explicit Text(const std::string & current_date);

        /**
         * Edit a note.
         *
         * In "Text" type asks for a new text. Exceptions are same
         */
        virtual void edit() override;

        virtual void save(std::ofstream & os) const override;

        virtual void print(std::ostream & os) const override;

        virtual void read(std::ifstream & os) override;

        virtual std::string get_summary() const override;

        virtual bool contains(const std::string & text) const override;

        virtual std::string get_content() const override;
};

#endif  // TEXT_HPP
