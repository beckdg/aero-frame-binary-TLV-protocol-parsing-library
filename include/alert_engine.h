#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
namespace aeroframe {
enum class AlertSeverity : uint8_t { Info, Warning, Critical, Emergency };
struct AlertThreshold {
    uint32_t metric_id;
    double lower_bound;
    double upper_bound;
    AlertSeverity severity;
    bool enabled;
};
struct AlertEvent {
    uint32_t alert_id;
    uint32_t metric_id;
    uint32_t timestamp_ms;
    double observed_value;
    AlertSeverity severity;
};
class AlertEngine {
public:
    AlertEngine() noexcept;
    bool add_threshold(const AlertThreshold& t) noexcept;
    bool load_thresholds_binary(const uint8_t* data, size_t len) noexcept;
    bool evaluate(uint32_t metric_id, uint32_t ts, double value,
                  std::vector<AlertEvent>& fired) noexcept;
    void clear() noexcept;
private:
    static uint32_t read_u32(const uint8_t* p) noexcept;
    std::vector<AlertThreshold> thresholds_;
    uint32_t next_alert_id_{1};
};
} // namespace aeroframe
