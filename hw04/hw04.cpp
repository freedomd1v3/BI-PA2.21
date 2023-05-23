#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#endif  /* __PROGTEST__ */

// ---------------------------------------------------------------------------
class CMail {
    char * m_From, * m_To, * m_Body;

    public:
        explicit CMail(const char * from, const char * to, const char * body);
        CMail(const CMail & src);
        CMail & operator = (const CMail & src);
        ~CMail();

        bool operator == (const CMail & x) const;

        friend std::ostream & operator << (std::ostream & os, const CMail & m);

        // Oh yes, getters!
        const char * get_from() const;
        const char * get_to() const;
        const char * get_body() const;
};
CMail::CMail(const char * from, const char * to, const char * body) {
    const size_t FROM_SIZE = strlen(from), TO_SIZE = strlen(to),
                 BODY_SIZE = strlen(body);
    m_From = new char[FROM_SIZE + 1], m_To = new char[TO_SIZE + 1],
    m_Body = new char[BODY_SIZE + 1];
    memcpy(m_From, from, FROM_SIZE + 1);
    memcpy(m_To, to, TO_SIZE + 1);
    memcpy(m_Body, body, BODY_SIZE + 1);
}
CMail::CMail(const CMail & src) {
    const size_t FROM_SIZE = strlen(src.m_From), TO_SIZE = strlen(src.m_To),
                 BODY_SIZE = strlen(src.m_Body);
    m_From = new char[FROM_SIZE + 1], m_To = new char[TO_SIZE + 1],
    m_Body = new char[BODY_SIZE + 1];
    memcpy(m_From, src.m_From, FROM_SIZE + 1);
    memcpy(m_To, src.m_To, TO_SIZE + 1);
    memcpy(m_Body, src.m_Body, BODY_SIZE + 1);
}
CMail &
CMail::operator = (const CMail & src) {
    if (&src == this) {
        return *this;
    }

    delete [] m_From;
    delete [] m_To;
    delete [] m_Body;

    const size_t FROM_SIZE = strlen(src.m_From), TO_SIZE = strlen(src.m_To),
                 BODY_SIZE = strlen(src.m_Body);
    m_From = new char[FROM_SIZE + 1], m_To = new char[TO_SIZE + 1],
    m_Body = new char[BODY_SIZE + 1];
    memcpy(m_From, src.m_From, FROM_SIZE + 1);
    memcpy(m_To, src.m_To, TO_SIZE + 1);
    memcpy(m_Body, src.m_Body, BODY_SIZE + 1);
    return *this;
}
CMail::~CMail() {
    delete [] m_From;
    delete [] m_To;
    delete [] m_Body;
    m_From = m_To = m_Body = nullptr;
}

bool
CMail::operator == (const CMail & x) const {
    return !strcmp(m_From, x.m_From) && !strcmp(m_To, x.m_To)
           && !strcmp(m_Body, x.m_Body);
}

std::ostream &
operator << (std::ostream & os, const CMail & m) {
    os << "From: " << m.m_From
       << ", To: " << m.m_To
       << ", Body: " << m.m_Body;
    return os;
}

const char *
CMail::get_from() const {
    return m_From;
}
const char *
CMail::get_to() const {
    return m_To;
}
const char *
CMail::get_body() const {
    return m_Body;
}
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
class CMailBox {
    char * m_Addr;
    CMail ** m_Out, ** m_In;
    size_t m_Out_size, m_In_size;

    public:
        explicit CMailBox(const char * mail_address);
        CMailBox(const CMailBox & src);
        CMailBox & operator = (const CMailBox & src);
        ~CMailBox();

        bool add_out(const CMail & out);
        bool add_in(const CMail & in);

