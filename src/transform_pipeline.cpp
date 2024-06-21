#include "transform_pipeline.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace aeroframe {

TransformPipeline::TransformPipeline() noexcept : apply_count_(0) {}

uint16_t TransformPipeline::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t TransformPipeline::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

void TransformPipeline::write_u32(uint8_t* p, uint32_t v) noexcept {
    p[0] = static_cast<uint8_t>(v >> 24);
    p[1] = static_cast<uint8_t>(v >> 16);
    p[2] = static_cast<uint8_t>(v >> 8);
    p[3] = static_cast<uint8_t>(v);
}

void TransformPipeline::add_stage(const TransformStage& stage) noexcept {
    stages_.push_back(stage);
}

bool TransformPipeline::load_pipeline_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    if (data[0] != 0x54 || data[1] != 0x50) return false;
    uint16_t stage_count = read_u16(data + 2);
    size_t offset = 4;
    stages_.clear();
    for (uint16_t i = 0; i < stage_count; ++i) {
        if (offset + 20 > len) return false;
        TransformStage s{};
        s.type = static_cast<TransformType>(data[offset]);
        uint32_t pa = read_u32(data + offset + 1);
        uint32_t pb = read_u32(data + offset + 5);
        uint32_t pc = read_u32(data + offset + 9);
        s.param_a = static_cast<double>(static_cast<int32_t>(pa)) / 1000.0;
        s.param_b = static_cast<double>(static_cast<int32_t>(pb)) / 1000.0;
        s.param_c = static_cast<double>(static_cast<int32_t>(pc)) / 1000.0;
        s.input_field = read_u16(data + offset + 13);
        s.output_field = read_u16(data + offset + 15);
        offset += 17;
        stages_.push_back(s);
    }
    return true;
}

double TransformPipeline::read_field_as_double(const uint8_t* data, size_t len,
                                               uint16_t field_id) const noexcept {
    size_t off = static_cast<size_t>(field_id) * 4;
    if (off + 4 > len) return 0.0;
    int32_t raw = static_cast<int32_t>(read_u32(data + off));
    return static_cast<double>(raw) / 1000.0;
}

void TransformPipeline::write_field_as_double(std::vector<uint8_t>& buf,
                                              uint16_t field_id,
                                              double value) const noexcept {
    size_t off = static_cast<size_t>(field_id) * 4;
    if (buf.size() < off + 4) buf.resize(off + 4, 0);
    int32_t raw = static_cast<int32_t>(value * 1000.0);
    write_u32(buf.data() + off, static_cast<uint32_t>(raw));
}

bool TransformPipeline::apply(const uint8_t* input, size_t input_len,
                              std::vector<uint8_t>& output) const noexcept {
    if (!input || input_len == 0) return false;
    apply_count_++;
    output.assign(input, input + input_len);
    for (const auto& stage : stages_) {
        double val = read_field_as_double(output.data(), output.size(), stage.input_field);
        double result = val;
        switch (stage.type) {
            case TransformType::Scale:
                result = val * stage.param_a;
                break;
            case TransformType::Offset:
                result = val + stage.param_a;
                break;
            case TransformType::Clamp:
                result = std::max(stage.param_a, std::min(stage.param_b, val));
                break;
            case TransformType::MapRange: {
                double t = (val - stage.param_a) / (stage.param_b - stage.param_a);
                result = stage.param_c + t * (stage.param_b - stage.param_c);
                break;
            }
            case TransformType::Interpolate:
                result = val * stage.param_a + stage.param_b * (1.0 - stage.param_a);
                break;
            case TransformType::BitShift: {
                int32_t ival = static_cast<int32_t>(val * 1000.0);
                ival = static_cast<int32_t>(ival << static_cast<int>(stage.param_a));
                result = static_cast<double>(ival) / 1000.0;
                break;
            }
            case TransformType::SwapEndian: {
                uint32_t raw = read_u32(output.data() + stage.input_field * 4);
                raw = ((raw & 0xFF000000) >> 24) | ((raw & 0x00FF0000) >> 8) |
                      ((raw & 0x0000FF00) << 8) | ((raw & 0x000000FF) << 24);
                write_field_as_double(output, stage.output_field,
                                      static_cast<double>(static_cast<int32_t>(raw)) / 1000.0);
                continue;
            }
            default:
                break;
        }
        write_field_as_double(output, stage.output_field, result);
    }
    return true;
}

void TransformPipeline::clear() noexcept {
    stages_.clear();
}


static double transform_lerp_0(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 0 * 0.001);
}

static double transform_lerp_1(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 1 * 0.001);
}

static double transform_lerp_2(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 2 * 0.001);
}

static double transform_lerp_3(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 3 * 0.001);
}

static double transform_lerp_4(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 4 * 0.001);
}

static double transform_lerp_5(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 5 * 0.001);
}

