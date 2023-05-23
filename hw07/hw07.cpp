#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>
#endif  /* __PROGTEST__ */

template <typename M_>
class CContest {
    private:
        std::map<std::pair<std::string, std::string>, M_> m_Matches;

    public:
        /**
         * Get all the contestants beaten by this contestant
         * @param contestant
         * @param Comp
         * @return Beaten contestants
         */
        template <class Compare>
        auto bfs_find_all_beaten(const std::string & contestant,
                                 Compare comp) const {
            // TODO: Make this method private
            std::queue<std::string> to_check;
            to_check.push(contestant);

            // It makes more sense to use vector instead of set here:
            // first of all, we want the container to be sequence;
            // secondly, adding data to both "to_check" queue and our list
            // at the same is easier (see lines 73 and 78)
            std::vector<std::string> beaten;
            while (!to_check.empty()) {
                const std::string & node = to_check.front();
                for (const auto & x: m_Matches) {
                    if ((x.first.first == node && comp(x.second) > 0)
                        || (x.first.second == node && comp(x.second) < 0)) {
                        // Insert the contestant to the beaten list and
                        // to queue to check, if it's not checked already
                        // TODO: Second check may be slow!
                        // You can at the very least rewrite this
                        // to the previous condition, or, even better,
                        // make some another structure like std::set,
                        // where you will save checked elements
                        if (x.first.first == node
                            && x.first.second != contestant
                            && !std::count(beaten.begin(), beaten.end(), x.first.second)) {
                            beaten.push_back(x.first.second);
                            to_check.push(x.first.second);
                        }
                        else if (x.first.second == node
                                 && x.first.first != contestant
                                 && !std::count(beaten.begin(), beaten.end(), x.first.first)) {
                            beaten.push_back(x.first.first);
                            to_check.push(x.first.first);
                        }
                    }
                }
                to_check.pop();
            }
            return beaten;
        }

        CContest & addMatch(const std::string & contestant1,
                            const std::string & contestant2,
                            const M_ & result) {
            if (m_Matches.find(std::make_pair(contestant1, contestant2))
                != m_Matches.end()
                || m_Matches.find(std::make_pair(contestant2, contestant1))
                   != m_Matches.end()) {
                throw std::logic_error("CContestant::addMatch(): Duplicate match");
            }
            m_Matches.emplace(std::make_pair(contestant1, contestant2), result);
            return *this;
        }

        template <class Compare>
        bool isOrdered(Compare comp) const {
            std::map<std::string, std::vector<std::string>> contestants_wins;
            for (const auto & x: m_Matches) {
                if (contestants_wins.find(x.first.first) == contestants_wins.end()) {
                    contestants_wins.emplace(x.first.first,
                                             bfs_find_all_beaten(x.first.first,
                                                                 comp));
                }
                if (contestants_wins.find(x.first.second) == contestants_wins.end()) {
                    contestants_wins.emplace(x.first.second,
                                             bfs_find_all_beaten(x.first.second,
                                                                 comp));
                }
            }

            std::list<std::string> results;
            // Adding contestants to the results
            for (const auto & x: contestants_wins) {
                size_t minimum_id = results.size();
                // Finding their place to insert in results
                for (const auto & y: x.second) {
                    // Finding the ID of the beaten element in results
                    if (std::count(results.begin(), results.end(), y)) {
                        size_t i = 0;
                        for (const auto & z: results) {
                            if (z == y) {
                                break;
                            }
                            i++;
                        }
                        if (minimum_id > i) {
                            minimum_id = i;
                        }
                    }
                }
                results.insert(std::next(results.begin(), minimum_id), x.first);
            }

            // Check if an element beats all the elements after it
            for (size_t i = 0; i < results.size(); i++) {
                for (size_t j = i + 1; j < results.size(); j++) {
                    auto to_find = std::next(results.begin(), j);
                    bool found = false;

                    // Optimization
                    auto this_contestant = std::next(results.begin(), i);
                    for (const auto & x: contestants_wins.at(*this_contestant)) {
                        if (x == *to_find) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        return false;
                    }
                }
            }
            // Check, if an element didn't beat any elements before it
            for (size_t i = 0; i < results.size(); i++) {
                for (size_t j = 0; j < i; j++) {
                    auto to_find = std::next(results.begin(), j);
                    bool found = false;

                    auto this_contestant = std::next(results.begin(), i);
                    for (const auto & x: contestants_wins.at(*this_contestant)) {
                        if (x == *to_find) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        return false;
                    }
                }
            }

            return true;
        }

