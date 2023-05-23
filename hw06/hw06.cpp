#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <typeinfo>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <algorithm>
#include <type_traits>

class CRect {
    public:
        CRect(double x, double y, double w, double h)
            : m_X(x), m_Y(y), m_W(w), m_H(h) { }

        double m_X, m_Y, m_W, m_H;

        friend std::ostream & operator << (std::ostream & os, const CRect & x) {
            return os << '(' << x.m_X << ',' << x.m_Y << ',' << x.m_W << ',' << x.m_H << ')';
        }
};
#endif  /* __PROGTEST__ */

// ---------------------------------------------------------------------------
// "CObj", because all buttons, inputs, etc. are all objects from user's POV
class CObj {
    protected:
        int m_Id;

        const CRect m_REL_POS;
        CRect m_pos_to_print { -1, -1, -1, -1 };

    public:
        explicit CObj(int id, const CRect & relPos)
            : m_Id(id), m_REL_POS(relPos) { }
        CObj(const CObj & src)
            : m_Id(src.m_Id), m_REL_POS(src.m_REL_POS),
              m_pos_to_print(src.m_pos_to_print) { }
        virtual ~CObj() noexcept = default;

        // I'd like to thank my teacher,
        // Radek Husek (diacritics are not used to avoid encoding problems),
        // for showing on laboratory
        // how to properly clone all derived objects with just 1 method
        virtual CObj * make_clone() const = 0;

        // Used in almost all objects except for CWindow
        virtual void set_pos_to_print(const CRect & abs_pos) {
            m_pos_to_print.m_X = abs_pos.m_X + m_REL_POS.m_X * abs_pos.m_W;
            m_pos_to_print.m_Y = abs_pos.m_Y + m_REL_POS.m_Y * abs_pos.m_H;

            m_pos_to_print.m_W = abs_pos.m_W * m_REL_POS.m_W;
            m_pos_to_print.m_H = abs_pos.m_H * m_REL_POS.m_H;
        }

        // Dummy method, should be present in all derived classes
        virtual CObj * search(int id) const {
            return nullptr;
        }
        int get_id() const {
            return m_Id;
        }

        // "IS_LAST" is a collection of information on which level
        // is the object last (according to it, '|' character may or may NOT
        // be drawn before heading)
        void print_header(std::ostream & os,
                          const std::vector<bool> & IS_LAST) const {
            for (size_t i = 1; i < IS_LAST.size(); i++) {
                if (!IS_LAST.at(i - 1)) {
                    os << '|';
                }
                else {
                    os << ' ';
                }
                os << "  ";
            }
            if (IS_LAST.size()) {
                os << "+- ";
            }
        }
        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const = 0;

        friend std::ostream & operator << (std::ostream & os, const CObj & x) {
            x.print(os, { });
            return os;
        }
};

// ---------------------------------------------------------------------------

class CButton: public CObj {
    private:
        const std::string m_NAME;

    public:
        explicit CButton(int id, const CRect & relPos, const std::string & name)
            : CObj(id, relPos), m_NAME(name) { }
        CButton(const CButton & src)
            : CObj(src), m_NAME(src.m_NAME) { }

        virtual CObj * make_clone() const {
            return new CButton(*this);
        }

        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const override {
            if (m_pos_to_print.m_X == -1 || m_pos_to_print.m_Y == -1
                || m_pos_to_print.m_W == -1 || m_pos_to_print.m_H == -1) {
                throw std::runtime_error("CButton::print: m_pos_to_print is used uninitialized.");
            }
            print_header(os, IS_LAST);
            os << '[' << m_Id << "] Button \"" << m_NAME << "\" " << m_pos_to_print << std::endl;
        }
};

// ---------------------------------------------------------------------------

class CInput: public CObj {
    private:
        std::string m_Value;

    public:
        explicit CInput(int id, const CRect & relPos, const std::string & value)
            : CObj(id, relPos), m_Value(value) { }
        CInput(const CInput & src)
            : CObj(src), m_Value(src.m_Value) { }

