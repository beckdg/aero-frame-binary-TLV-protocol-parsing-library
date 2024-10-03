#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <queue>
namespace aeroframe {
enum class StageKind : uint8_t {
    Decode, Validate, Transform, Filter, Store, Alert, Dispatch
};
struct ScheduledStage {
    uint32_t stage_id;
    StageKind kind;
    uint32_t deadline_ms;
    uint16_t priority;
    std::vector<uint8_t> input;
};
struct ScheduleResult {
    bool completed;
    uint32_t stages_run;
    uint32_t stages_failed;
};
class PipelineScheduler {
public:
    PipelineScheduler() noexcept;
    bool enqueue(ScheduledStage stage) noexcept;
    bool load_schedule_binary(const uint8_t* data, size_t len) noexcept;
    ScheduleResult run_until(uint32_t now_ms) noexcept;
    void clear() noexcept;
    size_t pending_count() const noexcept { return pending_.size(); }
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    static uint16_t read_u16(const uint8_t* p) noexcept;
    bool run_stage(ScheduledStage& stage) noexcept;
    std::vector<ScheduledStage> pending_;
    uint32_t total_run_{0};
};
} // namespace aeroframe
