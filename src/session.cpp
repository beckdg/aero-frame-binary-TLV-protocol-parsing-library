#include "session.h"

namespace aeroframe {

// -----------------------------------------------------------------------
// Destructor — release every owned session and its telemetry buffer.
// -----------------------------------------------------------------------
SessionManager::~SessionManager() {
    cleanup();
}

// -----------------------------------------------------------------------
// Move semantics
// -----------------------------------------------------------------------
SessionManager::SessionManager(SessionManager&& other) noexcept
    : sessions_(std::move(other.sessions_))
{
    other.sessions_.clear();
}

SessionManager& SessionManager::operator=(SessionManager&& other) noexcept {
    if (this != &other) {
        cleanup();
        sessions_ = std::move(other.sessions_);
        other.sessions_.clear();
    }
    return *this;
}

// -----------------------------------------------------------------------
// Internal: delete all sessions and their telemetry buffers.
// -----------------------------------------------------------------------
void SessionManager::cleanup() noexcept {
    for (auto& kv : sessions_) {
        delete kv.second->telemetry_data;
        delete kv.second;
    }
    sessions_.clear();
}

// -----------------------------------------------------------------------
// open_session — allocate a new session + telemetry buffer.
// -----------------------------------------------------------------------
bool SessionManager::open_session(uint32_t id) {
    if (sessions_.count(id) != 0) {
        return false;                       // already exists
    }

    auto* vec     = new std::vector<uint8_t>();
    auto* session = new Session{id, SESSION_STATE_OPEN, vec};
    sessions_[id] = session;
    return true;
}

// -----------------------------------------------------------------------
// close_session — teardown a session and its buffer.
// -----------------------------------------------------------------------
bool SessionManager::close_session(uint32_t id) {
    auto it = sessions_.find(id);
    if (it == sessions_.end()) {
        return false;                       // not found
    }

    Session* s = it->second;
    delete s->telemetry_data;
    delete s;
    sessions_.erase(it);
    return true;
}

// -----------------------------------------------------------------------
// append_telemetry — push raw bytes into an active session's buffer.
// -----------------------------------------------------------------------
bool SessionManager::append_telemetry(uint32_t id,
                                      const uint8_t* data,
                                      size_t len)
{
    auto it = sessions_.find(id);
    if (it == sessions_.end()) {
        return false;
    }

    auto* buf = it->second->telemetry_data;
    buf->insert(buf->end(), data, data + len);
    return true;
}

// -----------------------------------------------------------------------
// find — lookup (nullptr if missing).
// -----------------------------------------------------------------------
Session* SessionManager::find(uint32_t id) const {
    auto it = sessions_.find(id);
    return (it != sessions_.end()) ? it->second : nullptr;
}

} // namespace aeroframe
