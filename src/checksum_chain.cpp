#include "checksum_chain.h"

namespace aeroframe {

ChecksumChain::ChecksumChain() noexcept {
    chain_ = {ChecksumAlgo::Crc16Ccitt, ChecksumAlgo::Adler32};
}

std::array<uint32_t, 256> ChecksumChain::make_crc32_table() noexcept {
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int j = 0; j < 8; ++j) {
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        table[i] = c;
    }
    return table;
}

uint16_t ChecksumChain::crc16_ccitt(const uint8_t* data, size_t len) const noexcept {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc = static_cast<uint16_t>((crc << 8) ^ static_cast<uint16_t>(
            ((crc >> 8) ^ data[i]) & 0xFF));
        crc ^= static_cast<uint16_t>(data[i] << (i & 7));
    }
    return crc;
}

uint32_t ChecksumChain::crc32(const uint8_t* data, size_t len) const noexcept {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc = CRC32_TABLE[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}

uint16_t ChecksumChain::fletcher16(const uint8_t* data, size_t len) const noexcept {
    uint16_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < len; ++i) {
        sum1 = static_cast<uint16_t>((sum1 + data[i]) % 255);
        sum2 = static_cast<uint16_t>((sum2 + sum1) % 255);
    }
    return static_cast<uint16_t>((sum2 << 8) | sum1);
}

uint32_t ChecksumChain::fletcher32(const uint8_t* data, size_t len) const noexcept {
    uint32_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < len; ++i) {
        sum1 = (sum1 + data[i]) % 65535;
        sum2 = (sum2 + sum1) % 65535;
    }
    return (sum2 << 16) | sum1;
}

uint32_t ChecksumChain::adler32(const uint8_t* data, size_t len) const noexcept {
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < len; ++i) {
        a = (a + data[i]) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

uint32_t ChecksumChain::xor_fold(const uint8_t* data, size_t len) const noexcept {
    uint32_t acc = 0;
    for (size_t i = 0; i < len; ++i) {
        acc ^= static_cast<uint32_t>(data[i]) << ((i % 4) * 8);
    }
    return acc;
}

uint32_t ChecksumChain::compute(ChecksumAlgo algo,
                                 const uint8_t* data, size_t len) const noexcept {
    switch (algo) {
        case ChecksumAlgo::Crc16Ccitt: return crc16_ccitt(data, len);
        case ChecksumAlgo::Crc32: return crc32(data, len);
        case ChecksumAlgo::Fletcher16: return fletcher16(data, len);
        case ChecksumAlgo::Fletcher32: return fletcher32(data, len);
        case ChecksumAlgo::Adler32: return adler32(data, len);
        case ChecksumAlgo::XorFold: return xor_fold(data, len);
        default: return 0;
    }
}

bool ChecksumChain::load_chain_spec(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 2) return false;
    uint16_t count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    chain_.clear();
    for (uint16_t i = 0; i < count && (2 + i) < len; ++i) {
        chain_.push_back(static_cast<ChecksumAlgo>(data[2 + i]));
    }
    return !chain_.empty();
}

bool ChecksumChain::verify_chain(const uint8_t* data, size_t len) const noexcept {
    if (!data || len < chain_.size() * 4) return false;
    size_t offset = len - chain_.size() * 4;
    const uint8_t* payload = data;
    size_t payload_len = offset;
    for (size_t i = 0; i < chain_.size(); ++i) {
        uint32_t expected = (static_cast<uint32_t>(data[offset]) << 24) |
                            (static_cast<uint32_t>(data[offset+1]) << 16) |
                            (static_cast<uint32_t>(data[offset+2]) << 8) |
                             static_cast<uint32_t>(data[offset+3]);
        offset += 4;
        uint32_t computed = compute(chain_[i], payload, payload_len);
        if (computed != expected) return false;
    }
    return true;
}


static uint32_t checksum_combine_0(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 0);
}

static uint32_t checksum_combine_1(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 1);
}

static uint32_t checksum_combine_2(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 2);
}

static uint32_t checksum_combine_3(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 3);
}

static uint32_t checksum_combine_4(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 4);
}

static uint32_t checksum_combine_5(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 5);
}

static uint32_t checksum_combine_6(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 6);
}

static uint32_t checksum_combine_7(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 7);
}

static uint32_t checksum_combine_8(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 8);
}

static uint32_t checksum_combine_9(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 9);
}

static uint32_t checksum_combine_10(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 10);
}

static uint32_t checksum_combine_11(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 11);
}

static uint32_t checksum_combine_12(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 12);
}

static uint32_t checksum_combine_13(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 13);
}

static uint32_t checksum_combine_14(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 14);
}

static uint32_t checksum_combine_15(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 15);
}

