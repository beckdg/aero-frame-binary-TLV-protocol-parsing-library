#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
namespace aeroframe {
struct SessionRecord {
    uint32_t session_id;
    uint32_t opened_at_ms;
    uint32_t last_active_ms;
    uint8_t state;
    uint32_t byte_count;
    std::vector<uint8_t> metadata;
};
class SessionStore {
public:
    SessionStore() noexcept;
    bool open(uint32_t session_id, uint32_t timestamp_ms) noexcept;
    bool close(uint32_t session_id) noexcept;
    bool append(uint32_t session_id, const uint8_t* data, size_t len) noexcept;
    bool load_store_binary(const uint8_t* data, size_t len) noexcept;
    const SessionRecord* lookup(uint32_t session_id) const noexcept;
    size_t active_count() const noexcept;
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::unordered_map<uint32_t, SessionRecord> sessions_;
};
} // namespace aeroframe
