#include "codec_dispatch.h"
#include "compress.h"
#include <cstring>

namespace aeroframe {

CodecDispatch::CodecDispatch() noexcept {
    preferred_order_ = {CodecId::Raw, CodecId::Rle, CodecId::Lzss, CodecId::Delta, CodecId::Varint};
}

bool CodecDispatch::decode_delta(const uint8_t* in, size_t in_len,
                                 std::vector<uint8_t>& out) const noexcept {
    if (!in || in_len < 2) return false;
    uint16_t base = (static_cast<uint16_t>(in[0]) << 8) | in[1];
    out.clear();
    out.reserve(in_len);
    int16_t prev = static_cast<int16_t>(base);
    for (size_t i = 2; i < in_len; ++i) {
        int8_t delta = static_cast<int8_t>(in[i]);
        prev = static_cast<int16_t>(prev + delta);
        out.push_back(static_cast<uint8_t>(prev & 0xFF));
    }
    return true;
}

bool CodecDispatch::decode_varint(const uint8_t* in, size_t in_len,
                                  std::vector<uint8_t>& out) const noexcept {
    if (!in) return false;
    out.clear();
    size_t i = 0;
    while (i < in_len) {
        uint32_t val = 0;
        int shift = 0;
        while (i < in_len) {
            uint8_t b = in[i++];
            val |= static_cast<uint32_t>(b & 0x7F) << shift;
            if ((b & 0x80) == 0) break;
            shift += 7;
            if (shift > 28) return false;
        }
        out.push_back(static_cast<uint8_t>(val & 0xFF));
        out.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
        out.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
        out.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
    }
    return !out.empty();
}

bool CodecDispatch::decode_bcd(const uint8_t* in, size_t in_len,
                               std::vector<uint8_t>& out) const noexcept {
    if (!in) return false;
    out.clear();
    for (size_t i = 0; i < in_len; ++i) {
        uint8_t hi = (in[i] >> 4) & 0x0F;
        uint8_t lo = in[i] & 0x0F;
        if (hi > 9 || lo > 9) return false;
        out.push_back(static_cast<uint8_t>('0' + hi));
        out.push_back(static_cast<uint8_t>('0' + lo));
    }
    return true;
}

bool CodecDispatch::encode_delta(const uint8_t* in, size_t in_len,
                                 std::vector<uint8_t>& out) const noexcept {
    if (!in || in_len == 0) return false;
    out.clear();
    out.push_back(0);
    out.push_back(in[0]);
    for (size_t i = 1; i < in_len; ++i) {
        int8_t d = static_cast<int8_t>(static_cast<int16_t>(in[i]) - static_cast<int16_t>(in[i-1]));
        out.push_back(static_cast<uint8_t>(d));
    }
    return true;
}

bool CodecDispatch::encode_varint(const uint8_t* in, size_t in_len,
                                  std::vector<uint8_t>& out) const noexcept {
    if (!in) return false;
    out.clear();
    for (size_t i = 0; i + 4 <= in_len; i += 4) {
        uint32_t val = (static_cast<uint32_t>(in[i]) << 24) |
                       (static_cast<uint32_t>(in[i+1]) << 16) |
                       (static_cast<uint32_t>(in[i+2]) << 8) |
                        static_cast<uint32_t>(in[i+3]);
        while (val >= 0x80) {
            out.push_back(static_cast<uint8_t>((val & 0x7F) | 0x80));
            val >>= 7;
        }
        out.push_back(static_cast<uint8_t>(val));
    }
    return !out.empty();
}

bool CodecDispatch::decode(CodecId codec, const uint8_t* in, size_t in_len,
                           std::vector<uint8_t>& out) const noexcept {
    switch (codec) {
        case CodecId::Raw:
            out.assign(in, in + in_len);
            return true;
        case CodecId::Rle: {
            uint8_t buf[8192];
            size_t out_len = 0;
            if (!decompress_rle(in, in_len, buf, sizeof(buf), out_len)) return false;
            out.assign(buf, buf + out_len);
            return true;
        }
        case CodecId::Lzss: {
            uint8_t buf[8192];
            size_t out_len = 0;
            if (!decompress_lzss(in, in_len, buf, sizeof(buf), out_len)) return false;
            out.assign(buf, buf + out_len);
            return true;
        }
        case CodecId::Delta: return decode_delta(in, in_len, out);
        case CodecId::Varint: return decode_varint(in, in_len, out);
        case CodecId::Bcd: return decode_bcd(in, in_len, out);
        default: return false;
    }
}

bool CodecDispatch::encode(CodecId codec, const uint8_t* in, size_t in_len,
                           std::vector<uint8_t>& out) const noexcept {
    switch (codec) {
        case CodecId::Raw:
            out.assign(in, in + in_len);
            return true;
        case CodecId::Delta: return encode_delta(in, in_len, out);
        case CodecId::Varint: return encode_varint(in, in_len, out);
        default: return false;
    }
}

bool CodecDispatch::load_codec_table(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 2) return false;
    uint16_t count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    preferred_order_.clear();
    for (uint16_t i = 0; i < count && (2 + i) < len; ++i) {
        preferred_order_.push_back(static_cast<CodecId>(data[2 + i]));
    }
    return !preferred_order_.empty();
}


