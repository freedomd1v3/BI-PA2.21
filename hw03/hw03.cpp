#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>
#endif  /* __PROGTEST__ */

// Uncomment if your code implements initializer lists
// #define EXTENDED_SYNTAX

struct CRange {
    long long m_begin, m_end;

    CRange(long long lo, long long hi) {
        if (!(lo <= hi)) {
            throw std::invalid_argument("Nespravny vstup.");
        }
        m_begin = lo, m_end = hi;
    }

    /**
     * Check whether or not this range includes an element
     * @param elem
     * @return true, if does;
     *      false otherwise
     */
    bool inline
    includes(long long elem) const {
        if (m_begin <= elem && m_end >= elem) {
            return true;
        }
        return false;
    }
    /**
     * Check whether or not this range includes another range
     * @param range
     * @return true, if does;
     *      false otherwise
     */
    bool inline
    includes(const CRange & range) const {
        if (m_begin <= range.m_begin && m_end >= range.m_end) {
            return true;
        }
        return false;
    }

    /**
     * Check whether we can unite this range with another range
     * @param another
     * @return true, if we can;
     *      false otherwise
     */
    bool inline
    can_unite(const CRange & another) const {
        // Thanks to Stack Overflow
        if ((m_begin <= another.m_end + 1 && another.m_begin + 1 <= m_end)
            || (another.m_begin <= m_end + 1 && m_begin + 1 <= another.m_end)) {
            return true;
        }
        return false;
    }

    /**
     * Unite this interval with another interval.
     * Doesn't check if actually can unite those, it's up to programmer
     * to do this with method "can_unite()"
     * @param another
     */
    void inline
    unite(const CRange & another) {
        if (m_begin > another.m_begin) {
            m_begin = another.m_begin;
        }
        if (m_end < another.m_end) {
            m_end = another.m_end;
        }
    }

    /**
     * Check whether we can disunite this range with another range
     * @param another
     * @return true, if we can;
     *      false otherwise
     */
    bool inline
    can_disunite(const CRange & another) const {
        if (another.m_end < m_begin || another.m_begin > m_end) {
            return false;
        }
        return true;
    }

    bool operator == (const CRange & to_cmp) const {
        if (m_begin == to_cmp.m_begin && m_end == to_cmp.m_end) {
            return true;
        }
        return false;
    }

    bool operator != (const CRange & to_cmp) const {
        return !(*this == to_cmp);
    }

    friend std::ostream & operator << (std::ostream & os, const CRange & interval);
};
std::ostream & operator << (std::ostream & os, const CRange & interval) {
    if (interval.m_begin == interval.m_end) {
        os << interval.m_begin;
    }
    else {
        os << '<'
           << interval.m_begin
           << ".."
           << interval.m_end
           << '>';
    }
    return os;
}

class CRangeList {
    std::vector<CRange> m_intervals;

