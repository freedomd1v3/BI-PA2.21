#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream> 
#include <iomanip> 
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#endif  /* __PROGTEST__ */

class CPersonalAgenda {
    struct Emp_Names {
        std::string m_sn, m_n;
        unsigned m_sal;
        size_t m_EMP_ID;
    };

    struct Emp_Em {
        std::string m_em;
        unsigned m_sal;
        size_t m_EMP_ID;
    };

    std::vector<Emp_Names> m_db_names;
    std::vector<Emp_Em> m_db_ems;
    // Thanks to GeeksForGeeks for showing how to use
    // std::lower_bound() and std::upper_bound() :)
    std::vector<unsigned> m_db_sals;
    size_t next_unique_id = 0;

    /**
     * Find an employee with the provided surname and name.
     * If found, "out" will contain the index of the employee with
     * the provided name and surname.
     * It will otherwise contain the index of the best place
     * to insert new employee
     * @param surname
     * @param name
     * @param out
     * @return true, if found;
     *      false otherwise
     */
    bool
    find_employee(const std::string & surname, const std::string & name,
                  size_t & out) const {
        size_t lo = 0, hi = m_db_names.size();
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (m_db_names.at(mid).m_sn < surname) {
                lo = mid + 1;
            }
            else if (m_db_names.at(mid).m_sn > surname) {
                hi = mid;
            }
            else {
                if (m_db_names.at(mid).m_n < name) {
                    lo = mid + 1;
                }
                else if (m_db_names.at(mid).m_n > name) {
                    hi = mid;
                }
                else {
                    out = mid;
                    return true;
                }
            }
        }
        // Didn't find. lo is the closest result and therefore
        // is the best place to insert new employee
        out = lo;
        return false;
    }

    /**
     * Try to find the provided email. If not found, "out" will contain
     * index of employee with the closest email
     * @param email
     * @param out
     * @return true, if found;
     *      false otherwise
     */
    bool
    find_email(const std::string & email, size_t & out) const {
        size_t lo = 0, hi = m_db_ems.size();
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (m_db_ems.at(mid).m_em < email) {
                lo = mid + 1;
            }
            else if (m_db_ems.at(mid).m_em > email) {
                hi = mid;
            }
            else {
                out = mid;
                return true;
            }
        }
        // Didn't find. lo is the closest email and therefore
        // is the best place to insert new email record 
        out = lo;
        return false;
    }

    /**
     * Continue the getRank() method routine
     * @param salary
     * @param rankMin
     * @param rankMax
     */
    void
    getRank_continuation(unsigned & salary,
                         int & rankMin, int & rankMax) const {
        rankMin = static_cast<int> (std::lower_bound(m_db_sals.begin(), m_db_sals.end(), salary)
                                    - m_db_sals.begin());

        rankMax = static_cast<int> (std::upper_bound(m_db_sals.begin(), m_db_sals.end(), salary)
                                    - m_db_sals.begin());
        if (rankMax) {
            rankMax--;
        }
    }


    public:
        bool
        add(const std::string & name, const std::string & surname,
            const std::string & email, unsigned salary) {
            size_t insert_names, insert_email;
            if (find_employee(surname, name, insert_names)
                || find_email(email, insert_email)) {
                return false;
            }

            m_db_names.insert(m_db_names.begin() + insert_names,
                              Emp_Names { surname, name, salary, next_unique_id });
            m_db_ems.insert(m_db_ems.begin() + insert_email,
                            Emp_Em { email, salary, next_unique_id++ });
            m_db_sals.insert(std::lower_bound(m_db_sals.begin(), m_db_sals.end(), salary),
                             salary);
            return true;
        }

        bool
        del(const std::string & name, const std::string & surname) {
            size_t to_remove;
            if (!find_employee(surname, name, to_remove)) {
                return false;
            }

            for (size_t i = 0; i < m_db_names.size(); i++) {
                if (m_db_ems.at(i).m_EMP_ID == m_db_names.at(to_remove).m_EMP_ID) {
                    m_db_ems.erase(m_db_ems.begin() + i);
                    break;
                }
            }
            m_db_sals.erase(std::lower_bound(m_db_sals.begin(), m_db_sals.end(), m_db_names.at(to_remove).m_sal));
            m_db_names.erase(m_db_names.begin() + to_remove);
            return true;
        }
        bool
        del(const std::string & email) {
            size_t to_remove;
            if (!find_email(email, to_remove)) {
                return false;
            }
            
            for (size_t i = 0; i < m_db_ems.size(); i++) {
                if (m_db_names.at(i).m_EMP_ID == m_db_ems.at(to_remove).m_EMP_ID) {
                    m_db_names.erase(m_db_names.begin() + i);
                    break;
                }
            }
            m_db_sals.erase(std::lower_bound(m_db_sals.begin(), m_db_sals.end(), m_db_ems.at(to_remove).m_sal));
            m_db_ems.erase(m_db_ems.begin() + to_remove);
            return true;
        }

        bool
        changeName(const std::string & email,
                   const std::string & newName, const std::string & newSurname) {
            size_t to_change, where_to_move;
            if (!find_email(email, to_change)
                || find_employee(newSurname, newName, where_to_move)) {
                return false;
            }

            for (size_t i = 0; i < m_db_names.size(); i++) {
                if (m_db_names.at(i).m_EMP_ID == m_db_ems.at(to_change).m_EMP_ID) {
                    m_db_names.erase(m_db_names.begin() + i);
                    m_db_names.insert(m_db_names.begin() + where_to_move,
                                      Emp_Names { newSurname,
                                                  newName,
                                                  m_db_ems.at(to_change).m_sal,
                                                  m_db_ems.at(to_change).m_EMP_ID });
                    break;
                }
            }
            return true;
        }

        bool
        changeEmail(const std::string & name, const std::string & surname,
                    const std::string & newEmail) {
            size_t to_change, where_to_move;
            if (!find_employee(surname, name, to_change)
                || find_email(newEmail, where_to_move)) {
                return false;
            }

            for (size_t i = 0; i < m_db_ems.size(); i++) {
                if (m_db_ems.at(i).m_EMP_ID == m_db_names.at(to_change).m_EMP_ID) {
                    m_db_ems.erase(m_db_ems.begin() + i);
                    m_db_ems.insert(m_db_ems.begin() + where_to_move,
                                    Emp_Em { newEmail,
                                             m_db_names.at(to_change).m_sal,
                                             m_db_names.at(to_change).m_EMP_ID });
                    break;
                }
            }
            return true;
        }

        bool
        setSalary(const std::string & name, const std::string & surname,
                  unsigned salary) {
            size_t to_change;
            if (!find_employee(surname, name, to_change)) {
                return false;
            }
            
            bool email_salary_changed = false, salary_changed = false;
            for (size_t i = 0; i < m_db_names.size(); i++) {
                if (m_db_ems.at(i).m_EMP_ID == m_db_names.at(to_change).m_EMP_ID) {
                    m_db_ems.at(i).m_sal = salary;
                    email_salary_changed = true;
                }
                if (m_db_sals.at(i) == m_db_names.at(to_change).m_sal) {
                    m_db_sals.erase(m_db_sals.begin() + i);
                    m_db_sals.insert(std::lower_bound(m_db_sals.begin(), m_db_sals.end(), salary),
                                     salary);
                    salary_changed = true;
                }
                if (email_salary_changed && salary_changed) {
                    break;
                }
            }
            m_db_names.at(to_change).m_sal = salary;
            return true;
        }
        bool
        setSalary(const std::string & email, unsigned salary) {
            size_t to_change;
            if (!find_email(email, to_change)) {
                return false;
            }

            bool names_salary_changed = false, salary_changed = false;
            for (size_t i = 0; i < m_db_ems.size(); i++) {
                if (m_db_names.at(i).m_EMP_ID == m_db_ems.at(to_change).m_EMP_ID) {
                    m_db_names.at(i).m_sal = salary;
                    names_salary_changed = true;
                }
                if (m_db_sals.at(i) == m_db_ems.at(to_change).m_sal) {
                    m_db_sals.erase(m_db_sals.begin() + i);
                    m_db_sals.insert(std::lower_bound(m_db_sals.begin(), m_db_sals.end(), salary),
                                     salary);
                    salary_changed = true;
                }
                if (names_salary_changed && salary_changed) {
                    break;
                }
            }
            m_db_ems.at(to_change).m_sal = salary;
            return true;
        }

        unsigned
        getSalary(const std::string & name,
                  const std::string & surname) const {
            size_t to_ret;
            if (!find_employee(surname, name, to_ret)) {
                return 0;
            }
            return m_db_names.at(to_ret).m_sal;
        }
        unsigned
        getSalary(const std::string & email) const {
            size_t to_ret;
            if (!find_email(email, to_ret)) {
                return 0;
            }
            return m_db_ems.at(to_ret).m_sal;
        }
    
        bool
        getRank(const std::string & name, const std::string & surname,
                int & rankMin, int & rankMax) const {
            unsigned salary = getSalary(name, surname);
            if (!salary) {
                return false;
            }

            getRank_continuation(salary, rankMin, rankMax);
            return true;
        }
        bool
        getRank(const std::string & email,
                int & rankMin, int & rankMax) const {
            unsigned salary = getSalary(email);
            if (!salary) {
                return false;
            }

            getRank_continuation(salary, rankMin, rankMax);
            return true;
        }

        bool
        getFirst(std::string & outName, std::string & outSurname) const {
            if (!m_db_names.size()) {
                return false;
            }

            outSurname = m_db_names.front().m_sn;
            outName = m_db_names.front().m_n;
            return true;
        }

        bool
        getNext(const std::string & name, const std::string & surname,
                std::string & outName, std::string & outSurname) const {
            if (m_db_names.size() < 2) {
                return false;
            }

            size_t index;
            if (!find_employee(surname, name, index)
                || !(index + 1 < m_db_names.size())) {
                return false;
            }
            outSurname = m_db_names.at(index + 1).m_sn;
            outName = m_db_names.at(index + 1).m_n;
            return true;
        }
};