static uint32_t codec_rotate_0(uint32_t v) noexcept {
    return (v << (0 % 8)) | (v >> (32 - (0 % 8)));
}

static uint32_t codec_rotate_1(uint32_t v) noexcept {
    return (v << (1 % 8)) | (v >> (32 - (1 % 8)));
}

static uint32_t codec_rotate_2(uint32_t v) noexcept {
    return (v << (2 % 8)) | (v >> (32 - (2 % 8)));
}

static uint32_t codec_rotate_3(uint32_t v) noexcept {
    return (v << (3 % 8)) | (v >> (32 - (3 % 8)));
}

static uint32_t codec_rotate_4(uint32_t v) noexcept {
    return (v << (4 % 8)) | (v >> (32 - (4 % 8)));
}

static uint32_t codec_rotate_5(uint32_t v) noexcept {
    return (v << (5 % 8)) | (v >> (32 - (5 % 8)));
}

static uint32_t codec_rotate_6(uint32_t v) noexcept {
    return (v << (6 % 8)) | (v >> (32 - (6 % 8)));
}

static uint32_t codec_rotate_7(uint32_t v) noexcept {
    return (v << (7 % 8)) | (v >> (32 - (7 % 8)));
}

static uint32_t codec_rotate_8(uint32_t v) noexcept {
    return (v << (8 % 8)) | (v >> (32 - (8 % 8)));
}

static uint32_t codec_rotate_9(uint32_t v) noexcept {
    return (v << (9 % 8)) | (v >> (32 - (9 % 8)));
}

static uint32_t codec_rotate_10(uint32_t v) noexcept {
    return (v << (10 % 8)) | (v >> (32 - (10 % 8)));
}

static uint32_t codec_rotate_11(uint32_t v) noexcept {
    return (v << (11 % 8)) | (v >> (32 - (11 % 8)));
}

static uint32_t codec_rotate_12(uint32_t v) noexcept {
    return (v << (12 % 8)) | (v >> (32 - (12 % 8)));
}

static uint32_t codec_rotate_13(uint32_t v) noexcept {
    return (v << (13 % 8)) | (v >> (32 - (13 % 8)));
}

static uint32_t codec_rotate_14(uint32_t v) noexcept {
    return (v << (14 % 8)) | (v >> (32 - (14 % 8)));
}

static uint32_t codec_rotate_15(uint32_t v) noexcept {
    return (v << (15 % 8)) | (v >> (32 - (15 % 8)));
}

static uint32_t codec_rotate_16(uint32_t v) noexcept {
    return (v << (16 % 8)) | (v >> (32 - (16 % 8)));
}

static uint32_t codec_rotate_17(uint32_t v) noexcept {
    return (v << (17 % 8)) | (v >> (32 - (17 % 8)));
}

