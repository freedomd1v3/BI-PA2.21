#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iterator>
#include <functional>

class CDate {
    private:
        int16_t m_Year;
        int8_t m_Month, m_Day;

    public:
        CDate(int y, int m, int d)
            : m_Year(y), m_Month(m), m_Day(d) { }

        int
        compare(const CDate & x) const {
            if (m_Year != x.m_Year) {
                return m_Year - x.m_Year;
            }
            if (m_Month != x.m_Month) {
                return m_Month - x.m_Month;
            }
            return m_Day - x.m_Day;
        }

        int
        year() const {
            return m_Year;
        }

        int
        month() const {
            return m_Month;
        }

        int
        day() const {
            return m_Day;
        }

        friend std::ostream & operator << (std::ostream & os,
                                           const CDate & x) {
            char oldFill = os.fill();
            return os << std::setfill('0') << std::setw(4) << x.m_Year << "-"
                      << std::setw(2) << static_cast<int> (x.m_Month) << "-"
                      << std::setw(2) << static_cast<int> (x.m_Day)
                      << std::setfill(oldFill);
        }
};
#endif  /* __PROGTEST__ */

// ---------------------------------------------------------------------------
/**
 * Transform the string for our needs (make them suitable for comparators):
 * trim redundant spaces; spaces from the beginning and end of the string;
 * make all letters lower. Thanks to Stack Overflow
 * @param s
 */
void
transform_string(std::string & s) {
    // Trim spaces from start
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) {
                return !std::isspace(c);
            }));

    // From end
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) {
                return !std::isspace(c);
            }).base(), s.end());

    // Redundant spaces
    auto new_end = std::unique(s.begin(), s.end(),
                               [=](char lhs, char rhs) {
                                return (lhs == rhs) && (lhs == ' ');
                               });
    s.erase(new_end, s.end());

    // To lower
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
class CInvoice {
    private:
        CDate m_Date;
        // Regular name and their transformed variants
        std::string m_Seller, m_Buyer, m_Seller_tf, m_Buyer_tf;
        unsigned int m_Amount;
        double m_Vat;
        // I've missed this for a while: this is our last resort
        // for sorting
        size_t m_Id;

    public:
        CInvoice(const CDate & date,
                 const std::string & seller, const std::string & buyer,
                 unsigned int amount, double vat)
            : m_Date(date),
              m_Seller(seller), m_Buyer(buyer),
              m_Amount(amount), m_Vat(vat) {
            m_Seller_tf = m_Seller, m_Buyer_tf = m_Buyer;
            transform_string(m_Seller_tf), transform_string(m_Buyer_tf);

            // I'd like to point this out: this is the default m_Id.
            // It should always be changed, when you add it to register
            m_Id = 0;
        }

        // Getters :(
        CDate date() const {
            return m_Date;
        }
        std::string seller() const {
            return m_Seller;
        }
        std::string buyer() const {
            return m_Buyer;
        }
        unsigned int amount() const {
            return m_Amount;
        }
        double vat() const {
            return m_Vat;
        }

        // This operator is overloaded for unordered_map
        // Please note that equalLists() should compare those differently
        bool operator == (const CInvoice & x) const {
            if (!m_Date.compare(x.m_Date)
                && m_Seller_tf == x.m_Seller_tf
                && m_Buyer_tf == x.m_Buyer_tf
                && m_Amount == x.m_Amount
                && m_Vat == x.m_Vat) {
                return true;
            }
            return false;
        }
        // For equalLists()
        bool operator != (const CInvoice & x) const {
            if (!m_Date.compare(x.m_Date)
                && m_Seller == x.m_Seller
                && m_Buyer == x.m_Buyer
                && m_Amount == x.m_Amount
                && m_Vat == x.m_Vat) {
                return false;
            }
            return true;
        }

        static const bool m_TYPE_ISSUED = false, m_TYPE_ACCEPTED = true;

        // More getters!!
        const std::string &
        get_seller_tf() const {
            return m_Seller_tf;
        }
        const std::string &
        get_buyer_tf() const {
            return m_Buyer_tf;
        }

        size_t
        get_id() const {
            return m_Id;
        }

