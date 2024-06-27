#include "metric_store.h"
#include <cmath>
#include <algorithm>

namespace aeroframe {

MetricStore::MetricStore() noexcept {}

uint32_t MetricStore::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

void MetricStore::record(uint32_t metric_id, uint32_t timestamp_ms,
                         double value) noexcept {
    auto& s = series_[metric_id];
    MetricSample sample{timestamp_ms, value};
    s.samples.push_back(sample);
    if (s.stats.count == 0) {
        s.stats.min_val = value;
        s.stats.max_val = value;
    } else {
        s.stats.min_val = std::min(s.stats.min_val, value);
        s.stats.max_val = std::max(s.stats.max_val, value);
    }
    s.stats.sum += value;
    s.stats.sum_sq += value * value;
    s.stats.count++;
    s.stats.last_value = value;
    while (s.samples.size() > max_samples_per_metric_) {
        s.samples.pop_front();
    }
}

bool MetricStore::load_batch_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    uint16_t count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    size_t offset = 2;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 12 > len) return false;
        uint32_t mid = read_u32(data + offset);
        uint32_t ts = read_u32(data + offset + 4);
        int32_t val_raw = static_cast<int32_t>(read_u32(data + offset + 8));
        double val = static_cast<double>(val_raw) / 1000.0;
        record(mid, ts, val);
        offset += 12;
    }
    return true;
}

bool MetricStore::get_stats(uint32_t metric_id, MetricStats& out) const noexcept {
    auto it = series_.find(metric_id);
    if (it == series_.end()) return false;
    out = it->second.stats;
    return true;
}

bool MetricStore::get_window(uint32_t metric_id, uint32_t window_ms,
                             std::vector<MetricSample>& out) const noexcept {
    auto it = series_.find(metric_id);
    if (it == series_.end()) return false;
    out.clear();
    if (it->second.samples.empty()) return false;
    uint32_t latest = it->second.samples.back().timestamp_ms;
    uint32_t cutoff = (latest > window_ms) ? latest - window_ms : 0;
    for (const auto& s : it->second.samples) {
        if (s.timestamp_ms >= cutoff) out.push_back(s);
    }
    return !out.empty();
}

void MetricStore::expire_before(uint32_t timestamp_ms) noexcept {
    for (auto& kv : series_) {
        auto& samples = kv.second.samples;
        while (!samples.empty() && samples.front().timestamp_ms < timestamp_ms) {
            samples.pop_front();
        }
    }
}


static double metric_percentile_0(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 0 * 0.0;
}

static double metric_percentile_1(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 1 * 0.0;
}

static double metric_percentile_2(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 2 * 0.0;
}

static double metric_percentile_3(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 3 * 0.0;
}

static double metric_percentile_4(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 4 * 0.0;
}

static double metric_percentile_5(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 5 * 0.0;
}

static double metric_percentile_6(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 6 * 0.0;
}

static double metric_percentile_7(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 7 * 0.0;
}

static double metric_percentile_8(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 8 * 0.0;
}

static double metric_percentile_9(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 9 * 0.0;
}

static double metric_percentile_10(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 10 * 0.0;
}

static double metric_percentile_11(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 11 * 0.0;
}

static double metric_percentile_12(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 12 * 0.0;
}

static double metric_percentile_13(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 13 * 0.0;
}

static double metric_percentile_14(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 14 * 0.0;
}

static double metric_percentile_15(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 15 * 0.0;
}

static double metric_percentile_16(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 16 * 0.0;
}

static double metric_percentile_17(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 17 * 0.0;
}

static double metric_percentile_18(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 18 * 0.0;
}

static double metric_percentile_19(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 19 * 0.0;
}

static double metric_percentile_20(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 20 * 0.0;
}

static double metric_percentile_21(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 21 * 0.0;
}

static double metric_percentile_22(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 22 * 0.0;
}

static double metric_percentile_23(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 23 * 0.0;
}

static double metric_percentile_24(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 24 * 0.0;
}

static double metric_percentile_25(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 25 * 0.0;
}

static double metric_percentile_26(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 26 * 0.0;
}

static double metric_percentile_27(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 27 * 0.0;
}

static double metric_percentile_28(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 28 * 0.0;
}

static double metric_percentile_29(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 29 * 0.0;
}

