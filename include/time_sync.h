#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
namespace aeroframe {
struct TimeSyncSample {
    uint32_t local_ms;
    uint32_t remote_ms;
    int32_t offset_ms;
    int32_t drift_ppm;
    uint8_t stratum;
    uint8_t flags;
};
class TimeSyncEngine {
public:
    TimeSyncEngine() noexcept;
    bool parse_sync_message(const uint8_t* data, size_t len) noexcept;
    bool get_offset(int32_t& offset_ms) const noexcept;
    bool apply_correction(uint32_t& timestamp_ms) const noexcept;
    size_t sample_count() const noexcept { return samples_.size(); }
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    static int32_t read_i32(const uint8_t* p) noexcept;
    std::vector<TimeSyncSample> samples_;
    int32_t estimated_offset_{0};
    int32_t estimated_drift_{0};
    uint32_t last_sync_ms_{0};
};
} // namespace aeroframe
