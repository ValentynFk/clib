# Very straight forward and powerful Session Manager
Was developed using C++20 concepts. Here is a sample API.
```
#include <iostream>
#include "SessionManager.hpp"

class DummySession : ISession {
public: int i = 0;
};

int main() {
    SessionManager<DummySession> sess_mgr;
    std::cout << "sessions: " << sess_mgr.count() << '\n';
    static_cast<void>(sess_mgr.new_session());
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
# Extremely efficient and lightweight QT-like signals engine
Was developed using C++20 concepts.  
Many thanks to Lars (do not hesitate to go and check his repositories), especially to his SimpleSignal concept, https://github.com/larspensjo/SimpleSignal.git  
My implementation was inspired by SimpleSignal, albeit I've changed the underlying behavior to improve speed even more and added Google unittests. Here is a sample API.
```
#include <iostream>
#include "SimpleSignal.hpp"

#define IGNORE_RETURN(expr) static_cast<void>(expr);

void logger1(const std::string &msg) {
    std::cout << "Logger1: message has been raised: " << msg << '\n';
}

class DummyObserver {
    std::unordered_map<int, std::string> statuses;
public:
    DummyObserver() = default;
    void dummy_slot(const int &id, const std::string &update) { statuses[id] = update; };
    void flush() {
        for (const auto & status_entry : statuses) {
            std::cout << '[' << status_entry.first << "] -> " << status_entry.second << '\n'; // [<id>] -> <message body>
        }
    }
};

class DummyObservable {
    Simple::Signal<void(const int&, const std::string&)> signal;
    decltype(signal)::SlotHandle handle;
    const int id;
public:
    explicit DummyObservable(DummyObserver &obs, int _id = 0)
        : signal(), handle(signal.connect_slot(obs, &DummyObserver::dummy_slot)), id(_id) {};
    void trigger_event(const std::string &update) {
        signal.emit(id, update);
    }
};

int main() {
    Simple::Signal<decltype(logger1)> events_logger;
    IGNORE_RETURN(events_logger.connect(logger1));
    IGNORE_RETURN(events_logger.connect([] (const std::string &msg) {
        std::cout << "Logger2: message has been raised: " << msg << '\n';
    }));
    events_logger.emit("Event 1");
    events_logger.emit("Event 2");
    events_logger.emit("Event 3");

    auto observer = new DummyObserver();
    DummyObservable observable1(*observer, 10);
    DummyObservable observable2(*observer, 11);
    DummyObservable observable3(*observer, 12);
    observable1.trigger_event("Event in first object");
    observable2.trigger_event("Event in second object");
    observable3.trigger_event("Event in third object");

    observer->flush();

    return 0;
}
```