#ifndef __PROGTEST__
int
main() {
    std::string outName, outSurname;
    int lo, hi;

    CPersonalAgenda b1;
    assert(b1.add("John", "Smith", "john", 30000));
    assert(b1.add("John", "Miller", "johnm", 35000));
    assert(b1.add("Peter", "Smith", "peter", 23000));
    assert(b1.getFirst(outName, outSurname)
           && outName == "John"
           && outSurname == "Miller");
    assert(b1.getNext("John", "Miller", outName, outSurname)
           && outName == "John"
           && outSurname == "Smith");
    assert(b1.getNext("John", "Smith", outName, outSurname)
           && outName == "Peter"
           && outSurname == "Smith");
    assert(!b1.getNext("Peter", "Smith", outName, outSurname));
    assert(b1.setSalary("john", 32000));
    assert(b1.getSalary("john") ==  32000);
    assert(b1.getSalary("John", "Smith") ==  32000);
    assert(b1.getRank("John", "Smith", lo, hi)
           && lo == 1
           && hi == 1);
    assert(b1.getRank("john", lo, hi)
           && lo == 1
           && hi == 1);
    assert(b1.getRank("peter", lo, hi)
           && lo == 0
           && hi == 0);
    assert(b1.getRank("johnm", lo, hi)
           && lo == 2
           && hi == 2);
    assert(b1.setSalary("John", "Smith", 35000));
    assert(b1.getSalary("John", "Smith") ==  35000);
    assert(b1.getSalary("john") ==  35000);
    assert(b1.getRank("John", "Smith", lo, hi) 
           && lo == 1
           && hi == 2);
    assert(b1.getRank("john", lo, hi)
           && lo == 1
           && hi == 2);
    assert(b1.getRank("peter", lo, hi)
           && lo == 0
           && hi == 0);
    assert(b1.getRank("johnm", lo, hi)
           && lo == 1
           && hi == 2);
    assert(b1.changeName("peter", "James", "Bond"));
    assert(b1.getSalary("peter") ==  23000);
    assert(b1.getSalary("James", "Bond") ==  23000);
    assert(b1.getSalary("Peter", "Smith") ==  0);
    assert(b1.getFirst(outName, outSurname)
           && outName == "James"
           && outSurname == "Bond");
    assert(b1.getNext("James", "Bond", outName, outSurname)
           && outName == "John"
           && outSurname == "Miller");
    assert(b1.getNext("John", "Miller", outName, outSurname)
           && outName == "John"
           && outSurname == "Smith");
    assert(!b1.getNext("John", "Smith", outName, outSurname));
    assert(b1.changeEmail("James", "Bond", "james"));
    assert(b1.getSalary("James", "Bond") ==  23000);
    assert(b1.getSalary("james") ==  23000);
    assert(b1.getSalary("peter") ==  0);
    assert(b1.del("james"));
    assert(b1.getRank("john", lo, hi)
           && lo == 0
           && hi == 1);
    assert(b1.del("John", "Miller"));
    assert(b1.getRank("john", lo, hi)
           && lo == 0
           && hi == 0);
    assert(b1.getFirst(outName, outSurname)
           && outName == "John"
           && outSurname == "Smith");
    assert(!b1.getNext("John", "Smith", outName, outSurname));
    assert(b1.del("john"));
    assert(!b1.getFirst(outName, outSurname));
    assert(b1.add("John", "Smith", "john", 31000));
    assert(b1.add("john", "Smith", "joHn", 31000));
    assert(b1.add("John", "smith", "jOhn", 31000));

    CPersonalAgenda b2;
    assert(!b2.getFirst(outName, outSurname));
    assert(b2.add("James", "Bond", "james", 70000));
    assert(b2.add("James", "Smith", "james2", 30000));
    assert(b2.add("Peter", "Smith", "peter", 40000));
    assert(!b2.add("James", "Bond", "james3", 60000));
    assert(!b2.add("Peter", "Bond", "peter", 50000));
    assert(!b2.changeName("joe", "Joe", "Black"));
    assert(!b2.changeEmail("Joe", "Black", "joe"));
    assert(!b2.setSalary("Joe", "Black", 90000));
    assert(!b2.setSalary("joe", 90000));
    assert(b2.getSalary("Joe", "Black") ==  0);
    assert(b2.getSalary("joe") ==  0);
    assert(!b2.getRank("Joe", "Black", lo, hi));
    assert(!b2.getRank("joe", lo, hi));
    assert(!b2.changeName("joe", "Joe", "Black"));
    assert(!b2.changeEmail("Joe", "Black", "joe"));
    assert(!b2.del("Joe", "Black"));
    assert(!b2.del("joe")); 
    assert(!b2.changeName("james2", "James", "Bond"));
    assert(!b2.changeEmail("Peter", "Smith", "james"));
    assert(!b2.changeName("peter", "Peter", "Smith"));
    assert(!b2.changeEmail("Peter", "Smith", "peter"));
    assert(b2.del("Peter", "Smith"));
    assert(!b2.changeEmail("Peter", "Smith", "peter2"));
    assert(!b2.setSalary("Peter", "Smith", 35000));
    assert(b2.getSalary("Peter", "Smith") ==  0);
    assert(!b2.getRank("Peter", "Smith", lo, hi));
    assert(!b2.changeName("peter", "Peter", "Falcon"));
    assert(!b2.setSalary("peter", 37000));
    assert(b2.getSalary("peter") ==  0);
    assert(!b2.getRank("peter", lo, hi));
    assert(!b2.del("Peter", "Smith"));
    assert(!b2.del("peter"));
    assert(b2.add("Peter", "Smith", "peter", 40000));
    assert(b2.getSalary("peter") ==  40000);

    std::cout << "Yay! You passed!!" << std::endl;
    return 0;
}
#endif  /* __PROGTEST__ */
