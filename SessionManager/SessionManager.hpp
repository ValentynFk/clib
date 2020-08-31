//
// Created by Valentyn Faychuk (faithcouk.valentyn@gmail.com)
//                              on 8/30/2020.
//

#ifndef STUDY_SESSIONMANAGER_H
#define STUDY_SESSIONMANAGER_H

#include "ISession.hpp"

#include <string>
#include <cstdlib>
#include <exception>
#include <type_traits>
#include <unordered_map>
#include <boost/random.hpp>

#define MAX_SESSIONS 1000

template<typename Session> concept SessionLike =
    std::is_default_constructible<Session>::value && std::is_base_of<ISession, Session>::value;

template <SessionLike Session>
class SessionManager {
    std::unordered_map<size_t, Session> sessions;
    const size_t max_sessions;
    [[nodiscard]] typename std::unordered_map<size_t,Session>::iterator find_session_or_throw(const size_t&);
    [[nodiscard]] static size_t gen_handle(const size_t&);
public:
    class SessionInvalid : std::exception {
        std::string e;
    public:
        SessionInvalid()                      = delete;
        SessionInvalid(SessionInvalid&&)      = delete;
        SessionInvalid(const SessionInvalid&) = delete;
        explicit SessionInvalid(std::string reason)
            : e(std::move(reason)) {
        };
        [[nodiscard]] const char * what() const noexcept override {
            return e.c_str();
        }
    };

    explicit SessionManager(size_t max_sessions) : sessions(), max_sessions(max_sessions) {};
    SessionManager() : sessions(), max_sessions(MAX_SESSIONS) {};
    SessionManager(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;

    [[nodiscard]] size_t count() const;
    [[nodiscard]] bool empty() const;
    template <typename ...Params>
    [[nodiscard]] size_t new_session(Params &&...);
    [[nodiscard]] Session& get_session(const size_t&);
    void delete_session(const size_t&);
};

template <SessionLike Session>
[[nodiscard]] size_t SessionManager<Session>::count() const {
    return sessions.size();
}

template <SessionLike Session>
[[nodiscard]] bool SessionManager<Session>::empty() const {
    return sessions.empty();
}

template <SessionLike Session>
template <typename ...Params>
[[nodiscard]] size_t SessionManager<Session>::new_session(Params&& ...params) {
    if (sessions.size() >= max_sessions) throw SessionInvalid("Manager is full");
    size_t sess_handle;
    do sess_handle = gen_handle(max_sessions); while (sessions.find(sess_handle) != sessions.end());
    sessions[sess_handle] = std::move(Session(std::forward<Params>(params)...));
    return sess_handle;
}

template <SessionLike Session>
[[nodiscard]] Session& SessionManager<Session>::get_session(const size_t & sess_handle) {
    return std::forward<Session&>(find_session_or_throw(sess_handle)->second);
}

template <SessionLike Session>
void SessionManager<Session>::delete_session(const size_t & sess_handle) {
    sessions.erase(find_session_or_throw(sess_handle)->first);
}

template <SessionLike Session>
[[nodiscard]] size_t SessionManager<Session>::gen_handle(const size_t &upper_limit) {
    static boost::mt19937 randomizer(time(nullptr));
    static boost::uniform_int<> allowed_keys(0, upper_limit);
    static boost::variate_generator<boost::mt19937, boost::uniform_int<>> keygen(randomizer, allowed_keys);
    return keygen();
}

template<SessionLike Session>
typename std::unordered_map<size_t, Session>::iterator
SessionManager<Session>::find_session_or_throw(const size_t &sess_handle) {
    if (sessions.empty()) throw SessionInvalid("Manager is empty");
    auto p_sess = sessions.find(sess_handle);
    if (p_sess == sessions.end()) throw SessionInvalid("Session does not exist");
    return p_sess;
}

#endif //STUDY_SESSIONMANAGER_H