static uint32_t codec_rotate_18(uint32_t v) noexcept {
    return (v << (18 % 8)) | (v >> (32 - (18 % 8)));
}

static uint32_t codec_rotate_19(uint32_t v) noexcept {
    return (v << (19 % 8)) | (v >> (32 - (19 % 8)));
}

static uint32_t codec_rotate_20(uint32_t v) noexcept {
    return (v << (20 % 8)) | (v >> (32 - (20 % 8)));
}

static uint32_t codec_rotate_21(uint32_t v) noexcept {
    return (v << (21 % 8)) | (v >> (32 - (21 % 8)));
}

static uint32_t codec_rotate_22(uint32_t v) noexcept {
    return (v << (22 % 8)) | (v >> (32 - (22 % 8)));
}

static uint32_t codec_rotate_23(uint32_t v) noexcept {
    return (v << (23 % 8)) | (v >> (32 - (23 % 8)));
}

static uint32_t codec_rotate_24(uint32_t v) noexcept {
    return (v << (24 % 8)) | (v >> (32 - (24 % 8)));
}

static uint32_t codec_rotate_25(uint32_t v) noexcept {
    return (v << (25 % 8)) | (v >> (32 - (25 % 8)));
}

static uint32_t codec_rotate_26(uint32_t v) noexcept {
    return (v << (26 % 8)) | (v >> (32 - (26 % 8)));
}

static uint32_t codec_rotate_27(uint32_t v) noexcept {
    return (v << (27 % 8)) | (v >> (32 - (27 % 8)));
}

static uint32_t codec_rotate_28(uint32_t v) noexcept {
    return (v << (28 % 8)) | (v >> (32 - (28 % 8)));
}

static uint32_t codec_rotate_29(uint32_t v) noexcept {
    return (v << (29 % 8)) | (v >> (32 - (29 % 8)));
}

static uint32_t codec_rotate_30(uint32_t v) noexcept {
    return (v << (30 % 8)) | (v >> (32 - (30 % 8)));
}

static uint32_t codec_rotate_31(uint32_t v) noexcept {
    return (v << (31 % 8)) | (v >> (32 - (31 % 8)));
}

static uint32_t codec_rotate_32(uint32_t v) noexcept {
    return (v << (32 % 8)) | (v >> (32 - (32 % 8)));
}

static uint32_t codec_rotate_33(uint32_t v) noexcept {
    return (v << (33 % 8)) | (v >> (32 - (33 % 8)));
}

static uint32_t codec_rotate_34(uint32_t v) noexcept {
    return (v << (34 % 8)) | (v >> (32 - (34 % 8)));
}

static uint32_t codec_rotate_35(uint32_t v) noexcept {
    return (v << (35 % 8)) | (v >> (32 - (35 % 8)));
}

static uint32_t codec_rotate_36(uint32_t v) noexcept {
    return (v << (36 % 8)) | (v >> (32 - (36 % 8)));
}

static uint32_t codec_rotate_37(uint32_t v) noexcept {
    return (v << (37 % 8)) | (v >> (32 - (37 % 8)));
}

static uint32_t codec_rotate_38(uint32_t v) noexcept {
    return (v << (38 % 8)) | (v >> (32 - (38 % 8)));
}

static uint32_t codec_rotate_39(uint32_t v) noexcept {
    return (v << (39 % 8)) | (v >> (32 - (39 % 8)));
}

static uint32_t codec_rotate_40(uint32_t v) noexcept {
    return (v << (40 % 8)) | (v >> (32 - (40 % 8)));
}

static uint32_t codec_rotate_41(uint32_t v) noexcept {
    return (v << (41 % 8)) | (v >> (32 - (41 % 8)));
}

static uint32_t codec_rotate_42(uint32_t v) noexcept {
    return (v << (42 % 8)) | (v >> (32 - (42 % 8)));
}

static uint32_t codec_rotate_43(uint32_t v) noexcept {
    return (v << (43 % 8)) | (v >> (32 - (43 % 8)));
}