        // And, well, some setters.
        // WARNING: Those methods are potentially unsafe!
        // They should be used ONLY to fix the seller's / buyer's
        // case or trim spaces! This function doesn't make any checks
        void
        set_seller(const std::string & seller) {
            m_Seller = seller;
        }
        void
        set_buyer(const std::string & buyer) {
            m_Buyer = buyer;
        }

        void
        set_id(const size_t id) {
            m_Id = id;
        }
};

/**
 * Hash function for CInvoice.
 * Thanks to Stack Overflow
 * @param x
 * @return hash of x
 */
struct
CInvoice_Hasher {
    size_t
    operator() (const CInvoice & x) const {
        return std::hash<std::string>()(std::to_string(x.date().year())
                                        + "_" + std::to_string(x.date().month())
                                        + "_" + std::to_string(x.date().day())
                                        + "_" + x.get_seller_tf()
                                        + "_" + x.get_buyer_tf()
                                        + "_" + std::to_string(x.amount())
                                        + "_" + std::to_string(x.vat()));
    }
};
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
class CSortOpt {
    public:
        static const int BY_DATE = 0,
                         BY_BUYER = 1,
                         BY_SELLER = 2,
                         BY_AMOUNT = 3,
                         BY_VAT = 4;

        std::vector<std::pair<int, bool>> m_Args;

        CSortOpt & addKey(int key, bool ascending = true) {
            if (!(key < BY_DATE || key > BY_VAT)) {
                m_Args.emplace_back(key, ascending);
            }
            return *this;
        }
};
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
class CCompany {
    private:
        std::string m_Name;

        static const int m_INV_TYPE_BOTH = CInvoice::m_TYPE_ACCEPTED + 1;
        std::unordered_map<CInvoice, int, CInvoice_Hasher> m_Invs;

    public:
        CCompany(const std::string & name)
            : m_Name(name) { }

        const std::string &
        get_name() const {
            return m_Name;
        }

        bool emplace(const CInvoice & x, const bool TYPE);
        bool try_erase(const CInvoice & x, const bool TYPE);

        std::list<CInvoice> get_unmatched() const;
};

/**
 * Try to emplace the provided CInvoice
 * @param x
 * @param TYPE
 * @return true, if emplaced;
 *      false otherwise
 */
bool
CCompany::emplace(const CInvoice & x, const bool TYPE) {
    auto found = m_Invs.find(x);
    if (found != m_Invs.end()
        && (found->second == m_INV_TYPE_BOTH || found->second == TYPE)) {
        return false;
    }

    if (found == m_Invs.end()) {
        m_Invs.emplace(x, TYPE);
    }
    else {
        found->second = m_INV_TYPE_BOTH;
    }
    return true;
}

/**
 * Try to erase the provided CInvoice
 * @param x
 * @param TYPE
 * @return true, if erased;
 *      false otherwise
 */
bool
CCompany::try_erase(const CInvoice & x, const bool TYPE) {
    auto found = m_Invs.find(x);
    if (found == m_Invs.end()
        || (found->second != m_INV_TYPE_BOTH && found->second != TYPE)) {
        return false;
    }

    if (found->second == m_INV_TYPE_BOTH) {
        found->second = !TYPE;
    }
    else {
        m_Invs.erase(found);
    }
    return true;
}


/**
 * Get a list of unmatched invoices
 * @return list of unmatches invoices
 */
std::list<CInvoice>
CCompany::get_unmatched() const {
    std::list<CInvoice> unmatched;
    for (const auto & it: m_Invs) {
        if (it.second != m_INV_TYPE_BOTH) {
            unmatched.push_back(it.first);
        }
    }
    return unmatched;
}
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
class CVATRegister {
    private:
        // Map's 1st element: transformed name of the company
        // (see transform_string())
        std::map<std::string, CCompany> m_Comps;

        bool add_invoice(const CInvoice & x, const bool TYPE);
        bool del_invoice(const CInvoice & x, const bool TYPE);

        size_t m_Next_unique_id;

    public:
        CVATRegister()
            : m_Next_unique_id(0) { }

        bool registerCompany(const std::string & name);

        bool addIssued(const CInvoice & x);
        bool addAccepted(const CInvoice & x);
        bool delIssued(const CInvoice & x);
        bool delAccepted(const CInvoice & x);