        virtual CObj * make_clone() const {
            return new CInput(*this);
        }

        void setValue(const std::string & new_value) {
            m_Value = new_value;
        }
        const std::string & getValue() const {
            return m_Value;
        }

        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const override {
            if (m_pos_to_print.m_X == -1 || m_pos_to_print.m_Y == -1
                || m_pos_to_print.m_W == -1 || m_pos_to_print.m_H == -1) {
                throw std::runtime_error("CInput::print: m_pos_to_print is used uninitialized.");
            }
            print_header(os, IS_LAST);
            os << '[' << m_Id << "] Input \"" << m_Value << "\" " << m_pos_to_print << std::endl;
        }
};

// ---------------------------------------------------------------------------

class CLabel: public CObj {
    private:
        const std::string m_LABEL;

    public:
        explicit CLabel(int id, const CRect & relPos, const std::string & label)
            : CObj(id, relPos), m_LABEL(label) { }
        CLabel(const CLabel & src)
            : CObj(src), m_LABEL(src.m_LABEL) { }

        virtual CObj * make_clone() const {
            return new CLabel(*this);
        }

        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const override {
            if (m_pos_to_print.m_X == -1 || m_pos_to_print.m_Y == -1
                || m_pos_to_print.m_W == -1 || m_pos_to_print.m_H == -1) {
                throw std::runtime_error("CLabel::print: m_pos_to_print is used uninitialized.");
            }
            print_header(os, IS_LAST);
            os << '[' << m_Id << "] Label \"" << m_LABEL << "\" " << m_pos_to_print << std::endl;
        }
};

// ---------------------------------------------------------------------------

class CComboBox: public CObj {
    private:
        std::vector<std::string> m_Offers;
        size_t m_Selected = 0;

    public:
        explicit CComboBox(int id, const CRect & relPos)
            : CObj(id, relPos) { }
        CComboBox(const CComboBox & src)
            : CObj(src),
              m_Offers(src.m_Offers), m_Selected(src.m_Selected) { }

        virtual CObj * make_clone() const {
            return new CComboBox (*this);
        }

        CComboBox & add(const std::string & offer) {
            m_Offers.push_back(offer);
            return *this;
        }

        void setSelected(size_t new_selected) {
            m_Selected = new_selected;
        }
        size_t getSelected() const {
            return m_Selected;
        }

        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const override {
            if (m_pos_to_print.m_X == -1 || m_pos_to_print.m_Y == -1
                || m_pos_to_print.m_W == -1 || m_pos_to_print.m_H == -1) {
                throw std::runtime_error("CComboBox::print: m_pos_to_print is used uninitialized.");
            }
            print_header(os, IS_LAST);
            os << '[' << m_Id << "] ComboBox " << m_pos_to_print << std::endl;

            for (size_t i = 0; i < m_Offers.size(); i++) {
                for (size_t j = 0; j < IS_LAST.size(); j++) {
                    if (!IS_LAST.at(j)) {
                        os << '|';
                    }
                    else {
                        os << ' ';
                    }
                    os << "  ";
                }

                os << "+-";
                if (i == m_Selected) {
                    os << '>';
                }
                else {
                    os << ' ';
                }
                os << m_Offers.at(i);
                if (i == m_Selected) {
                    os << '<';
                }
                os << std::endl;
            }
        }
};

// ---------------------------------------------------------------------------

class CPanel: public CObj {
    private:
        std::vector<std::unique_ptr<CObj>> m_Objects;

    public:
        explicit CPanel(int id, const CRect & relPos)
            : CObj(id, relPos) { }
        CPanel(const CPanel & src)
            : CObj(src) {
            for (const auto & x: src.m_Objects) {
                m_Objects.emplace_back(x->make_clone());
            }
        }

        virtual CObj * make_clone() const override {
            return new CPanel(*this);
        }

