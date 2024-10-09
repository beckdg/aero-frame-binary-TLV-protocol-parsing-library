#pragma once
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <deque>
namespace aeroframe {
struct RateLimitConfig {
    uint32_t key_id;
    uint32_t max_events;
    uint32_t window_ms;
};
class RateLimiter {
public:
    RateLimiter() noexcept;
    bool configure(const RateLimitConfig& cfg) noexcept;
    bool load_config_binary(const uint8_t* data, size_t len) noexcept;
    bool allow(uint32_t key_id, uint32_t timestamp_ms) noexcept;
    void reset(uint32_t key_id) noexcept;
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    struct Bucket {
        RateLimitConfig config;
        std::deque<uint32_t> timestamps;
    };
    std::unordered_map<uint32_t, Bucket> buckets_;
};
} // namespace aeroframe
