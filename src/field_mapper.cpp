#include "field_mapper.h"
#include <cmath>
#include <algorithm>

namespace aeroframe {

FieldMapper::FieldMapper() noexcept {}

uint16_t FieldMapper::read_u16(const uint8_t* p) noexcept {
    return static_cast<uint16_t>(p[0]) << 8 | static_cast<uint16_t>(p[1]);
}

uint32_t FieldMapper::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

void FieldMapper::write_u32(uint8_t* p, uint32_t v) noexcept {
    p[0] = static_cast<uint8_t>(v >> 24);
    p[1] = static_cast<uint8_t>(v >> 16);
    p[2] = static_cast<uint8_t>(v >> 8);
    p[3] = static_cast<uint8_t>(v);
}

bool FieldMapper::add_mapping(const FieldMapping& m) noexcept {
    mappings_.push_back(m);
    return true;
}

bool FieldMapper::load_mappings_binary(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 4) return false;
    uint16_t count = read_u16(data);
    size_t offset = 2;
    mappings_.clear();
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 16 > len) return false;
        FieldMapping m{};
        m.src_field_id = read_u16(data + offset);
        m.dst_field_id = read_u16(data + offset + 2);
        m.transform_id = data[offset + 4];
        int32_t sc = static_cast<int32_t>(read_u32(data + offset + 5));
        int32_t off = static_cast<int32_t>(read_u32(data + offset + 9));
        m.scale = static_cast<double>(sc) / 1000.0;
        m.offset = static_cast<double>(off) / 1000.0;
        mappings_.push_back(m);
        offset += 13;
    }
    return true;
}

double FieldMapper::apply_transform(uint8_t tid, double v,
                                    double scale, double offset) const noexcept {
    switch (tid) {
        case 0: return v;
        case 1: return v * scale;
        case 2: return v + offset;
        case 3: return v * scale + offset;
        case 4: return std::log1p(v > -1.0 ? v : 0.0);
        case 5: return std::sqrt(v >= 0.0 ? v : 0.0);
        default: return v;
    }
}

bool FieldMapper::map_record(const uint8_t* src, size_t src_len,
                             std::vector<uint8_t>& dst) const noexcept {
    if (!src) return false;
    size_t max_field = 0;
    for (const auto& m : mappings_) {
        max_field = std::max(max_field, static_cast<size_t>(m.dst_field_id));
    }
    dst.assign((max_field + 1) * 4, 0);
    for (const auto& m : mappings_) {
        size_t src_off = static_cast<size_t>(m.src_field_id) * 4;
        size_t dst_off = static_cast<size_t>(m.dst_field_id) * 4;
        if (src_off + 4 > src_len || dst_off + 4 > dst.size()) continue;
        int32_t raw = static_cast<int32_t>(read_u32(src + src_off));
        double val = static_cast<double>(raw) / 1000.0;
        val = apply_transform(m.transform_id, val, m.scale, m.offset);
        int32_t out_raw = static_cast<int32_t>(val * 1000.0);
        write_u32(dst.data() + dst_off, static_cast<uint32_t>(out_raw));
    }
    return !mappings_.empty();
}

void FieldMapper::clear() noexcept { mappings_.clear(); }


static double field_map_curve_0(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 1);
}

static double field_map_curve_1(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 2);
}

static double field_map_curve_2(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 3);
}

static double field_map_curve_3(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 4);
}

static double field_map_curve_4(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 5);
}

static double field_map_curve_5(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 6);
}

static double field_map_curve_6(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 7);
}

static double field_map_curve_7(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 8);
}

static double field_map_curve_8(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 9);
}

static double field_map_curve_9(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 10);
}

static double field_map_curve_10(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 11);
}

static double field_map_curve_11(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 12);
}

static double field_map_curve_12(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 13);
}

static double field_map_curve_13(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 14);
}

static double field_map_curve_14(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 15);
}

static double field_map_curve_15(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 16);
}

static double field_map_curve_16(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 17);
}

static double field_map_curve_17(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 18);
}

static double field_map_curve_18(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 19);
}

static double field_map_curve_19(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 20);
}

static double field_map_curve_20(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 21);
}

static double field_map_curve_21(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 22);
}

static double field_map_curve_22(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 23);
}

static double field_map_curve_23(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 24);
}

static double field_map_curve_24(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 25);
}

static double field_map_curve_25(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 26);
}

static double field_map_curve_26(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 27);
}

static double field_map_curve_27(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 28);
}

static double field_map_curve_28(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 29);
}

static double field_map_curve_29(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 30);
}

static double field_map_curve_30(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 31);
}

static double field_map_curve_31(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 32);
}

static double field_map_curve_32(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 33);
}

static double field_map_curve_33(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 34);
}

static double field_map_curve_34(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 35);
}

static double field_map_curve_35(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 36);
}

static double field_map_curve_36(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 37);
}

static double field_map_curve_37(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 38);
}

static double field_map_curve_38(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 39);
}

static double field_map_curve_39(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 40);
}

static double field_map_curve_40(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 41);
}