        // Even more getters! :)
        const char * get_addr() const;
        const CMail & get_out(const size_t idx) const;
        const CMail & get_in(const size_t idx) const;
        const size_t get_out_size() const;
        const size_t get_in_size() const;
};
CMailBox::CMailBox(const char * mail_address)
    : m_Out(nullptr), m_In(nullptr), m_Out_size(0), m_In_size(0) {
    const size_t ADDR_SIZE = strlen(mail_address);
    m_Addr = new char[ADDR_SIZE + 1];
    memcpy(m_Addr, mail_address, ADDR_SIZE + 1);
}
CMailBox::CMailBox(const CMailBox & src)
    : m_Out_size(src.m_Out_size), m_In_size(src.m_In_size) {
    const size_t ADDR_SIZE = strlen(src.m_Addr);
    m_Addr = new char[ADDR_SIZE + 1];
    memcpy(m_Addr, src.m_Addr, ADDR_SIZE + 1);

    m_Out = m_Out_size ? new CMail *[m_Out_size] : nullptr;
    m_In = m_In_size ? new CMail *[m_In_size] : nullptr;
    const size_t MAX = (m_In_size < m_Out_size) ? m_Out_size : m_In_size;
    for (size_t i = 0; i < MAX; i++) {
        if (i < m_Out_size) {
            m_Out[i] = new CMail(*(src.m_Out[i]));
        }
        if (i < m_In_size) {
            m_In[i] = new CMail(*(src.m_In[i]));
        }
    }
}
CMailBox &
CMailBox::operator = (const CMailBox & src) {
    if (&src == this) {
        return *this;
    }

    delete [] m_Addr;
    size_t MAX = (m_In_size < m_Out_size) ? m_Out_size : m_In_size;
    for (size_t i = 0; i < MAX; i++) {
        if (i < m_Out_size) {
            delete m_Out[i];
        }
        if (i < m_In_size) {
            delete m_In[i];
        }
    }
    delete [] m_Out;
    delete [] m_In;

    const size_t ADDR_SIZE = strlen(src.m_Addr);
    m_Addr = new char[ADDR_SIZE + 1];
    memcpy(m_Addr, src.m_Addr, ADDR_SIZE + 1);

    m_Out_size = src.m_Out_size, m_In_size = src.m_In_size;
    m_Out = m_Out_size ? new CMail *[m_Out_size] : nullptr;
    m_In = m_In_size ? new CMail *[m_In_size] : nullptr;
    MAX = (m_In_size < m_Out_size) ? m_Out_size : m_In_size;
    for (size_t i = 0; i < MAX; i++) {
        if (i < m_Out_size) {
            m_Out[i] = new CMail(*(src.m_Out[i]));
        }
        if (i < m_In_size) {
            m_In[i] = new CMail(*(src.m_In[i]));
        }
    }
    return *this;
}
CMailBox::~CMailBox() {
    delete [] m_Addr;
    const size_t MAX = (m_In_size < m_Out_size) ? m_Out_size : m_In_size;
    for (size_t i = 0; i < MAX; i++) {
        if (i < m_Out_size) {
            delete m_Out[i];
        }
        if (i < m_In_size) {
            delete m_In[i];
        }
    }
    delete [] m_Out;
    delete [] m_In;

    m_Addr = nullptr;
    m_Out = m_In = nullptr;
    m_Out_size = m_In_size = 0;
}

/**
 * Try to add new email to outbox
 * @param out
 * @return true, if ok;
 *      false otherwise
 */
bool
CMailBox::add_out(const CMail & out) {
    if (strcmp(out.get_from(), m_Addr)) {
        return false;
    }

    CMail ** new_m_Out = new CMail *[m_Out_size + 1];
    for (size_t i = 0; i < m_Out_size; i++) {
        new_m_Out[i] = new CMail(*(m_Out[i]));
        delete m_Out[i];
    }
    delete [] m_Out;
    new_m_Out[m_Out_size++] = new CMail(out);
    m_Out = new_m_Out;
    return true;
}

/**
 * Try to add new email to inbox
 * @param in
 * @return true, if ok;
 *      false otherwise
 */
bool
CMailBox::add_in(const CMail & in) {
    if (strcmp(in.get_to(), m_Addr)) {
        return false;
    }

    CMail ** new_m_In = new CMail *[m_In_size + 1];
    for (size_t i = 0; i < m_In_size; i++) {
        new_m_In[i] = new CMail(*(m_In[i]));
        delete m_In[i];
    }
    delete [] m_In;
    new_m_In[m_In_size++] = new CMail(in);
    m_In = new_m_In;
    return true;
}

const char *
CMailBox::get_addr() const {
    return m_Addr;
}
const CMail &
CMailBox::get_out(const size_t idx) const {
    return *(m_Out[idx]);
}
const CMail &
CMailBox::get_in(const size_t idx) const {
    return *(m_In[idx]);
}
const size_t
CMailBox::get_out_size() const {
    return m_Out_size;
}
const size_t
CMailBox::get_in_size() const {
    return m_In_size;
}
// ---------------------------------------------------------------------------




const bool OUT = false, IN = true;
// ---------------------------------------------------------------------------
class CMailIterator;
class CMailServer {
    CMailBox ** m_Boxes;
    size_t m_Boxes_size;

    bool find_box(const char * email, size_t & out) const;

    void sendMail_wrapper(const CMail & m, const bool TYPE);

