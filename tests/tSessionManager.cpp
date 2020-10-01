#include "gtest/gtest.h"
#include "SessionManager/SessionManager.hpp"

class DummySession {
public:
    int i = 0;
}; // Mock the session type
class ExpectedException : std::exception {};
class UnexpectedException : std::exception {};
#define IGNORE_RETURN(expr) static_cast<void>(expr)

using namespace Simple;

TEST(SessionManager, InitialCountIs0) {
    SessionManager<DummySession> sess_mgr;
    EXPECT_EQ (sess_mgr.count(),  0);
}

TEST(SessionManager, AfterNewSessionBecomesNotEmpty) {
    SessionManager<DummySession> sess_mgr;
    EXPECT_TRUE (sess_mgr.empty());
    IGNORE_RETURN(sess_mgr.new_session());
    EXPECT_FALSE (sess_mgr.empty());
}

TEST(SessionManager, AfterNewSessionCountIncreases) {
    SessionManager<DummySession> sess_mgr;
    for (uint8_t i = 0; i < 0xff; ++i) {
        EXPECT_EQ (sess_mgr.count(),  i);
        IGNORE_RETURN(sess_mgr.new_session());
    }
}

TEST(SessionManager, DeleteSessWhenEmptyThrows) {
    SessionManager<DummySession> sess_mgr;
    EXPECT_THROW ( {
        try {
            sess_mgr.delete_session(0);
        } catch (const SessionManager<DummySession>::SessionInvalid &e) {
            if (strcmp(e.what(), "Manager is empty") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SessionManager, DeleteUnexistingSessThrows) {
    SessionManager<DummySession> sess_mgr;
    auto broken_sess_handle = sess_mgr.new_session() + 1;
    EXPECT_THROW ( {
        try {
            sess_mgr.delete_session(broken_sess_handle);
        } catch (const SessionManager<DummySession>::SessionInvalid &e) {
            if (strcmp(e.what(), "Session does not exist") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SessionManager, DeleteExistingSessDoesNotThrow) {
    SessionManager<DummySession> sess_mgr;
    std::vector<size_t> sess_handles(0xff);
    for (uint8_t i = 0; i < 0xff; ++i) {
        sess_handles[i] = sess_mgr.new_session();
    }
    EXPECT_NO_THROW ( {
        for (uint8_t i = 0; i < 0xff; ++i) {
            sess_mgr.delete_session(sess_handles[i]);
        }
    });
}

TEST(SessionManager, AfterDeletingAllSessionsIsEmpty) {
    SessionManager<DummySession> sess_mgr;
    auto sess_handle = sess_mgr.new_session();
    sess_mgr.delete_session(sess_handle);
    EXPECT_TRUE (sess_mgr.empty());
}

TEST(SessionManager, RightAfterReachingSessLimitThrows) {
    SessionManager<DummySession> sess_mgr(1);
    EXPECT_NO_THROW ( {
        IGNORE_RETURN(sess_mgr.new_session());
    });
    EXPECT_THROW ( {
        try {
            IGNORE_RETURN(sess_mgr.new_session());
        } catch (const SessionManager<DummySession>::SessionInvalid &e) {
            if (strcmp(e.what(), "Manager is full") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SessionManager, GetSessWhenEmptyThrows) {
    SessionManager<DummySession> sess_mgr;
    EXPECT_THROW ( {
        try {
            IGNORE_RETURN(sess_mgr.get_session(0));
        } catch (const SessionManager<DummySession>::SessionInvalid &e) {
            if (strcmp(e.what(), "Manager is empty") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SessionManager, GetUnexistingSessThrows) {
    SessionManager<DummySession> sess_mgr;
    auto broken_sess_handle = sess_mgr.new_session() + 1;
    EXPECT_THROW ( {
        try {
            IGNORE_RETURN(sess_mgr.get_session(broken_sess_handle));
        } catch (const SessionManager<DummySession>::SessionInvalid &e) {
            if (strcmp(e.what(), "Session does not exist") == 0)
                throw ExpectedException();
            else throw UnexpectedException();
        }
    }, ExpectedException);
}

TEST(SessionManager, GetExistingSessDoesNotThrow) {
    SessionManager<DummySession> sess_mgr;
    auto sess_handle = sess_mgr.new_session();
    EXPECT_NO_THROW ( {
        IGNORE_RETURN(sess_mgr.get_session(sess_handle));
    });
}

TEST(SessionManager, GetSessReturnsMutableSess) {
    SessionManager<DummySession> sess_mgr;
    auto sess_handle = sess_mgr.new_session();
    /*[[maybe_unused]]*/ auto& sess = sess_mgr.get_session(sess_handle);
    sess.i = 13;
    EXPECT_EQ (sess_mgr.get_session(sess_handle).i, 13);
    sess.i = 14;
    EXPECT_EQ (sess_mgr.get_session(sess_handle).i, 14);
}