        virtual void set_pos_to_print(const CRect & new_pos) override {
            CObj::set_pos_to_print(new_pos);

            // Doing the same in all included objects
            for (const auto & x: m_Objects) {
                x->set_pos_to_print(m_pos_to_print);
            }
        }

        CPanel & add(const CObj & x) {
            m_Objects.emplace_back(x.make_clone());
            m_Objects.back()->set_pos_to_print(m_pos_to_print);
            return *this;
        }

        virtual CObj * search(int id) const {
            for (const auto & x: m_Objects) {
                if (x->get_id() == id) {
                    return x.get(); // Found an object with the exact id
                }
                else if (x->search(id) != nullptr) {
                    // Found an object, where the object with the exact id
                    // is included
                    return x->search(id);
                }
            }
            return nullptr;
        }

        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const override {
            if (m_pos_to_print.m_X == -1 || m_pos_to_print.m_Y == -1
                || m_pos_to_print.m_W == -1 || m_pos_to_print.m_H == -1) {
                throw std::runtime_error("CPanel::print: m_pos_to_print is used uninitialized.");
            }
            print_header(os, IS_LAST);
            os << '[' << m_Id << "] Panel " << m_pos_to_print << std::endl;

            for (size_t i = 0; i < m_Objects.size(); i++) {
                std::vector<bool> current_status = IS_LAST;
                // This "-1" won't cause any overflows
                current_status.push_back(i == m_Objects.size() - 1);
                m_Objects.at(i)->print(os, current_status);
            }
        }
};

// ---------------------------------------------------------------------------

class CWindow: public CObj {
    private:
        std::string m_Title;

        std::vector<std::unique_ptr<CObj>> m_Objects;

    public:
        explicit CWindow(int id, const std::string & title, const CRect & absPos)
            : CObj(id, CRect(-1, -1, -1, -1)), m_Title(title) {
            set_pos_to_print(absPos);
        }
        CWindow(const CWindow & src)
            : CObj(src.m_Id, CRect (-1, -1, -1, -1)), m_Title(src.m_Title) {
            set_pos_to_print(src.m_pos_to_print);

            for (const auto & x: src.m_Objects) {
                m_Objects.emplace_back(x->make_clone());
            }
        }
        CWindow & operator = (const CWindow & src) {
            if (&src == this) {
                return *this;
            }

            m_Objects.clear();

            m_Id = src.m_Id;
            m_Title = src.m_Title;
            setPosition(src.m_pos_to_print);

            for (const auto & x: src.m_Objects) {
                m_Objects.emplace_back(x->make_clone());
            }
            return *this;
        }

        virtual CObj * make_clone() const {
            return new CWindow(*this);
        }

        // This method should be called "set_window_pos" here instead,
        // otherwise it's confusing :/
        virtual void set_pos_to_print(const CRect & new_pos) override {
            m_pos_to_print = new_pos;
        }
        void setPosition(const CRect & new_pos) {
            // That's exactly what I've been talking about
            set_pos_to_print(new_pos);

            for (auto & x: m_Objects) {
                x->set_pos_to_print(new_pos);
            }
        }

        virtual CWindow & add(const CObj & x) {
            m_Objects.emplace_back(x.make_clone());
            m_Objects.back()->set_pos_to_print(m_pos_to_print);
            return *this;
        }

        virtual CObj * search(int id) const override {
            for (const auto & x: m_Objects) {
                if (x->get_id() == id) {
                    return x.get();
                }
                else if (x->search(id) != nullptr) {
                    return x->search(id);
                }
            }
            return nullptr;
        }

        virtual void print(std::ostream & os,
                           const std::vector<bool> & IS_LAST) const override {
            os << '[' << m_Id << "] Window \"" << m_Title << "\" " << m_pos_to_print << std::endl;

            for (size_t i = 0; i < m_Objects.size(); i++) {
                std::vector<bool> current_status = { i == m_Objects.size() - 1 };
                // Again, this "-1" won't cause any overflows
                m_Objects.at(i)->print(os, current_status);
            }
        }
};
// ---------------------------------------------------------------------------