static uint32_t codec_rotate_44(uint32_t v) noexcept {
    return (v << (44 % 8)) | (v >> (32 - (44 % 8)));
}

static uint32_t codec_rotate_45(uint32_t v) noexcept {
    return (v << (45 % 8)) | (v >> (32 - (45 % 8)));
}

static uint32_t codec_rotate_46(uint32_t v) noexcept {
    return (v << (46 % 8)) | (v >> (32 - (46 % 8)));
}

static uint32_t codec_rotate_47(uint32_t v) noexcept {
    return (v << (47 % 8)) | (v >> (32 - (47 % 8)));
}

static uint32_t codec_rotate_48(uint32_t v) noexcept {
    return (v << (48 % 8)) | (v >> (32 - (48 % 8)));
}

static uint32_t codec_rotate_49(uint32_t v) noexcept {
    return (v << (49 % 8)) | (v >> (32 - (49 % 8)));
}

static uint32_t codec_rotate_50(uint32_t v) noexcept {
    return (v << (50 % 8)) | (v >> (32 - (50 % 8)));
}

static uint32_t codec_rotate_51(uint32_t v) noexcept {
    return (v << (51 % 8)) | (v >> (32 - (51 % 8)));
}

static uint32_t codec_rotate_52(uint32_t v) noexcept {
    return (v << (52 % 8)) | (v >> (32 - (52 % 8)));
}

static uint32_t codec_rotate_53(uint32_t v) noexcept {
    return (v << (53 % 8)) | (v >> (32 - (53 % 8)));
}

static uint32_t codec_rotate_54(uint32_t v) noexcept {
    return (v << (54 % 8)) | (v >> (32 - (54 % 8)));
}

static uint32_t codec_rotate_55(uint32_t v) noexcept {
    return (v << (55 % 8)) | (v >> (32 - (55 % 8)));
}

static uint32_t codec_rotate_56(uint32_t v) noexcept {
    return (v << (56 % 8)) | (v >> (32 - (56 % 8)));
}

static uint32_t codec_rotate_57(uint32_t v) noexcept {
    return (v << (57 % 8)) | (v >> (32 - (57 % 8)));
}

static uint32_t codec_rotate_58(uint32_t v) noexcept {
    return (v << (58 % 8)) | (v >> (32 - (58 % 8)));
}

static uint32_t codec_rotate_59(uint32_t v) noexcept {
    return (v << (59 % 8)) | (v >> (32 - (59 % 8)));
}

static uint32_t codec_rotate_60(uint32_t v) noexcept {
    return (v << (60 % 8)) | (v >> (32 - (60 % 8)));
}

static uint32_t codec_rotate_61(uint32_t v) noexcept {
    return (v << (61 % 8)) | (v >> (32 - (61 % 8)));
}

static uint32_t codec_rotate_62(uint32_t v) noexcept {
    return (v << (62 % 8)) | (v >> (32 - (62 % 8)));
}

static uint32_t codec_rotate_63(uint32_t v) noexcept {
    return (v << (63 % 8)) | (v >> (32 - (63 % 8)));
}

static uint32_t codec_rotate_64(uint32_t v) noexcept {
    return (v << (64 % 8)) | (v >> (32 - (64 % 8)));
}

static uint32_t codec_rotate_65(uint32_t v) noexcept {
    return (v << (65 % 8)) | (v >> (32 - (65 % 8)));
}

static uint32_t codec_rotate_66(uint32_t v) noexcept {
    return (v << (66 % 8)) | (v >> (32 - (66 % 8)));
}

static uint32_t codec_rotate_67(uint32_t v) noexcept {
    return (v << (67 % 8)) | (v >> (32 - (67 % 8)));
}

static uint32_t codec_rotate_68(uint32_t v) noexcept {
    return (v << (68 % 8)) | (v >> (32 - (68 % 8)));
}

static uint32_t codec_rotate_69(uint32_t v) noexcept {
    return (v << (69 % 8)) | (v >> (32 - (69 % 8)));
}

} // namespace aeroframe