    public:
    bool
    includes(long long elem) const {
        size_t lo = 0, hi = m_intervals.size();
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (elem < m_intervals.at(mid).m_begin) {
                hi = mid;
            }
            else if (m_intervals.at(mid).includes(elem)) {
                return true;
            }
            else {
                lo = mid + 1;
            }
        }
        return false;
    }
    bool
    includes(const CRange & range) const {
        size_t lo = 0, hi = m_intervals.size();
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (range.m_begin < m_intervals.at(mid).m_begin) {
                // In this case we already know, that no interval
                // would include this one
                if (range.m_end >= m_intervals.at(mid).m_end) {
                    return false;
                }
                hi = mid;
            }
            else if (m_intervals.at(mid).includes(range)) {
                return true;
            }
            else {
                lo = mid + 1;
            }
        }
        return false;
    }

    // += range / range list
    CRangeList & operator += (const CRange & to_append) {
        bool found = false;
        size_t lo = 0, hi = m_intervals.size(), append_i;
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (to_append.m_begin < m_intervals.at(mid).m_begin) {
                hi = mid;
            }
            else if (to_append.m_begin == m_intervals.at(mid).m_begin) {
                append_i = mid;
                found = true;
                break;
            }
            else {
                lo = mid + 1;
            }
        }
        if (!found) {
            append_i = lo;
        }
        m_intervals.insert(m_intervals.begin() + append_i,
                           CRange(to_append.m_begin, to_append.m_end));
        if (append_i > 0 && to_append.can_unite(m_intervals.at(append_i - 1))) {
            m_intervals.at(append_i).unite(m_intervals.at(append_i - 1));
            m_intervals.erase(m_intervals.begin() + --append_i);
        }
        while (append_i + 1 < m_intervals.size()
               && m_intervals.at(append_i).can_unite(m_intervals.at(append_i + 1))) {
            m_intervals.at(append_i).unite(m_intervals.at(append_i + 1));
            m_intervals.erase(m_intervals.begin() + append_i + 1);
        }
        return *this;
    }
    CRangeList & operator += (const CRangeList & to_append) {
        // TODO: Perhaps improve this function?
        // What, if we actually unite all other intervals
        // in the original CRangeList before adding them here?
        // This would also require us to change "to_append" to "CRangeList &"
        for (size_t i = 0; i < to_append.m_intervals.size(); i++) {
            *this += to_append.m_intervals.at(i);
        }
        return *this;
    }

    // -= range / range list
    CRangeList & operator -= (const CRange & to_remove) {
        for (;;) {
            size_t lo = 0, hi = m_intervals.size(), mid;
            while (lo < hi) {
                mid = lo + (hi - lo) / 2;
                if (to_remove.m_end < m_intervals.at(mid).m_begin) {
                    hi = mid;
                }
                else if (to_remove.m_begin > m_intervals.at(mid).m_end) {
                    lo = mid + 1;
                }
                else {
                    break;
                }
            }
            if (!(mid < m_intervals.size()
                  && m_intervals.at(mid).can_disunite(to_remove))) {
                break;
            }
            while (mid < m_intervals.size()
                   && m_intervals.at(mid).can_disunite(to_remove)) {
                if (to_remove.includes(m_intervals.at(mid))) {
                    m_intervals.erase(m_intervals.begin() + mid);
                }
                else {
                    if (m_intervals.at(mid).m_begin < to_remove.m_begin
                        && m_intervals.at(mid).m_end > to_remove.m_end) {
                        m_intervals.insert(m_intervals.begin() + mid + 1,
                                           CRange(to_remove.m_end + 1,
                                                  m_intervals.at(mid).m_end));
                    }
                    if (m_intervals.at(mid).m_begin >= to_remove.m_begin) {
                        m_intervals.at(mid).m_begin = to_remove.m_end + 1;
                    }
                    else {
                        m_intervals.at(mid).m_end = to_remove.m_begin - 1;
                    }
                }
            }
        }
        return *this;
    }
    CRangeList & operator -= (const CRangeList & to_remove) {
        for (size_t i = 0; i < to_remove.m_intervals.size(); i++) {
            *this -= to_remove.m_intervals.at(i);
        }
        return *this;
    }

    // = range / range list
    CRangeList & operator = (const CRange & to_copy) {
        m_intervals.clear();
        m_intervals.push_back(to_copy);
        return *this;
    }
    CRangeList & operator = (const CRangeList & to_copy) {
        // TODO: Compact "to_copy"?
        m_intervals = to_copy.m_intervals;
        return *this;
    }

    bool operator == (const CRangeList & to_cmp) const {
        if (m_intervals.size() != to_cmp.m_intervals.size()) {
            return false;
        }
        for (size_t i = 0; i < m_intervals.size(); i++) {
            if (m_intervals.at(i) != to_cmp.m_intervals.at(i)) {
                return false;
            }
        }
        return true;
    }

    bool operator != (const CRangeList & to_cmp) const {
        return !(*this == to_cmp);
    }

    friend std::ostream & operator << (std::ostream & os,
                                       const CRangeList & list);
};
// You'd obviously need to rewrite this, if you want bonus points
CRangeList operator + (const CRange & first, const CRange & second) {
    CRangeList res;
    res += first, res += second;
    return res;
}
CRangeList operator + (CRangeList list, const CRange & to_add) {
    list += to_add;
    return list;
}
CRangeList operator - (const CRange & first, const CRange & second) {
    CRangeList res;
    res += first, res -= second;
    return res;
}
CRangeList operator - (CRangeList list, const CRange & to_add) {
    list -= to_add;
    return list;
}