#ifndef __PROGTEST__
template <typename _T>
std::string toString(const _T & x)
{
    std::ostringstream oss;
    oss << x;
    return oss.str();
}

#define LVL 2

int main() {
    double got_points = 0;

#if LVL >= 1
    {
        double points_for_the_level = 3.3;
        std::cout << "LVL 1 (easy task)" << std::endl;
        got_points += points_for_the_level;

        assert(sizeof(CButton) - sizeof(std::string) < sizeof(CComboBox) - sizeof(std::vector<std::string>));
        assert(sizeof(CInput) - sizeof(std::string) < sizeof(CComboBox) - sizeof(std::vector<std::string>));
        assert(sizeof(CLabel) - sizeof(std::string) < sizeof(CComboBox) - sizeof(std::vector<std::string>));
        CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
        a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
        a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
        a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
        a.add(CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest"));
        assert(toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [20] ComboBox (70,154,480,48)\n"
            "   +->Karate<\n"
            "   +- Judo\n"
            "   +- Box\n"
            "   +- Progtest\n");
        CWindow b = a;
        b = b;
        assert(toString(*(b.search(20))) ==
            "[20] ComboBox (70,154,480,48)\n"
            "+->Karate<\n"
            "+- Judo\n"
            "+- Box\n"
            "+- Progtest\n");
        assert(dynamic_cast<CComboBox &> (*(b.search(20))).getSelected() == 0);
        dynamic_cast<CComboBox &> (*(b.search(20))).setSelected(3);
        assert(dynamic_cast<CInput &> (*(b.search(11))).getValue() == "chucknorris");
        dynamic_cast<CInput &> (*(b.search(11))).setValue("chucknorris@fit.cvut.cz");
        b.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
        assert(toString(b) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
            "+- [20] ComboBox (70,154,480,48)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (70,250,480,48)\n"
            "   +->PA2<\n"
            "   +- OSY\n"
            "   +- Both\n");
        assert(toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [20] ComboBox (70,154,480,48)\n"
            "   +->Karate<\n"
            "   +- Judo\n"
            "   +- Box\n"
            "   +- Progtest\n");
        b.setPosition(CRect(20, 30, 640, 520));
        assert(toString(b) ==
            "[0] Window \"Sample window\" (20,30,640,520)\n"
            "+- [1] Button \"Ok\" (84,446,192,52)\n"
            "+- [2] Button \"Cancel\" (404,446,192,52)\n"
            "+- [10] Label \"Username:\" (84,82,128,52)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
            "+- [20] ComboBox (84,186,512,52)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (84,290,512,52)\n"
            "   +->PA2<\n"
            "   +- OSY\n"
            "   +- Both\n");
    }
#endif

#if LVL >= 2
    {
        double points_for_the_level = 2.2;
        std::cout << "LVL 2 (harder task)" << std::endl;
        got_points += points_for_the_level;

        assert(sizeof(CInput) - sizeof(std::string) < sizeof(CComboBox) - sizeof(std::vector<std::string>));
        assert(sizeof(CLabel) - sizeof(std::string) < sizeof(CComboBox) - sizeof(std::vector<std::string>));
        assert(sizeof(CButton) - sizeof(std::string) <= sizeof(CPanel) - sizeof(std::vector<void*>));
        assert(sizeof(CInput) - sizeof(std::string) <= sizeof(CPanel) - sizeof(std::vector<void*>));
        assert(sizeof(CLabel) - sizeof(std::string) <= sizeof(CPanel) - sizeof(std::vector<void*>));
        CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
        a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
        a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
        a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
        a.add(CPanel(12, CRect(0.1, 0.3, 0.8, 0.7)).add(CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest")));
        assert(toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "      +->Karate<\n"
            "      +- Judo\n"
            "      +- Box\n"
            "      +- Progtest\n");
        CWindow b = a;
        assert(toString(*(b.search(20))) ==
        "[20] ComboBox (118,254.8,384,33.6)\n"
        "+->Karate<\n"
        "+- Judo\n"
        "+- Box\n"
        "+- Progtest\n");
        assert(dynamic_cast<CComboBox &> (*(b.search(20))).getSelected() == 0);
        dynamic_cast<CComboBox &> (*(b.search(20))).setSelected(3);
        assert(dynamic_cast<CInput &> (*(b.search (11))).getValue() == "chucknorris");
        dynamic_cast<CInput &> (*(b.search(11))).setValue("chucknorris@fit.cvut.cz");
        CPanel & p = dynamic_cast<CPanel &> (*(b.search(12)));
        p.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
        assert(toString(b) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (118,322,384,33.6)\n"
            "      +->PA2<\n"
            "      +- OSY\n"
            "      +- Both\n");
        assert(toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "      +->Karate<\n"
            "      +- Judo\n"
            "      +- Box\n"
            "      +- Progtest\n");
        assert(toString(p) ==
            "[12] Panel (70,154,480,336)\n"
            "+- [20] ComboBox (118,254.8,384,33.6)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (118,322,384,33.6)\n"
            "   +->PA2<\n"
            "   +- OSY\n"
            "   +- Both\n");
        b.setPosition(CRect(20, 30, 640, 520));
        assert(toString(b) ==
            "[0] Window \"Sample window\" (20,30,640,520)\n"
            "+- [1] Button \"Ok\" (84,446,192,52)\n"
            "+- [2] Button \"Cancel\" (404,446,192,52)\n"
            "+- [10] Label \"Username:\" (84,82,128,52)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
            "+- [12] Panel (84,186,512,364)\n"
            "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "      +->PA2<\n"
            "      +- OSY\n"
            "      +- Both\n");
        p.add(p);
        assert(toString(p) ==
            "[12] Panel (84,186,512,364)\n"
            "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "|  +->PA2<\n"
            "|  +- OSY\n"
            "|  +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "      +->PA2<\n"
            "      +- OSY\n"
            "      +- Both\n");
        p.add(p);
        assert(toString(p) ==
            "[12] Panel (84,186,512,364)\n"
            "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "|  +->PA2<\n"
            "|  +- OSY\n"
            "|  +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "|  |  +- Karate\n"
            "|  |  +- Judo\n"
            "|  |  +- Box\n"
            "|  |  +->Progtest<\n"
            "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "|     +->PA2<\n"
            "|     +- OSY\n"
            "|     +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |  +->PA2<\n"
            "   |  +- OSY\n"
            "   |  +- Both\n"
            "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "      |  +- Karate\n"
            "      |  +- Judo\n"
            "      |  +- Box\n"
            "      |  +->Progtest<\n"
            "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "         +->PA2<\n"
            "         +- OSY\n"
            "         +- Both\n");
        p.add(p);
        assert(toString(p) ==
            "[12] Panel (84,186,512,364)\n"
            "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "|  +->PA2<\n"
            "|  +- OSY\n"
            "|  +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "|  |  +- Karate\n"
            "|  |  +- Judo\n"
            "|  |  +- Box\n"
            "|  |  +->Progtest<\n"
            "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "|     +->PA2<\n"
            "|     +- OSY\n"
            "|     +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "|  |  +- Karate\n"
            "|  |  +- Judo\n"
            "|  |  +- Box\n"
            "|  |  +->Progtest<\n"
            "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "|  |  +->PA2<\n"
            "|  |  +- OSY\n"
            "|  |  +- Both\n"
            "|  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "|     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "|     |  +- Karate\n"
            "|     |  +- Judo\n"
            "|     |  +- Box\n"
            "|     |  +->Progtest<\n"
            "|     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "|        +->PA2<\n"
            "|        +- OSY\n"
            "|        +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |  +->PA2<\n"
            "   |  +- OSY\n"
            "   |  +- Both\n"
            "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "   |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "   |  |  +- Karate\n"
            "   |  |  +- Judo\n"
            "   |  |  +- Box\n"
            "   |  |  +->Progtest<\n"
            "   |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "   |     +->PA2<\n"
            "   |     +- OSY\n"
            "   |     +- Both\n"
            "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "      |  +- Karate\n"
            "      |  +- Judo\n"
            "      |  +- Box\n"
            "      |  +->Progtest<\n"
            "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "      |  +->PA2<\n"
            "      |  +- OSY\n"
            "      |  +- Both\n"
            "      +- [12] Panel (208.928,425.148,262.144,124.852)\n"
            "         +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
            "         |  +- Karate\n"
            "         |  +- Judo\n"
            "         |  +- Box\n"
            "         |  +->Progtest<\n"
            "         +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
            "            +->PA2<\n"
            "            +- OSY\n"
            "            +- Both\n");
        assert(toString(b) ==
            "[0] Window \"Sample window\" (20,30,640,520)\n"
            "+- [1] Button \"Ok\" (84,446,192,52)\n"
            "+- [2] Button \"Cancel\" (404,446,192,52)\n"
            "+- [10] Label \"Username:\" (84,82,128,52)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
            "+- [12] Panel (84,186,512,364)\n"
            "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "   |  +->PA2<\n"
            "   |  +- OSY\n"
            "   |  +- Both\n"
            "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  |  +- Karate\n"
            "   |  |  +- Judo\n"
            "   |  |  +- Box\n"
            "   |  |  +->Progtest<\n"
            "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |     +->PA2<\n"
            "   |     +- OSY\n"
            "   |     +- Both\n"
            "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  |  +- Karate\n"
            "   |  |  +- Judo\n"
            "   |  |  +- Box\n"
            "   |  |  +->Progtest<\n"
            "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |  |  +->PA2<\n"
            "   |  |  +- OSY\n"
            "   |  |  +- Both\n"
            "   |  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "   |     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "   |     |  +- Karate\n"
            "   |     |  +- Judo\n"
            "   |     |  +- Box\n"
            "   |     |  +->Progtest<\n"
            "   |     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "   |        +->PA2<\n"
            "   |        +- OSY\n"
            "   |        +- Both\n"
            "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "      +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "      |  +- Karate\n"
            "      |  +- Judo\n"
            "      |  +- Box\n"
            "      |  +->Progtest<\n"
            "      +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "      |  +->PA2<\n"
            "      |  +- OSY\n"
            "      |  +- Both\n"
            "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "      |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "      |  |  +- Karate\n"
            "      |  |  +- Judo\n"
            "      |  |  +- Box\n"
            "      |  |  +->Progtest<\n"
            "      |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "      |     +->PA2<\n"
            "      |     +- OSY\n"
            "      |     +- Both\n"
            "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "         +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "         |  +- Karate\n"
            "         |  +- Judo\n"
            "         |  +- Box\n"
            "         |  +->Progtest<\n"
            "         +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "         |  +->PA2<\n"
            "         |  +- OSY\n"
            "         |  +- Both\n"
            "         +- [12] Panel (208.928,425.148,262.144,124.852)\n"
            "            +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
            "            |  +- Karate\n"
            "            |  +- Judo\n"
            "            |  +- Box\n"
            "            |  +->Progtest<\n"
            "            +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
            "               +->PA2<\n"
            "               +- OSY\n"
            "               +- Both\n");
        assert(toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "      +->Karate<\n"
            "      +- Judo\n"
            "      +- Box\n"
            "      +- Progtest\n");
    }
#endif

    std::cout << "\tYay! You passed LVL " << LVL << std::endl
              << "\tYou gain " << got_points << " points" << std::endl;
    return 0;
}
#endif  /* __PROGTEST__ */