static double field_map_curve_41(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 42);
}

static double field_map_curve_42(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 43);
}

static double field_map_curve_43(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 44);
}

static double field_map_curve_44(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 45);
}

static double field_map_curve_45(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 46);
}

static double field_map_curve_46(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 47);
}

static double field_map_curve_47(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 48);
}

static double field_map_curve_48(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 49);
}

static double field_map_curve_49(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 50);
}

static double field_map_curve_50(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 51);
}

static double field_map_curve_51(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 52);
}

static double field_map_curve_52(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 53);
}

static double field_map_curve_53(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 54);
}

static double field_map_curve_54(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 55);
}

static double field_map_curve_55(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 56);
}

static double field_map_curve_56(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 57);
}

static double field_map_curve_57(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 58);
}

static double field_map_curve_58(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 59);
}

static double field_map_curve_59(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 60);
}

static double field_map_curve_60(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 61);
}

static double field_map_curve_61(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 62);
}

static double field_map_curve_62(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 63);
}

static double field_map_curve_63(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 64);
}

static double field_map_curve_64(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 65);
}

static double field_map_curve_65(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 66);
}

static double field_map_curve_66(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 67);
}

static double field_map_curve_67(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 68);
}

static double field_map_curve_68(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 69);
}

static double field_map_curve_69(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 70);
}

static double field_map_curve_70(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 71);
}

static double field_map_curve_71(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 72);
}

static double field_map_curve_72(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 73);
}

static double field_map_curve_73(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 74);
}

static double field_map_curve_74(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 75);
}

static double field_map_curve_75(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 76);
}

static double field_map_curve_76(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 77);
}

static double field_map_curve_77(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 78);
}

static double field_map_curve_78(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 79);
}

static double field_map_curve_79(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 80);
}

static double field_map_curve_80(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 81);
}

static double field_map_curve_81(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 82);
}

static double field_map_curve_82(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 83);
}

static double field_map_curve_83(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 84);
}

static double field_map_curve_84(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 85);
}

static double field_map_curve_85(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 86);
}

static double field_map_curve_86(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 87);
}

static double field_map_curve_87(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 88);
}

static double field_map_curve_88(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 89);
}

static double field_map_curve_89(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 90);
}

static double field_map_curve_90(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 91);
}

static double field_map_curve_91(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 92);
}

static double field_map_curve_92(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 93);
}

static double field_map_curve_93(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 94);
}

static double field_map_curve_94(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 95);
}

static double field_map_curve_95(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 96);
}

static double field_map_curve_96(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 97);
}

static double field_map_curve_97(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 98);
}

static double field_map_curve_98(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 99);
}

static double field_map_curve_99(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 100);
}

static double field_map_curve_100(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 101);
}

static double field_map_curve_101(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 102);
}

static double field_map_curve_102(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 103);
}

static double field_map_curve_103(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 104);
}

static double field_map_curve_104(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 105);
}

static double field_map_curve_105(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 106);
}

static double field_map_curve_106(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 107);
}

static double field_map_curve_107(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 108);
}

static double field_map_curve_108(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 109);
}

static double field_map_curve_109(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 110);
}

static double field_map_curve_110(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 111);
}

static double field_map_curve_111(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 112);
}

static double field_map_curve_112(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 113);
}

static double field_map_curve_113(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 114);
}

static double field_map_curve_114(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 115);
}

static double field_map_curve_115(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 116);
}

static double field_map_curve_116(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 117);
}

static double field_map_curve_117(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 118);
}

static double field_map_curve_118(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 119);
}

static double field_map_curve_119(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 120);
}

static double field_map_curve_120(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 121);
}

static double field_map_curve_121(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 122);
}

static double field_map_curve_122(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 123);
}

static double field_map_curve_123(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 124);
}

static double field_map_curve_124(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 125);
}

static double field_map_curve_125(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 126);
}

static double field_map_curve_126(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 127);
}

static double field_map_curve_127(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 128);
}

static double field_map_curve_128(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 129);
}

static double field_map_curve_129(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 130);
}

static double field_map_curve_130(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 131);
}

static double field_map_curve_131(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 132);
}

static double field_map_curve_132(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 133);
}

static double field_map_curve_133(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 134);
}

static double field_map_curve_134(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 135);
}

static double field_map_curve_135(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 136);
}

static double field_map_curve_136(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 137);
}

static double field_map_curve_137(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 138);
}

static double field_map_curve_138(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 139);
}

static double field_map_curve_139(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 140);
}

static double field_map_curve_140(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 141);
}

static double field_map_curve_141(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 142);
}

static double field_map_curve_142(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 143);
}

static double field_map_curve_143(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 144);
}

static double field_map_curve_144(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 145);
}

static double field_map_curve_145(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 146);
}

static double field_map_curve_146(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 147);
}

static double field_map_curve_147(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 148);
}

static double field_map_curve_148(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 149);
}

static double field_map_curve_149(double x) noexcept {
    return x / (1.0 + std::abs(x) * 0.001 * 150);
}

} // namespace aeroframe
