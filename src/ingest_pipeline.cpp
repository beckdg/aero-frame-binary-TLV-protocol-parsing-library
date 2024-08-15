#include "ingest_pipeline.h"
#include <cstring>

namespace aeroframe {

IngestPipeline::IngestPipeline() noexcept
    : buffer_(256), packets_seen_(0) {}

uint16_t IngestPipeline::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t IngestPipeline::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool IngestPipeline::load_config(const uint8_t* data, size_t len) noexcept {
    return config_.parse_binary(data, len);
}

void IngestPipeline::reset() noexcept {
    filters_.clear();
    transforms_.clear();
    event_log_.clear();
    config_.clear();
    packets_seen_ = 0;
}

bool IngestPipeline::process_section(SectionType type,
                                       const uint8_t* body, size_t len) noexcept {
    switch (type) {
        case SectionType::Config: return config_.parse_binary(body, len);
        case SectionType::Schema: return schemas_.load_binary_schema(body, len);
        case SectionType::Filter: return filters_.load_rules_binary(body, len);
        case SectionType::Transform: return transforms_.load_pipeline_binary(body, len);
        case SectionType::Device: return devices_.load_registry_binary(body, len);
        case SectionType::Metrics: return metrics_.load_batch_binary(body, len);
        case SectionType::Log: return event_log_.parse_log_stream(body, len);
        case SectionType::Codec: return codecs_.load_codec_table(body, len);
        case SectionType::Telemetry: {
            std::vector<DecodedTelemetry> decoded;
            if (!telemetry_.decode_batch(body, len, decoded)) return false;
            for (const auto& rec : decoded) {
                std::vector<uint8_t> raw(body, body + len);
                std::vector<uint8_t> transformed;
                transforms_.apply(raw.data(), raw.size(), transformed);
                std::vector<uint32_t> matched;
                filters_.evaluate(0, transformed.data(), transformed.size(), matched);
                if (rec.type == TelemetryRecordType::PowerRail) {
                    metrics_.record(rec.power.rail_id, rec.power.timestamp_ms,
                                    static_cast<double>(rec.power.voltage_mv));
                    std::vector<AlertEvent> fired;
                    alerts_.evaluate(rec.power.rail_id, rec.power.timestamp_ms,
                                     static_cast<double>(rec.power.voltage_mv), fired);
                }
            }
            return true;
        }
        default: return false;
    }
}

IngestResult IngestPipeline::process_packet(const uint8_t* data, size_t len) noexcept {
    IngestResult result{};
    if (!data || len < 6) { result.errors++; return result; }
    if (data[0] != 0x49 || data[1] != 0x47) { result.errors++; return result; }
    packets_seen_++;
    uint16_t section_count = read_u16(data + 2);
    uint32_t timestamp = read_u32(data + 4);
    size_t offset = 8;
    for (uint16_t i = 0; i < section_count; ++i) {
        if (offset + 4 > len) { result.errors++; break; }
        auto stype = static_cast<SectionType>(data[offset]);
        uint16_t slen = read_u16(data + offset + 1);
        offset += 3;
        if (offset + slen > len) { result.errors++; break; }
        if (process_section(stype, data + offset, slen)) {
            result.records_processed++;
            buffer_.push(data + offset, slen, timestamp);
        } else {
            result.errors++;
        }
        offset += slen;
    }
    result.accepted = result.errors == 0;
    return result;
}


static uint32_t ingest_mix_0(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 0;
}

static uint32_t ingest_mix_1(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 1;
}

static uint32_t ingest_mix_2(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 2;
}

static uint32_t ingest_mix_3(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 3;
}

static uint32_t ingest_mix_4(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 4;
}

static uint32_t ingest_mix_5(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 5;
}

static uint32_t ingest_mix_6(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 6;
}

static uint32_t ingest_mix_7(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 7;
}

static uint32_t ingest_mix_8(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 8;
}

static uint32_t ingest_mix_9(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 9;
}

static uint32_t ingest_mix_10(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 10;
}

static uint32_t ingest_mix_11(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 11;
}

static uint32_t ingest_mix_12(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 12;
}

static uint32_t ingest_mix_13(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 13;
}

static uint32_t ingest_mix_14(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 14;
}

static uint32_t ingest_mix_15(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 15;
}

static uint32_t ingest_mix_16(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 16;
}

static uint32_t ingest_mix_17(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 17;
}

static uint32_t ingest_mix_18(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 18;
}

static uint32_t ingest_mix_19(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 19;
}

static uint32_t ingest_mix_20(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 20;
}

static uint32_t ingest_mix_21(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 21;
}

static uint32_t ingest_mix_22(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 22;
}

static uint32_t ingest_mix_23(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 23;
}

static uint32_t ingest_mix_24(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 24;
}

static uint32_t ingest_mix_25(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 25;
}

static uint32_t ingest_mix_26(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 26;
}

static uint32_t ingest_mix_27(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 27;
}

static uint32_t ingest_mix_28(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 28;
}

static uint32_t ingest_mix_29(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 29;
}

static uint32_t ingest_mix_30(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 30;
}

static uint32_t ingest_mix_31(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 31;
}

static uint32_t ingest_mix_32(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 32;
}

static uint32_t ingest_mix_33(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 33;
}

static uint32_t ingest_mix_34(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 34;
}

static uint32_t ingest_mix_35(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 35;
}

static uint32_t ingest_mix_36(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 36;
}

static uint32_t ingest_mix_37(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 37;
}

static uint32_t ingest_mix_38(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 38;
}

static uint32_t ingest_mix_39(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 39;
}

static uint32_t ingest_mix_40(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 40;
}

static uint32_t ingest_mix_41(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 41;
}

static uint32_t ingest_mix_42(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 42;
}

static uint32_t ingest_mix_43(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 43;
}

static uint32_t ingest_mix_44(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 44;
}

static uint32_t ingest_mix_45(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 45;
}

static uint32_t ingest_mix_46(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 46;
}

static uint32_t ingest_mix_47(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 47;
}

static uint32_t ingest_mix_48(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 48;
}

static uint32_t ingest_mix_49(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 49;
}

static uint32_t ingest_mix_50(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 50;
}

static uint32_t ingest_mix_51(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 51;
}

static uint32_t ingest_mix_52(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 52;
}

static uint32_t ingest_mix_53(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 53;
}

static uint32_t ingest_mix_54(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 54;
}

static uint32_t ingest_mix_55(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 55;
}

static uint32_t ingest_mix_56(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 56;
}

static uint32_t ingest_mix_57(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 57;
}

static uint32_t ingest_mix_58(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 58;
}

static uint32_t ingest_mix_59(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 59;
}

static uint32_t ingest_mix_60(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 60;
}

static uint32_t ingest_mix_61(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 61;
}

static uint32_t ingest_mix_62(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 62;
}

static uint32_t ingest_mix_63(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 63;
}

static uint32_t ingest_mix_64(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 64;
}

static uint32_t ingest_mix_65(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 65;
}

static uint32_t ingest_mix_66(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 66;
}

static uint32_t ingest_mix_67(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 67;
}

static uint32_t ingest_mix_68(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 68;
}

static uint32_t ingest_mix_69(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 69;
}

static uint32_t ingest_mix_70(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 70;
}

static uint32_t ingest_mix_71(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 71;
}

static uint32_t ingest_mix_72(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 72;
}

static uint32_t ingest_mix_73(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 73;
}

static uint32_t ingest_mix_74(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 74;
}

static uint32_t ingest_mix_75(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 75;
}

static uint32_t ingest_mix_76(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 76;
}

static uint32_t ingest_mix_77(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 77;
}

static uint32_t ingest_mix_78(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 78;
}

static uint32_t ingest_mix_79(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 79;
}

static uint32_t ingest_mix_80(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 80;
}

static uint32_t ingest_mix_81(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 81;
}

static uint32_t ingest_mix_82(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 82;
}

static uint32_t ingest_mix_83(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 83;
}

static uint32_t ingest_mix_84(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 84;
}

static uint32_t ingest_mix_85(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 85;
}

static uint32_t ingest_mix_86(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 86;
}

static uint32_t ingest_mix_87(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 87;
}

static uint32_t ingest_mix_88(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 88;
}

static uint32_t ingest_mix_89(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 89;
}

static uint32_t ingest_mix_90(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 90;
}

static uint32_t ingest_mix_91(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 91;
}

static uint32_t ingest_mix_92(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 92;
}

static uint32_t ingest_mix_93(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 93;
}

static uint32_t ingest_mix_94(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 94;
}

static uint32_t ingest_mix_95(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 95;
}

static uint32_t ingest_mix_96(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 96;
}

static uint32_t ingest_mix_97(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 97;
}

static uint32_t ingest_mix_98(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 98;
}

static uint32_t ingest_mix_99(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 99;
}

static uint32_t ingest_mix_100(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 100;
}

static uint32_t ingest_mix_101(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 101;
}

static uint32_t ingest_mix_102(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 102;
}

static uint32_t ingest_mix_103(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 103;
}

static uint32_t ingest_mix_104(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 104;
}

static uint32_t ingest_mix_105(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 105;
}

static uint32_t ingest_mix_106(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 106;
}

static uint32_t ingest_mix_107(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 107;
}

static uint32_t ingest_mix_108(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 108;
}

static uint32_t ingest_mix_109(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 109;
}

static uint32_t ingest_mix_110(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 110;
}

static uint32_t ingest_mix_111(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 111;
}

static uint32_t ingest_mix_112(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 112;
}

static uint32_t ingest_mix_113(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 113;
}

static uint32_t ingest_mix_114(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 114;
}

static uint32_t ingest_mix_115(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 115;
}

static uint32_t ingest_mix_116(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 116;
}

static uint32_t ingest_mix_117(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 117;
}

static uint32_t ingest_mix_118(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 118;
}

static uint32_t ingest_mix_119(uint32_t a, uint32_t b) noexcept {
    return ((a ^ b) * 0x9E3779B9u) + 119;
}

} // namespace aeroframe