    public:
        explicit CMailServer();
        CMailServer(const CMailServer & src);
        CMailServer & operator = (const CMailServer & src);
        ~CMailServer();

        void sendMail(const CMail & m);
        CMailIterator outbox(const char * email) const;
        CMailIterator inbox(const char * email) const;
        
        friend class CMailIterator;
};
CMailServer::CMailServer()
    : m_Boxes(nullptr), m_Boxes_size(0) { }
CMailServer::CMailServer(const CMailServer & src)
    : m_Boxes_size(src.m_Boxes_size) {
    m_Boxes = new CMailBox *[m_Boxes_size];
    for (size_t i = 0; i < m_Boxes_size; i++) {
        m_Boxes[i] = new CMailBox(*(src.m_Boxes[i]));
    }
}
CMailServer &
CMailServer::operator = (const CMailServer & src) {
    if (&src == this) {
        return *this;
    }

    for (size_t i = 0; i < m_Boxes_size; i++) {
        delete m_Boxes[i];
    }
    delete [] m_Boxes;

    m_Boxes_size = src.m_Boxes_size;
    m_Boxes = new CMailBox *[m_Boxes_size];
    for (size_t i = 0; i < m_Boxes_size; i++) {
        m_Boxes[i] = new CMailBox(*(src.m_Boxes[i]));
    }
    return *this;
}
CMailServer::~CMailServer() {
    for (size_t i = 0; i < m_Boxes_size; i++) {
        delete m_Boxes[i];
    }
    delete [] m_Boxes;
    m_Boxes = nullptr;
    m_Boxes_size = 0;
}

/**
 * Try to find a mail box with the provided address.
 * If found, out will contain an index of that box;
 * it will otherwise contain an index of the box with the most similar address
 * @param email
 * @param out
 * @return true, if found;
 *      false otherwise
 */
bool
CMailServer::find_box(const char * email, size_t & out) const {
    size_t lo = 0, hi = m_Boxes_size;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int strcmp_status = strcmp(m_Boxes[mid]->get_addr(), email);
        if (strcmp_status < 0) {
            lo = mid + 1;
        }
        else if (!strcmp_status) {
            out = mid;
            return true;
        }
        else {
            hi = mid;
        }
    }
    out = lo;
    return false;
}

void
CMailServer::sendMail_wrapper(const CMail & m, const bool TYPE) {
    size_t search_idx;
    bool found;
    if (TYPE == OUT) {
        found = find_box(m.get_from(), search_idx);
    }
    else {
        found = find_box(m.get_to(), search_idx);
    }
    if (!found) {
        CMailBox ** new_m_Boxes = new CMailBox *[m_Boxes_size + 1];
        size_t idx = 0;
        for (size_t i = 0; i < m_Boxes_size; i++) {
            if (search_idx == i) {
                if (TYPE == OUT) {
                    new_m_Boxes[idx++] = new CMailBox(m.get_from());
                }
                else {
                    new_m_Boxes[idx++] = new CMailBox(m.get_to());
                }
            }
            new_m_Boxes[idx++] = new CMailBox(*(m_Boxes[i]));
            delete m_Boxes[i];
        }
        delete [] m_Boxes;
        // Didn't add
        if (m_Boxes_size++ <= search_idx) {
            if (TYPE == OUT) {
                new_m_Boxes[idx++] = new CMailBox(m.get_from());
            }
            else {
                new_m_Boxes[idx++] = new CMailBox(m.get_to());
            }
        }
        m_Boxes = new_m_Boxes;
    }

    if (TYPE == OUT) {
        m_Boxes[search_idx]->add_out(m);
    }
    else {
        m_Boxes[search_idx]->add_in(m);
    }
}

void
CMailServer::sendMail(const CMail & m) {
    sendMail_wrapper(m, OUT);
    sendMail_wrapper(m, IN);
}
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
class CMailIterator {
    // TODO: Private
    const CMailServer * m_Server;
    const size_t m_BOX_ID, m_BOX_SIZE;
    const bool m_TYPE;
    size_t m_idx;

    public:
        CMailIterator(const CMailServer * server,
                      const size_t BOX_ID, const size_t BOX_SIZE,
                      const bool TYPE);
        // CMailIterator & operator = (const CMailIterator & src);

        explicit operator bool () const;
        bool operator ! () const;

        const CMail & operator * () const;

        CMailIterator & operator ++ ();
};
CMailIterator::CMailIterator(const CMailServer * server,
                             const size_t BOX_ID, const size_t BOX_SIZE,
                             const bool TYPE)
    : m_Server(server), m_BOX_ID(BOX_ID), m_BOX_SIZE(BOX_SIZE),
      m_TYPE(TYPE), m_idx(0) { }

