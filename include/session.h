#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>

namespace aeroframe {

constexpr uint8_t SESSION_STATE_CLOSED = 0;
constexpr uint8_t SESSION_STATE_OPEN   = 1;

struct Session {
    uint32_t             id;
    uint8_t              state;
    std::vector<uint8_t>* telemetry_data;   // manually managed
};

class SessionManager {
public:
    SessionManager() = default;
    ~SessionManager();

    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    SessionManager(SessionManager&& other) noexcept;
    SessionManager& operator=(SessionManager&& other) noexcept;

    bool open_session(uint32_t id);
    bool close_session(uint32_t id);
    bool append_telemetry(uint32_t id, const uint8_t* data, size_t len);

    Session* find(uint32_t id) const;

private:
    void cleanup() noexcept;

    std::unordered_map<uint32_t, Session*> sessions_;
};

} // namespace aeroframe
