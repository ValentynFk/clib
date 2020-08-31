# Very lightweight and powerfule Session Manaager
Was developed using C++20 concepts. Here is a simple API.
```
#include <iostream>
#include "SessionManager.hpp"

class DummySession : ISession {
public: int i = 0;
};

int main() {
    SessionManager<DummySession> sess_mgr;
    std::cout << "sessions: " << sess_mgr.count() << '\n';
    IGNORE_RETURN(sess_mgr.new_session());
    auto handle1 = sess_mgr.new_session();
    auto handle2 = sess_mgr.new_session();
    auto handle3 = sess_mgr.new_session();
    std::cout << "handle 1: " << handle1 << '\n';
    std::cout << "handle 2: " << handle2 << '\n';
    std::cout << "handle 3: " << handle3 << '\n';
    std::cout << "sessions: " << sess_mgr.count() << '\n';
    sess_mgr.delete_session(handle3);
    sess_mgr.delete_session(handle2);
    std::cout << "sessions: " << sess_mgr.count() << '\n';
    auto & sess = sess_mgr.get_session(handle1);
    std::cout << "sess.i: " << sess.i << '\n';
    sess.i++;
    std::cout << "sess.i: " << sess.i << '\n';
    sess_mgr.delete_session(handle1);
    std::cout << "sessions: " << sess_mgr.count() << '\n';

    return 0;
}
```