        std::list<CInvoice> unmatched(const std::string & company,
                                      const CSortOpt & sortBy) const;
};

bool
CVATRegister::registerCompany(const std::string & name) {
    std::string name_tf = name;
    transform_string(name_tf);

    auto emplace_res = m_Comps.emplace(name_tf, CCompany(name));
    if (!emplace_res.second) {
        return false;
    }
    return true;
}


bool
CVATRegister::add_invoice(const CInvoice & x, const bool TYPE) {
    if (x.get_seller_tf() == x.get_buyer_tf()) {
        // Seller and buyer are same
        return false;
    }

    auto seller_it = m_Comps.find(x.get_seller_tf());
    if (seller_it == m_Comps.end()) {
        // Seller is not registered
        return false;
    }
    auto buyer_it = m_Comps.find(x.get_buyer_tf());
    if (buyer_it == m_Comps.end()) {
        // Buyer is not registered
        return false;
    }

    // Set the CInvoice's namings to be exactly the same way
    // companies are registered
    CInvoice to_emplace = x;
    to_emplace.set_seller(seller_it->second.get_name());
    to_emplace.set_buyer(buyer_it->second.get_name());
    to_emplace.set_id(m_Next_unique_id++);
    
    // Invoice is already present in the sellers' database
    if (!m_Comps.at(x.get_seller_tf()).emplace(to_emplace, TYPE)
        || !m_Comps.at(x.get_buyer_tf()).emplace(to_emplace, TYPE)) {
        return false;
    }
    return true;
}
bool
CVATRegister::addIssued(const CInvoice & x) {
    return add_invoice(x, CInvoice::m_TYPE_ISSUED);
}
bool
CVATRegister::addAccepted(const CInvoice & x) {
    return add_invoice(x, CInvoice::m_TYPE_ACCEPTED);
}

bool
CVATRegister::del_invoice(const CInvoice & x, const bool TYPE) {
    if (// Seller or buyer is not registered
        // (just to avoid out-of-bounds or any other possible errors)
        !m_Comps.count(x.get_seller_tf())
        || !m_Comps.count(x.get_buyer_tf())

        // Couldn't delete the invoice
        || !m_Comps.at(x.get_seller_tf()).try_erase(x, TYPE)
        || !m_Comps.at(x.get_buyer_tf()).try_erase(x,TYPE)) {
        return false;
    }
    return true;
}
bool
CVATRegister::delIssued(const CInvoice & x) {
    return del_invoice(x, CInvoice::m_TYPE_ISSUED);
}
bool
CVATRegister::delAccepted(const CInvoice & x) {
    return del_invoice(x, CInvoice::m_TYPE_ACCEPTED);
}


// Comparator for CInvoice to sort list of unmatched invoices
// @param lhs
// @param rhs
// @param sortBy
// @return lhs < rhs (by arguments in sortBy)
struct CInvoice_Cmp {
    private:
        CSortOpt m_Sort_Args;

    public:
        CInvoice_Cmp(const CSortOpt & sort_by)
            : m_Sort_Args(sort_by) {
        }

        bool
        operator() (const CInvoice & lhs, const CInvoice & rhs) const;
};

bool
CInvoice_Cmp::operator() (const CInvoice & lhs, const CInvoice & rhs) const {
    for (const auto & x: m_Sort_Args.m_Args) {
        switch (x.first) {
            case CSortOpt::BY_DATE: {
                int date_compare_status = lhs.date().compare(rhs.date());
                if (!date_compare_status) {
                    continue;
                }

                if (x.second) {
                    return date_compare_status < 0;
                }
                else {
                    return date_compare_status > 0;
                }
            }
            case CSortOpt::BY_BUYER: {
                int buyer_compare_status = lhs.buyer().compare(rhs.buyer());
                if (!buyer_compare_status) {
                    continue;
                }

                if (x.second) {
                    return buyer_compare_status < 0;
                }
                else {
                    return buyer_compare_status > 0;
                }
            }
            case CSortOpt::BY_SELLER: {
                int seller_compare_status = lhs.seller().compare(rhs.seller());
                if (!seller_compare_status) {
                    continue;
                }

                // I have no idea why those are so weird
                if (x.second) {
                    return seller_compare_status > 0;
                }
                else {
                    return seller_compare_status < 0;
                }
            }
            case CSortOpt::BY_AMOUNT:
                if (lhs.amount() == rhs.amount()) {
                    continue;
                }

                if (x.second) {
                    return lhs.amount() < rhs.amount();
                }
                else {
                    return lhs.amount() > rhs.amount();
                }
            case CSortOpt::BY_VAT:
                // Oh god, I really hope I don't need to compare those
                // using all those DBL_EPSILON things
                if (lhs.vat() == rhs.vat()) {
                    continue;
                }

                if (x.second) {
                    return lhs.vat() < rhs.vat();
                }
                else {
                    return lhs.vat() > rhs.vat();
                }
        }
        // If we got here, then those are equal
        // and should NOT be compared by id
        // return false;
    }

    return lhs.get_id() < rhs.get_id();
}