CMailIterator::operator bool() const {
    if (!m_Server || !(m_idx < m_BOX_SIZE)) {
        return false;
    }
    return true;
}
bool
CMailIterator::operator ! () const {
    if (!m_Server || !(m_idx < m_BOX_SIZE)) {
        return true;
    }
    return false;
}

const CMail &
CMailIterator::operator * () const {
    if (m_TYPE == OUT) {
        return m_Server->m_Boxes[m_BOX_ID]->get_out(m_idx);
    }
    else {
        return m_Server->m_Boxes[m_BOX_ID]->get_in(m_idx);
    }
}

CMailIterator &
CMailIterator::operator ++ () {
    m_idx++;
    return *this;
}




CMailIterator
CMailServer::outbox(const char * email) const {
    for (size_t i = 0; i < m_Boxes_size; i++) {
        if (!strcmp(email, m_Boxes[i]->get_addr())) {
            return CMailIterator(this, i, m_Boxes[i]->get_out_size(),
                                 OUT);
        }
    }
    return CMailIterator(nullptr, 0, 0, OUT);
}
CMailIterator
CMailServer::inbox(const char * email) const {
    for (size_t i = 0; i < m_Boxes_size; i++) {
        if (!strcmp(email, m_Boxes[i]->get_addr())) {
            return CMailIterator(this, i, m_Boxes[i]->get_in_size(),
                                 IN);
        }
    }
    return CMailIterator(nullptr, 0, 0, IN);
}
// ---------------------------------------------------------------------------

#ifndef __PROGTEST__
bool
matchOutput(const CMail & m, const char * str) {
    std::ostringstream oss;
    oss << m;
    return oss.str() == str;
}

