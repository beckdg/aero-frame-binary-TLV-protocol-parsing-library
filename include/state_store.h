#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
namespace aeroframe {
struct StateSnapshot {
    uint32_t snapshot_id;
    uint32_t timestamp_ms;
    uint32_t version;
    std::vector<uint8_t> blob;
};
class StateStore {
public:
    StateStore() noexcept;
    bool save_snapshot(const StateSnapshot& snap) noexcept;
    bool load_snapshot_binary(const uint8_t* data, size_t len) noexcept;
    bool get_snapshot(uint32_t id, StateSnapshot& out) const noexcept;
    bool get_latest(StateSnapshot& out) const noexcept;
    void prune_before(uint32_t timestamp_ms) noexcept;
    size_t snapshot_count() const noexcept { return snapshots_.size(); }
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::unordered_map<uint32_t, StateSnapshot> snapshots_;
    uint32_t latest_id_{0};
};
} // namespace aeroframe