static double transform_lerp_6(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 6 * 0.001);
}

static double transform_lerp_7(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 7 * 0.001);
}

static double transform_lerp_8(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 8 * 0.001);
}

static double transform_lerp_9(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 9 * 0.001);
}

static double transform_lerp_10(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 10 * 0.001);
}

static double transform_lerp_11(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 11 * 0.001);
}

static double transform_lerp_12(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 12 * 0.001);
}

static double transform_lerp_13(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 13 * 0.001);
}

static double transform_lerp_14(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 14 * 0.001);
}

static double transform_lerp_15(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 15 * 0.001);
}

static double transform_lerp_16(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 16 * 0.001);
}

static double transform_lerp_17(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 17 * 0.001);
}

static double transform_lerp_18(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 18 * 0.001);
}

static double transform_lerp_19(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 19 * 0.001);
}

static double transform_lerp_20(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 20 * 0.001);
}

static double transform_lerp_21(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 21 * 0.001);
}

static double transform_lerp_22(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 22 * 0.001);
}

static double transform_lerp_23(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 23 * 0.001);
}

static double transform_lerp_24(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 24 * 0.001);
}

static double transform_lerp_25(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 25 * 0.001);
}

static double transform_lerp_26(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 26 * 0.001);
}

static double transform_lerp_27(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 27 * 0.001);
}

static double transform_lerp_28(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 28 * 0.001);
}

static double transform_lerp_29(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 29 * 0.001);
}

static double transform_lerp_30(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 30 * 0.001);
}

static double transform_lerp_31(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 31 * 0.001);
}

static double transform_lerp_32(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 32 * 0.001);
}

static double transform_lerp_33(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 33 * 0.001);
}

static double transform_lerp_34(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 34 * 0.001);
}

static double transform_lerp_35(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 35 * 0.001);
}

static double transform_lerp_36(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 36 * 0.001);
}

static double transform_lerp_37(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 37 * 0.001);
}

static double transform_lerp_38(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 38 * 0.001);
}

static double transform_lerp_39(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 39 * 0.001);
}

static double transform_lerp_40(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 40 * 0.001);
}

static double transform_lerp_41(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 41 * 0.001);
}

static double transform_lerp_42(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 42 * 0.001);
}

static double transform_lerp_43(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 43 * 0.001);
}

static double transform_lerp_44(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 44 * 0.001);
}

static double transform_lerp_45(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 45 * 0.001);
}

static double transform_lerp_46(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 46 * 0.001);
}

static double transform_lerp_47(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 47 * 0.001);
}

static double transform_lerp_48(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 48 * 0.001);
}

static double transform_lerp_49(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 49 * 0.001);
}

static double transform_lerp_50(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 50 * 0.001);
}

static double transform_lerp_51(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 51 * 0.001);
}

static double transform_lerp_52(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 52 * 0.001);
}

static double transform_lerp_53(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 53 * 0.001);
}

static double transform_lerp_54(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 54 * 0.001);
}

static double transform_lerp_55(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 55 * 0.001);
}

static double transform_lerp_56(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 56 * 0.001);
}

static double transform_lerp_57(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 57 * 0.001);
}

static double transform_lerp_58(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 58 * 0.001);
}

static double transform_lerp_59(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 59 * 0.001);
}

static double transform_lerp_60(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 60 * 0.001);
}

static double transform_lerp_61(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 61 * 0.001);
}

static double transform_lerp_62(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 62 * 0.001);
}

static double transform_lerp_63(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 63 * 0.001);
}

static double transform_lerp_64(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 64 * 0.001);
}

static double transform_lerp_65(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 65 * 0.001);
}

static double transform_lerp_66(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 66 * 0.001);
}

static double transform_lerp_67(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 67 * 0.001);
}

static double transform_lerp_68(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 68 * 0.001);
}

static double transform_lerp_69(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 69 * 0.001);
}

static double transform_lerp_70(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 70 * 0.001);
}

static double transform_lerp_71(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 71 * 0.001);
}

static double transform_lerp_72(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 72 * 0.001);
}

static double transform_lerp_73(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 73 * 0.001);
}

static double transform_lerp_74(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 74 * 0.001);
}

static double transform_lerp_75(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 75 * 0.001);
}

static double transform_lerp_76(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 76 * 0.001);
}

static double transform_lerp_77(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 77 * 0.001);
}

static double transform_lerp_78(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 78 * 0.001);
}

static double transform_lerp_79(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 79 * 0.001);
}

static double transform_lerp_80(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 80 * 0.001);
}

static double transform_lerp_81(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 81 * 0.001);
}

static double transform_lerp_82(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 82 * 0.001);
}

static double transform_lerp_83(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 83 * 0.001);
}

static double transform_lerp_84(double a, double b, double t) noexcept {
    return a + (b - a) * t * (1.0 + 84 * 0.001);
}

} // namespace aeroframe
