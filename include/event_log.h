#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string_view>
namespace aeroframe {
enum class EventLevel : uint8_t { Debug, Info, Notice, Warning, Error, Critical };
struct LogEvent {
    uint32_t event_id;
    uint32_t timestamp_ms;
    EventLevel level;
    uint16_t source_id;
    std::vector<uint8_t> message;
};
class EventLog {
public:
    EventLog() noexcept;
    bool parse_log_stream(const uint8_t* data, size_t len) noexcept;
    bool append_event(const LogEvent& ev) noexcept;
    const std::vector<LogEvent>& events() const noexcept { return events_; }
    size_t count_by_level(EventLevel level) const noexcept;
    void clear() noexcept;
private:
    static uint16_t read_u16(const uint8_t* p) noexcept;
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::vector<LogEvent> events_;
    uint32_t max_events_{4096};
};
} // namespace aeroframe