        template <class Compare>
        std::list<std::string> results(Compare comp) const {
            std::map<std::string, std::vector<std::string>> contestants_wins;
            for (const auto & x: m_Matches) {
                if (contestants_wins.find(x.first.first) == contestants_wins.end()) {
                    contestants_wins.emplace(x.first.first,
                                             bfs_find_all_beaten(x.first.first,
                                                                 comp));
                }
                if (contestants_wins.find(x.first.second) == contestants_wins.end()) {
                    contestants_wins.emplace(x.first.second,
                                             bfs_find_all_beaten(x.first.second,
                                                                 comp));
                }
            }

            std::list<std::string> results;
            // Adding contestants to the results
            for (const auto & x: contestants_wins) {
                size_t minimum_id = results.size();
                // Finding their place to insert in results
                for (const auto & y: x.second) {
                    // Finding the ID of the beaten element in results
                    if (std::count(results.begin(), results.end(), y)) {
                        size_t i = 0;
                        for (const auto & z: results) {
                            if (z == y) {
                                break;
                            }
                            i++;
                        }
                        if (minimum_id > i) {
                            minimum_id = i;
                        }
                    }
                }
                results.insert(std::next(results.begin(), minimum_id), x.first);
            }

            // Check if an element beats all the elements after it
            for (size_t i = 0; i < results.size(); i++) {
                for (size_t j = i + 1; j < results.size(); j++) {
                    auto to_find = std::next(results.begin(), j);
                    bool found = false;

                    // Optimization
                    auto this_contestant = std::next(results.begin(), i);
                    for (const auto & x: contestants_wins.at(*this_contestant)) {
                        if (x == *to_find) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        throw std::logic_error("CContest::results(): Couldn't make list");
                    }
                }
            }
            // Check, if an element didn't beat any elements before it
            for (size_t i = 0; i < results.size(); i++) {
                for (size_t j = 0; j < i; j++) {
                    auto to_find = std::next(results.begin(), j);
                    bool found = false;

                    auto this_contestant = std::next(results.begin(), i);
                    for (const auto & x: contestants_wins.at(*this_contestant)) {
                        if (x == *to_find) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        throw std::logic_error("CContest::results(): Couldn't make list");
                    }
                }
            }

            return results;
        }
};

#ifndef __PROGTEST__
struct CMatch {
    public:
        CMatch(int a, int b)
            : m_A(a), m_B(b) { }

        int m_A, m_B;
};

class HigherScoreThreshold {
    private:
        int m_DiffAtLeast;

    public:
        HigherScoreThreshold(int diffAtLeast)
            : m_DiffAtLeast(diffAtLeast) { }

        int operator () (const CMatch & x) const {
            return (x.m_A > x.m_B + m_DiffAtLeast) - (x.m_B > x.m_A + m_DiffAtLeast);
        }
};

int HigherScore(const CMatch & x) {
    return (x.m_A > x.m_B) - (x.m_B > x.m_A);
}