int
main() {
    char from[100], to[100], body[1024];

    assert(CMail("john", "peter", "progtest deadline") == CMail("john", "peter", "progtest deadline"));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("john", "progtest deadline", "peter")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "john", "progtest deadline")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "progtest deadline", "john")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "john", "peter")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "peter", "john")));
    CMailServer s0;
    s0.sendMail(CMail("john", "peter", "some important mail"));
    strncpy(from, "john", sizeof(from));
    strncpy(to, "thomas", sizeof(to));
    strncpy(body, "another important mail", sizeof(body));
    s0.sendMail(CMail(from, to, body));
    strncpy(from, "john", sizeof(from));
    strncpy(to, "alice", sizeof(to));
    strncpy(body, "deadline notice", sizeof(body));
    s0.sendMail(CMail(from, to, body));
    s0.sendMail(CMail("alice", "john", "deadline confirmation"));
    s0.sendMail(CMail("peter", "alice", "PR bullshit"));
    CMailIterator i0 = s0.inbox("alice");
    assert(i0 && *i0 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i0, "From: john, To: alice, Body: deadline notice"));
    assert(++i0 && *i0 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i0, "From: peter, To: alice, Body: PR bullshit"));
    assert(!++i0);

    CMailIterator i1 = s0.inbox("john");
    assert(i1 && *i1 == CMail("alice", "john", "deadline confirmation"));
    assert(matchOutput(*i1,  "From: alice, To: john, Body: deadline confirmation"));
    assert(!++i1);

    CMailIterator i2 = s0.outbox("john");
    assert(i2 && *i2 == CMail("john", "peter", "some important mail"));
    assert(matchOutput(*i2, "From: john, To: peter, Body: some important mail"));
    assert(++i2 && *i2 == CMail("john", "thomas", "another important mail"));
    assert(matchOutput(*i2, "From: john, To: thomas, Body: another important mail"));
    assert(++i2 && *i2 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i2, "From: john, To: alice, Body: deadline notice"));
    assert(!++i2);

    CMailIterator i3 = s0.outbox("thomas");
    assert(!i3);

    CMailIterator i4 = s0.outbox("steve");
    assert(!i4);

    CMailIterator i5 = s0.outbox("thomas");
    s0.sendMail(CMail("thomas", "boss", "daily report"));
    assert(!i5);

    CMailIterator i6 = s0.outbox("thomas");
    assert(i6 && *i6 == CMail("thomas", "boss", "daily report"));
    assert(matchOutput(*i6, "From: thomas, To: boss, Body: daily report"));
    assert(!++i6);

    CMailIterator i7 = s0.inbox("alice");
    s0.sendMail(CMail("thomas", "alice", "meeting details"));
    assert(i7 && *i7 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i7, "From: john, To: alice, Body: deadline notice"));
    assert(++i7 && *i7 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i7, "From: peter, To: alice, Body: PR bullshit"));
    assert(!++i7);

    CMailIterator i8 = s0.inbox("alice");
    assert(i8 && *i8 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i8, "From: john, To: alice, Body: deadline notice"));
    assert(++i8 && *i8 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i8, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i8 && *i8 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i8, "From: thomas, To: alice, Body: meeting details"));
    assert(!++i8);

    CMailServer s1(s0);
    s0.sendMail(CMail("joe", "alice", "delivery details"));
    s1.sendMail(CMail("sam", "alice", "order confirmation"));
    CMailIterator i9 = s0.inbox("alice");
    assert(i9 && *i9 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i9, "From: john, To: alice, Body: deadline notice"));
    assert(++i9 && *i9 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i9, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i9 && *i9 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i9, "From: thomas, To: alice, Body: meeting details"));
    assert(++i9 && *i9 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i9, "From: joe, To: alice, Body: delivery details"));
    assert(!++i9);

    CMailIterator i10 = s1.inbox("alice");
    assert(i10 && *i10 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i10, "From: john, To: alice, Body: deadline notice"));
    assert(++i10 && *i10 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i10, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i10 && *i10 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i10, "From: thomas, To: alice, Body: meeting details"));
    assert(++i10 && *i10 == CMail("sam", "alice", "order confirmation"));
    assert(matchOutput(*i10, "From: sam, To: alice, Body: order confirmation"));
    assert(!++i10);

    CMailServer s2;
    s2.sendMail(CMail("alice", "alice", "mailbox test"));
    CMailIterator i11 = s2.inbox("alice");
    assert(i11 && *i11 == CMail("alice", "alice", "mailbox test"));
    assert(matchOutput(*i11, "From: alice, To: alice, Body: mailbox test"));
    assert(!++i11);

    /* s2 = s0;
    s0.sendMail(CMail("steve", "alice", "newsletter"));
    s2.sendMail(CMail("paul", "alice", "invalid invoice"));
    CMailIterator i12 = s0.inbox("alice");
    assert(i12 && *i12 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i12, "From: john, To: alice, Body: deadline notice"));
    assert(++i12 && *i12 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i12, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i12 && *i12 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i12, "From: thomas, To: alice, Body: meeting details"));
    assert(++i12 && *i12 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i12, "From: joe, To: alice, Body: delivery details"));
    assert(++i12 && *i12 == CMail("steve", "alice", "newsletter"));
    assert(matchOutput(*i12, "From: steve, To: alice, Body: newsletter"));
    assert(!++i12);

    CMailIterator i13 = s2.inbox("alice");
    assert(i13 && *i13 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i13, "From: john, To: alice, Body: deadline notice"));
    assert(++i13 && *i13 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i13, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i13 && *i13 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i13, "From: thomas, To: alice, Body: meeting details"));
    assert(++i13 && *i13 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i13, "From: joe, To: alice, Body: delivery details"));
    assert(++i13 && *i13 == CMail("paul", "alice", "invalid invoice"));
    assert(matchOutput(*i13, "From: paul, To: alice, Body: invalid invoice"));
    assert(!++i13); */

    s2 = s0;
    s0.sendMail(CMail("steve", "alice", "newsletter"));
    s2.sendMail(CMail("paul", "alice", "invalid invoice"));
    CMailIterator i12 = s0.inbox("alice");
    assert(i12 && *i12 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i12, "From: john, To: alice, Body: deadline notice"));
    assert(++i12 && *i12 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i12, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i12 && *i12 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i12, "From: thomas, To: alice, Body: meeting details"));
    assert(++i12 && *i12 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i12, "From: joe, To: alice, Body: delivery details"));
    assert(++i12 && *i12 == CMail("steve", "alice", "newsletter"));
    assert(matchOutput(*i12, "From: steve, To: alice, Body: newsletter"));
    assert(!++i12);

    CMailIterator i13 = s2.inbox("alice");
    assert(i13 && *i13 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i13, "From: john, To: alice, Body: deadline notice"));
    assert(++i13 && *i13 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i13, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i13 && *i13 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i13, "From: thomas, To: alice, Body: meeting details"));
    assert(++i13 && *i13 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i13, "From: joe, To: alice, Body: delivery details"));
    assert(++i13 && *i13 == CMail("paul", "alice", "invalid invoice"));
    assert(matchOutput(*i13, "From: paul, To: alice, Body: invalid invoice"));
    assert(!++i13);

    return 0;
}
#endif  /* __PROGTEST__ */
