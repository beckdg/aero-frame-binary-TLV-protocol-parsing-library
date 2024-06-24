#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <deque>

namespace aeroframe {

struct MetricSample {
    uint32_t timestamp_ms;
    double value;
};

struct MetricStats {
    double min_val;
    double max_val;
    double sum;
    double sum_sq;
    uint64_t count;
    double last_value;
};

class MetricStore {
public:
    MetricStore() noexcept;

    void record(uint32_t metric_id, uint32_t timestamp_ms, double value) noexcept;
    bool load_batch_binary(const uint8_t* data, size_t len) noexcept;
    bool get_stats(uint32_t metric_id, MetricStats& out) const noexcept;
    bool get_window(uint32_t metric_id, uint32_t window_ms,
                    std::vector<MetricSample>& out) const noexcept;
    void expire_before(uint32_t timestamp_ms) noexcept;
    size_t metric_count() const noexcept { return series_.size(); }

private:
    static uint32_t read_u32(const uint8_t* p) noexcept;

    struct MetricSeries {
        std::deque<MetricSample> samples;
        MetricStats stats{};
    };

    std::unordered_map<uint32_t, MetricSeries> series_;
    uint32_t max_samples_per_metric_{1024};
};

} // namespace aeroframe