int main() {
    double got_points = 5.00;

    {
        CContest<CMatch> addMatch_test;

        addMatch_test.addMatch("C++", "Pascal", CMatch(10, 3))
                     .addMatch("C++", "Java", CMatch(8, 1))
                     .addMatch("Pascal", "Basic", CMatch(40, 0))
                     .addMatch("Java", "PHP", CMatch(6, 2))
                     .addMatch("Java", "Pascal", CMatch(7, 3))
                     .addMatch("PHP", "Basic", CMatch(10, 0));

        try {
            addMatch_test.addMatch("Basic", "PHP", CMatch(4, 7));
            assert("Exception missing!" == nullptr);
        }
        catch(const std::logic_error & e) { }

        try {
            addMatch_test.addMatch("PHP", "Pascal", CMatch(3, 6));
        }
        catch(...) {
            assert("Unexpected exception!" == nullptr);
        }
    }

    CContest<CMatch> x;

    x.addMatch("C++", "Pascal", CMatch(10, 3))
     .addMatch("C++", "Java", CMatch(8, 1))
     .addMatch("Pascal", "Basic", CMatch(40, 0))
     .addMatch("Java", "PHP", CMatch(6, 2))
     .addMatch("Java", "Pascal", CMatch(7, 3))
     .addMatch("PHP", "Basic", CMatch(10, 0));

    assert(!x.isOrdered(HigherScore));
    try {
        std::list<std::string> res = x.results(HigherScore);
        assert("Exception missing!" == nullptr);
    }
    catch(const std::logic_error & e) { }
    catch(...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    x.addMatch("PHP", "Pascal", CMatch(3, 6)); 

    assert(x.isOrdered(HigherScore));
    try {
        std::list<std::string> res = x.results(HigherScore);
        assert((res == std::list<std::string>{ "C++", "Java", "Pascal", "PHP", "Basic" }));
    }
    catch(...) {
        assert("Unexpected exception!" == nullptr);
    }


    assert(!x.isOrdered(HigherScoreThreshold(3)));
    try {
        std::list<std::string> res = x.results(HigherScoreThreshold(3));
        assert("Exception missing!" == nullptr);
    }
    catch(const std::logic_error & e) { }
    catch(...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    assert(x.isOrdered([] (const CMatch & x) {
         return(x.m_A < x.m_B) - (x.m_B < x.m_A); 
    }));
    try {
        std::list<std::string> res = x.results([] (const CMatch & x) {
            return(x.m_A < x.m_B) - (x.m_B < x.m_A); 
        });
        assert((res == std::list<std::string>{ "Basic", "PHP", "Pascal", "Java", "C++" }));
    }
    catch(...) {
        assert("Unexpected exception!" == nullptr);
    }


    CContest<bool> y;

    y.addMatch("Python", "PHP", true)
     .addMatch("PHP", "Perl", true)
     .addMatch("Perl", "Bash", true)
     .addMatch("Bash", "JavaScript", true)
     .addMatch("JavaScript", "VBScript", true);

    assert(y.isOrdered([] (bool v) {
        return v ? 10 : -10;
    }));
    try {
        std::list<std::string> res = y.results([] (bool v) {
            return v ? 10 : -10;
        });
        assert((res == std::list<std::string>{ "Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript" }));
    }
    catch(...) {
        assert("Unexpected exception!" == nullptr);
    }

    y.addMatch("PHP", "JavaScript", false);
    assert(!y.isOrdered([] (bool v) {
        return v ? 10 : -10;
    }));
    try {
        std::list<std::string> res = y.results([] (bool v) {
            return v ? 10 : -10;
        });
        assert("Exception missing!" == nullptr);
    }
    catch(const std::logic_error & e) { }
    catch(...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    try {
        y.addMatch("PHP", "JavaScript", false);
        assert("Exception missing!" == nullptr);
    }
    catch(const std::logic_error & e) { }
    catch(...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    try {
        y.addMatch("JavaScript", "PHP", true);
        assert("Exception missing!" == nullptr);
    }
    catch(const std::logic_error & e) { }
    catch(...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    std::cout << "You passed." << std::endl;
    std::cout << "You gain " << got_points << " points." << std::endl;
    return 0;
}
#endif  /* __PROGTEST__ */