std::list<CInvoice>
CVATRegister::unmatched(const std::string & company,
                        const CSortOpt & sortBy) const {
    std::string name_tf = company;
    transform_string(name_tf);

    std::list<CInvoice> to_ret;
    auto comp = m_Comps.find(name_tf);
    if (comp == m_Comps.end()) {
        // To avoid any possible errors
        return to_ret;
    }
    to_ret = comp->second.get_unmatched();
    to_ret.sort(CInvoice_Cmp(sortBy));
    return to_ret;
}
// ---------------------------------------------------------------------------

#ifndef __PROGTEST__
bool
equalLists(const std::list<CInvoice> & a, const std::list<CInvoice> & b) {
    if (a.size() != b.size()) {
        return false;
    }
    auto a_it = a.begin(), b_it = b.begin();
    for (size_t i = 0; i < a.size(); i++) {
        if (*a_it++ != *b_it++) {
            return false;
        }
    }
    return true;
}

int
main() {
    CVATRegister r;
    assert(r.registerCompany("first Company"));
    assert(r.registerCompany("Second     Company"));
    assert(r.registerCompany("ThirdCompany, Ltd."));
    assert(r.registerCompany("Third Company, Ltd."));
    assert(!r.registerCompany("Third Company, Ltd."));
    assert(!r.registerCompany(" Third  Company,  Ltd.  "));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 20)));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 2), "FirSt Company", "Second Company ", 200, 30)));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 30)));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "Second Company ", "First Company", 300, 30)));
    assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "Third  Company,  Ltd.", "  Second    COMPANY ", 400, 34)));
    assert(!r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
    assert(!r.addIssued(CInvoice(CDate(2000, 1, 4), "First Company", "First   Company", 200, 30)));
    assert(!r.addIssued(CInvoice(CDate(2000, 1, 4), "Another Company", "First   Company", 200, 30)));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, false).addKey(CSortOpt::BY_DATE, false)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)
                      }));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000)
                      }));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_VAT, true).addKey(CSortOpt::BY_AMOUNT, true).addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)
                      }));
    assert(equalLists(r.unmatched("First Company", CSortOpt()),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)
                      }));
    assert(equalLists(r.unmatched("second company", CSortOpt().addKey(CSortOpt::BY_DATE, false)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Third Company, Ltd.", "Second     Company", 400, 34.000000)
                      }));
    assert(equalLists(r.unmatched("last company", CSortOpt () . addKey ( CSortOpt::BY_VAT, true)),
                      std::list<CInvoice> {
                      }));
    assert(r.addAccepted(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company ", 200, 30)));
    assert(r.addAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert(r.addAccepted(CInvoice(CDate(2000, 1, 1), "Second company ", "First Company", 300, 32)));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)
                      }));
    assert(!r.delIssued(CInvoice(CDate(2001, 1, 1), "First Company", "Second Company ", 200, 30)));
    assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "A First Company", "Second Company ", 200, 30)));
    assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Hand", 200, 30)));
    assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 1200, 30)));
    assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 200, 130)));
    assert(r.delIssued(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company", 200, 30)));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)
                      }));
    assert(r.delAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)
                      }));
    assert(r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                      std::list<CInvoice> {
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)
                      }));

    std::cout << "You passed." << std::endl;
    return 0;
}
#endif  /* __PROGTEST__ */