std::ostream & operator << (std::ostream & os, const CRangeList & list) {
    os << '{';
    for (size_t i = 0; i < list.m_intervals.size(); i++) {
        os << list.m_intervals.at(i);
        if (i + 1 < list.m_intervals.size()) {
            os << ',';
        }
    }
    os << '}';
    return os;
}

#ifndef __PROGTEST__
std::string
to_string(const CRangeList& x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();
}

int
main() {
    CRangeList my_test_1;
    my_test_1 = CRange(-5, 0);
    my_test_1 += CRange(5, 10);
    my_test_1 += CRange(20, 50);
    my_test_1 += CRange(70, 100);
    my_test_1 += CRange(12, 1000);
    assert(to_string(my_test_1) == "{<-5..0>,<5..10>,<12..1000>}");

    CRangeList a, b;

    assert(sizeof(CRange) <= 2 * sizeof(long long));
    a = CRange(5, 10);
    a += CRange(25, 100);
    assert(to_string(a) == "{<5..10>,<25..100>}");
    a += CRange(-5, 0);
    a += CRange(8, 50);
    assert(to_string(a) == "{<-5..0>,<5..100>}");
    a += CRange(101, 105) + CRange(120, 150) + CRange(160, 180) + CRange(190, 210);
    assert(to_string(a) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}");
    a += CRange(106, 119) + CRange(152, 158);
    assert(to_string(a) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}");
    a += CRange(-3, 170);
    a += CRange(-30, 1000);
    assert(to_string(a) == "{<-30..1000>}");
    b = CRange(-500, -300) + CRange (2000, 3000) + CRange (700, 1001);
    a += b;
    assert(to_string(a) == "{<-500..-300>,<-30..1001>,<2000..3000>}");
    a -= CRange(-400, -400);
    assert(to_string(a) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}");
    a -= CRange(10, 20) + CRange(900, 2500) + CRange(30, 40) + CRange(10000, 20000);
    assert(to_string(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
    try {
        a += CRange(15, 18) + CRange(10, 0) + CRange(35, 38);
        assert("Exception not thrown" == nullptr);
    }
    catch(const std::logic_error & e) {
    }
    catch(...) {
        assert("Invalid exception thrown" == nullptr);
    }
    assert(to_string(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
    b = a;
    assert(a == b);
    assert(!(a != b));
    b += CRange(2600, 2700);
    assert(to_string(b) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
    assert(a == b);
    assert(!(a != b));
    b += CRange(15, 15);
    assert(to_string(b) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}");
    assert(!(a == b));
    assert(a != b);
    assert(b.includes(15));
    assert(b.includes(2900));
    assert(b.includes(CRange(15, 15)));
    assert(b.includes(CRange(-350, -350)));
    assert(b.includes(CRange(100, 200)));
    assert(!b.includes(CRange(800, 900)));
    assert(!b.includes(CRange(-1000, -450)));
    assert(!b.includes(CRange(0, 500)));
    a += CRange(-10000, 10000) + CRange(10000000, 1000000000);
    assert(to_string(a) == "{<-10000..10000>,<10000000..1000000000>}");
    b += a;
    assert(to_string(b) == "{<-10000..10000>,<10000000..1000000000>}");
    b -= a;
    assert(to_string(b) == "{}");
    b += CRange(0, 100) + CRange(200, 300) - CRange(150, 250) + CRange(160, 180) - CRange(170, 170);
    assert(to_string(b) == "{<0..100>,<160..169>,<171..180>,<251..300>}");
    b -= CRange(10, 90) - CRange(20, 30) - CRange(40, 50) - CRange(60, 90) + CRange(70, 80);
    assert(to_string(b) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}");
#ifdef EXTENDED_SYNTAX
    CRangeList x { { 5, 20 }, { 150, 200 }, { -9, 12 }, { 48, 93 } };
    assert(to_string(x) == "{<-9..20>,<48..93>,<150..200>}");
    std::ostringstream oss;
    oss << std::setfill('=') << std::hex << std::left;
    for (const auto & v : x + CRange(-100, -100)) {
        oss << v << std::endl;
    }
    oss << std::setw(10) << 1024;
    assert(oss.str() == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======");
#endif  /* EXTENDED_SYNTAX */
    return 0;
}
#endif  /* __PROGTEST__ */
