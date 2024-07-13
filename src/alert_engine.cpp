#include "alert_engine.h"

namespace aeroframe {

AlertEngine::AlertEngine() noexcept : next_alert_id_(1) {}

uint32_t AlertEngine::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool AlertEngine::add_threshold(const AlertThreshold& t) noexcept {
    thresholds_.push_back(t);
    return true;
}

bool AlertEngine::load_thresholds_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    uint16_t count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    size_t offset = 2;
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 18 > len) return false;
        AlertThreshold t{};
        t.metric_id = read_u32(data + offset);
        int32_t lo = static_cast<int32_t>(read_u32(data + offset + 4));
        int32_t hi = static_cast<int32_t>(read_u32(data + offset + 8));
        t.lower_bound = static_cast<double>(lo) / 1000.0;
        t.upper_bound = static_cast<double>(hi) / 1000.0;
        t.severity = static_cast<AlertSeverity>(data[offset + 12]);
        t.enabled = data[offset + 13] != 0;
        add_threshold(t);
        offset += 14;
    }
    return true;
}

bool AlertEngine::evaluate(uint32_t metric_id, uint32_t ts, double value,
                           std::vector<AlertEvent>& fired) noexcept {
    fired.clear();
    for (const auto& t : thresholds_) {
        if (!t.enabled || t.metric_id != metric_id) continue;
        if (value < t.lower_bound || value > t.upper_bound) {
            AlertEvent ev{};
            ev.alert_id = next_alert_id_++;
            ev.metric_id = metric_id;
            ev.timestamp_ms = ts;
            ev.observed_value = value;
            ev.severity = t.severity;
            fired.push_back(ev);
        }
    }
    return !fired.empty();
}

void AlertEngine::clear() noexcept { thresholds_.clear(); }


static AlertSeverity alert_escalate_0(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_1(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_2(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_3(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_4(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_5(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_6(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_7(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_8(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_9(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_10(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_11(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_12(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_13(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_14(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_15(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_16(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_17(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_18(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_19(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_20(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_21(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_22(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_23(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_24(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_25(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_26(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_27(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_28(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_29(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_30(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_31(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_32(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_33(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_34(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_35(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_36(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_37(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_38(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_39(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_40(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_41(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_42(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_43(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_44(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_45(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_46(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_47(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_48(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_49(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_50(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_51(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_52(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_53(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_54(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_55(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_56(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_57(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_58(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_59(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_60(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_61(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_62(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_63(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_64(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_65(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_66(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_67(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_68(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_69(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_70(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_71(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_72(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_73(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_74(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_75(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_76(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_77(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 2 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_78(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 0 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

static AlertSeverity alert_escalate_79(AlertSeverity s) noexcept {
    if (static_cast<uint8_t>(s) + 1 >= 3) return AlertSeverity::Emergency;
    return static_cast<AlertSeverity>(static_cast<uint8_t>(s) + 1);
}

} // namespace aeroframe