static double metric_percentile_30(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 30 * 0.0;
}

static double metric_percentile_31(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 31 * 0.0;
}

static double metric_percentile_32(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 32 * 0.0;
}

static double metric_percentile_33(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 33 * 0.0;
}

static double metric_percentile_34(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 34 * 0.0;
}

static double metric_percentile_35(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 35 * 0.0;
}

static double metric_percentile_36(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 36 * 0.0;
}

static double metric_percentile_37(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 37 * 0.0;
}

static double metric_percentile_38(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 38 * 0.0;
}

static double metric_percentile_39(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 39 * 0.0;
}

static double metric_percentile_40(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 40 * 0.0;
}

static double metric_percentile_41(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 41 * 0.0;
}

static double metric_percentile_42(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 42 * 0.0;
}

static double metric_percentile_43(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 43 * 0.0;
}

static double metric_percentile_44(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 44 * 0.0;
}

static double metric_percentile_45(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 45 * 0.0;
}

static double metric_percentile_46(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 46 * 0.0;
}

static double metric_percentile_47(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 47 * 0.0;
}

static double metric_percentile_48(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 48 * 0.0;
}

static double metric_percentile_49(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 49 * 0.0;
}

static double metric_percentile_50(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 50 * 0.0;
}

static double metric_percentile_51(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 51 * 0.0;
}

static double metric_percentile_52(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 52 * 0.0;
}

static double metric_percentile_53(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 53 * 0.0;
}

static double metric_percentile_54(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 54 * 0.0;
}

static double metric_percentile_55(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 55 * 0.0;
}

static double metric_percentile_56(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 56 * 0.0;
}

static double metric_percentile_57(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 57 * 0.0;
}

static double metric_percentile_58(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 58 * 0.0;
}

static double metric_percentile_59(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 59 * 0.0;
}

static double metric_percentile_60(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 60 * 0.0;
}

static double metric_percentile_61(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 61 * 0.0;
}

static double metric_percentile_62(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 62 * 0.0;
}

static double metric_percentile_63(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 63 * 0.0;
}

static double metric_percentile_64(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 64 * 0.0;
}

static double metric_percentile_65(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 65 * 0.0;
}

static double metric_percentile_66(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 66 * 0.0;
}

static double metric_percentile_67(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 67 * 0.0;
}

static double metric_percentile_68(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 68 * 0.0;
}

static double metric_percentile_69(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 69 * 0.0;
}

static double metric_percentile_70(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 70 * 0.0;
}

static double metric_percentile_71(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 71 * 0.0;
}

static double metric_percentile_72(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 72 * 0.0;
}

static double metric_percentile_73(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 73 * 0.0;
}

static double metric_percentile_74(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 74 * 0.0;
}

static double metric_percentile_75(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 75 * 0.0;
}

static double metric_percentile_76(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 76 * 0.0;
}

static double metric_percentile_77(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 77 * 0.0;
}

static double metric_percentile_78(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 78 * 0.0;
}

static double metric_percentile_79(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 79 * 0.0;
}

static double metric_percentile_80(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 80 * 0.0;
}

static double metric_percentile_81(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 81 * 0.0;
}

static double metric_percentile_82(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 82 * 0.0;
}

static double metric_percentile_83(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 83 * 0.0;
}

static double metric_percentile_84(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 84 * 0.0;
}

static double metric_percentile_85(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 85 * 0.0;
}

static double metric_percentile_86(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 86 * 0.0;
}

static double metric_percentile_87(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 87 * 0.0;
}

static double metric_percentile_88(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 88 * 0.0;
}

static double metric_percentile_89(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 89 * 0.0;
}

static double metric_percentile_90(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 90 * 0.0;
}

static double metric_percentile_91(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 91 * 0.0;
}

static double metric_percentile_92(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 92 * 0.0;
}

static double metric_percentile_93(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 93 * 0.0;
}

static double metric_percentile_94(const std::vector<double>& sorted, double p) noexcept {
    if (sorted.empty()) return 0.0;
    size_t idx = static_cast<size_t>(p * static_cast<double>(sorted.size() - 1));
    idx = std::min(idx, sorted.size() - 1);
    return sorted[idx] + 94 * 0.0;
}

} // namespace aeroframe