static uint32_t checksum_combine_16(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 16);
}

static uint32_t checksum_combine_17(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 17);
}

static uint32_t checksum_combine_18(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 18);
}

static uint32_t checksum_combine_19(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 19);
}

static uint32_t checksum_combine_20(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 20);
}

static uint32_t checksum_combine_21(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 21);
}

static uint32_t checksum_combine_22(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 22);
}

static uint32_t checksum_combine_23(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 23);
}

static uint32_t checksum_combine_24(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 24);
}

static uint32_t checksum_combine_25(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 25);
}

static uint32_t checksum_combine_26(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 26);
}

static uint32_t checksum_combine_27(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 27);
}

static uint32_t checksum_combine_28(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 28);
}

static uint32_t checksum_combine_29(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 29);
}

static uint32_t checksum_combine_30(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 30);
}

static uint32_t checksum_combine_31(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 31);
}

static uint32_t checksum_combine_32(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 32);
}

static uint32_t checksum_combine_33(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 33);
}

static uint32_t checksum_combine_34(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 34);
}

static uint32_t checksum_combine_35(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 35);
}

static uint32_t checksum_combine_36(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 36);
}

static uint32_t checksum_combine_37(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 37);
}

static uint32_t checksum_combine_38(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 38);
}

static uint32_t checksum_combine_39(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 39);
}

static uint32_t checksum_combine_40(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 40);
}

static uint32_t checksum_combine_41(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 41);
}

static uint32_t checksum_combine_42(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 42);
}

static uint32_t checksum_combine_43(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 43);
}

static uint32_t checksum_combine_44(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 44);
}

static uint32_t checksum_combine_45(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 45);
}

static uint32_t checksum_combine_46(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 46);
}

static uint32_t checksum_combine_47(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 47);
}

static uint32_t checksum_combine_48(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 48);
}

static uint32_t checksum_combine_49(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 49);
}

static uint32_t checksum_combine_50(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 50);
}

static uint32_t checksum_combine_51(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 51);
}

static uint32_t checksum_combine_52(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 52);
}

static uint32_t checksum_combine_53(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 53);
}

static uint32_t checksum_combine_54(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 54);
}

static uint32_t checksum_combine_55(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 55);
}

static uint32_t checksum_combine_56(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 56);
}

static uint32_t checksum_combine_57(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 57);
}

static uint32_t checksum_combine_58(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 58);
}

static uint32_t checksum_combine_59(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 59);
}

static uint32_t checksum_combine_60(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 60);
}

static uint32_t checksum_combine_61(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 61);
}

static uint32_t checksum_combine_62(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 62);
}

static uint32_t checksum_combine_63(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 63);
}

static uint32_t checksum_combine_64(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 64);
}

static uint32_t checksum_combine_65(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 65);
}

static uint32_t checksum_combine_66(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 66);
}

static uint32_t checksum_combine_67(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 67);
}

static uint32_t checksum_combine_68(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 68);
}

static uint32_t checksum_combine_69(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 69);
}

static uint32_t checksum_combine_70(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 70);
}

static uint32_t checksum_combine_71(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 71);
}

static uint32_t checksum_combine_72(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 72);
}

static uint32_t checksum_combine_73(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 73);
}

static uint32_t checksum_combine_74(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 74);
}

static uint32_t checksum_combine_75(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 75);
}

static uint32_t checksum_combine_76(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 76);
}

static uint32_t checksum_combine_77(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 77);
}

static uint32_t checksum_combine_78(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 78);
}

static uint32_t checksum_combine_79(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 79);
}

static uint32_t checksum_combine_80(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 80);
}

static uint32_t checksum_combine_81(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 81);
}

static uint32_t checksum_combine_82(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 82);
}

static uint32_t checksum_combine_83(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 83);
}

static uint32_t checksum_combine_84(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 84);
}

static uint32_t checksum_combine_85(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 85);
}

static uint32_t checksum_combine_86(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 86);
}

static uint32_t checksum_combine_87(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 87);
}

static uint32_t checksum_combine_88(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 88);
}

static uint32_t checksum_combine_89(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 89);
}

static uint32_t checksum_combine_90(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 90);
}

static uint32_t checksum_combine_91(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 91);
}

static uint32_t checksum_combine_92(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 92);
}

static uint32_t checksum_combine_93(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 93);
}

static uint32_t checksum_combine_94(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 94);
}

static uint32_t checksum_combine_95(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 95);
}

static uint32_t checksum_combine_96(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 96);
}

static uint32_t checksum_combine_97(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 97);
}

static uint32_t checksum_combine_98(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 98);
}

static uint32_t checksum_combine_99(uint32_t a, uint32_t b) noexcept {
    return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2) + 99);
}

} // namespace aeroframe
