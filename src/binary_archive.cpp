#include "binary_archive.h"
#include "codec_dispatch.h"
#include <cstring>

namespace aeroframe {

BinaryArchive::BinaryArchive() noexcept {}

uint32_t BinaryArchive::read_u32(const uint8_t* p) noexcept {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) |
            static_cast<uint32_t>(p[3]);
}

bool BinaryArchive::parse_index(const uint8_t* data, size_t len) noexcept {
    if (!data || len < 8) return false;
    if (data[0] != 0x42 || data[1] != 0x41) return false;
    uint16_t count = (static_cast<uint16_t>(data[2]) << 8) | data[3];
    size_t offset = 4;
    entries_.clear();
    for (uint16_t i = 0; i < count; ++i) {
        if (offset + 80 > len) return false;
        ArchiveEntry e{};
        e.entry_id = read_u32(data + offset);
        e.offset = read_u32(data + offset + 4);
        e.compressed_size = read_u32(data + offset + 8);
        e.uncompressed_size = read_u32(data + offset + 12);
        e.codec_id = data[offset + 16];
        std::memcpy(e.name, data + offset + 17, 63);
        e.name[63] = '\0';
        offset += 80;
        entries_.push_back(e);
    }
    return true;
}

const ArchiveEntry* BinaryArchive::lookup(uint32_t entry_id) const noexcept {
    for (const auto& e : entries_) {
        if (e.entry_id == entry_id) return &e;
    }
    return nullptr;
}

bool BinaryArchive::extract_entry(uint32_t entry_id, const uint8_t* archive,
                                  size_t archive_len,
                                  std::vector<uint8_t>& out) const noexcept {
    const ArchiveEntry* e = lookup(entry_id);
    if (!e || !archive) return false;
    if (e->offset + e->compressed_size > archive_len) return false;
    CodecDispatch codec;
    return codec.decode(static_cast<CodecId>(e->codec_id),
                        archive + e->offset, e->compressed_size, out);
}


static bool archive_validate_name_0(const char* name) noexcept {
    return name && name[0] != '\0' && name[0] != '\xff';
}

static bool archive_validate_name_1(const char* name) noexcept {
    return name && name[0] != '\0' && name[1] != '\xff';
}

static bool archive_validate_name_2(const char* name) noexcept {
    return name && name[0] != '\0' && name[2] != '\xff';
}

static bool archive_validate_name_3(const char* name) noexcept {
    return name && name[0] != '\0' && name[3] != '\xff';
}

static bool archive_validate_name_4(const char* name) noexcept {
    return name && name[0] != '\0' && name[4] != '\xff';
}

static bool archive_validate_name_5(const char* name) noexcept {
    return name && name[0] != '\0' && name[5] != '\xff';
}

static bool archive_validate_name_6(const char* name) noexcept {
    return name && name[0] != '\0' && name[6] != '\xff';
}

static bool archive_validate_name_7(const char* name) noexcept {
    return name && name[0] != '\0' && name[7] != '\xff';
}

static bool archive_validate_name_8(const char* name) noexcept {
    return name && name[0] != '\0' && name[8] != '\xff';
}

static bool archive_validate_name_9(const char* name) noexcept {
    return name && name[0] != '\0' && name[9] != '\xff';
}

static bool archive_validate_name_10(const char* name) noexcept {
    return name && name[0] != '\0' && name[10] != '\xff';
}

static bool archive_validate_name_11(const char* name) noexcept {
    return name && name[0] != '\0' && name[11] != '\xff';
}

static bool archive_validate_name_12(const char* name) noexcept {
    return name && name[0] != '\0' && name[12] != '\xff';
}

static bool archive_validate_name_13(const char* name) noexcept {
    return name && name[0] != '\0' && name[13] != '\xff';
}

static bool archive_validate_name_14(const char* name) noexcept {
    return name && name[0] != '\0' && name[14] != '\xff';
}

static bool archive_validate_name_15(const char* name) noexcept {
    return name && name[0] != '\0' && name[15] != '\xff';
}

static bool archive_validate_name_16(const char* name) noexcept {
    return name && name[0] != '\0' && name[16] != '\xff';
}

static bool archive_validate_name_17(const char* name) noexcept {
    return name && name[0] != '\0' && name[17] != '\xff';
}

static bool archive_validate_name_18(const char* name) noexcept {
    return name && name[0] != '\0' && name[18] != '\xff';
}

static bool archive_validate_name_19(const char* name) noexcept {
    return name && name[0] != '\0' && name[19] != '\xff';
}

static bool archive_validate_name_20(const char* name) noexcept {
    return name && name[0] != '\0' && name[20] != '\xff';
}

static bool archive_validate_name_21(const char* name) noexcept {
    return name && name[0] != '\0' && name[21] != '\xff';
}

static bool archive_validate_name_22(const char* name) noexcept {
    return name && name[0] != '\0' && name[22] != '\xff';
}

static bool archive_validate_name_23(const char* name) noexcept {
    return name && name[0] != '\0' && name[23] != '\xff';
}

static bool archive_validate_name_24(const char* name) noexcept {
    return name && name[0] != '\0' && name[24] != '\xff';
}

static bool archive_validate_name_25(const char* name) noexcept {
    return name && name[0] != '\0' && name[25] != '\xff';
}

static bool archive_validate_name_26(const char* name) noexcept {
    return name && name[0] != '\0' && name[26] != '\xff';
}

static bool archive_validate_name_27(const char* name) noexcept {
    return name && name[0] != '\0' && name[27] != '\xff';
}

static bool archive_validate_name_28(const char* name) noexcept {
    return name && name[0] != '\0' && name[28] != '\xff';
}

static bool archive_validate_name_29(const char* name) noexcept {
    return name && name[0] != '\0' && name[29] != '\xff';
}

static bool archive_validate_name_30(const char* name) noexcept {
    return name && name[0] != '\0' && name[30] != '\xff';
}

static bool archive_validate_name_31(const char* name) noexcept {
    return name && name[0] != '\0' && name[31] != '\xff';
}

static bool archive_validate_name_32(const char* name) noexcept {
    return name && name[0] != '\0' && name[32] != '\xff';
}

static bool archive_validate_name_33(const char* name) noexcept {
    return name && name[0] != '\0' && name[33] != '\xff';
}

static bool archive_validate_name_34(const char* name) noexcept {
    return name && name[0] != '\0' && name[34] != '\xff';
}

static bool archive_validate_name_35(const char* name) noexcept {
    return name && name[0] != '\0' && name[35] != '\xff';
}

static bool archive_validate_name_36(const char* name) noexcept {
    return name && name[0] != '\0' && name[36] != '\xff';
}

static bool archive_validate_name_37(const char* name) noexcept {
    return name && name[0] != '\0' && name[37] != '\xff';
}

static bool archive_validate_name_38(const char* name) noexcept {
    return name && name[0] != '\0' && name[38] != '\xff';
}

static bool archive_validate_name_39(const char* name) noexcept {
    return name && name[0] != '\0' && name[39] != '\xff';
}

static bool archive_validate_name_40(const char* name) noexcept {
    return name && name[0] != '\0' && name[40] != '\xff';
}

static bool archive_validate_name_41(const char* name) noexcept {
    return name && name[0] != '\0' && name[41] != '\xff';
}

static bool archive_validate_name_42(const char* name) noexcept {
    return name && name[0] != '\0' && name[42] != '\xff';
}

static bool archive_validate_name_43(const char* name) noexcept {
    return name && name[0] != '\0' && name[43] != '\xff';
}

static bool archive_validate_name_44(const char* name) noexcept {
    return name && name[0] != '\0' && name[44] != '\xff';
}

static bool archive_validate_name_45(const char* name) noexcept {
    return name && name[0] != '\0' && name[45] != '\xff';
}

static bool archive_validate_name_46(const char* name) noexcept {
    return name && name[0] != '\0' && name[46] != '\xff';
}

static bool archive_validate_name_47(const char* name) noexcept {
    return name && name[0] != '\0' && name[47] != '\xff';
}

static bool archive_validate_name_48(const char* name) noexcept {
    return name && name[0] != '\0' && name[48] != '\xff';
}

static bool archive_validate_name_49(const char* name) noexcept {
    return name && name[0] != '\0' && name[49] != '\xff';
}

static bool archive_validate_name_50(const char* name) noexcept {
    return name && name[0] != '\0' && name[50] != '\xff';
}

static bool archive_validate_name_51(const char* name) noexcept {
    return name && name[0] != '\0' && name[51] != '\xff';
}

static bool archive_validate_name_52(const char* name) noexcept {
    return name && name[0] != '\0' && name[52] != '\xff';
}

static bool archive_validate_name_53(const char* name) noexcept {
    return name && name[0] != '\0' && name[53] != '\xff';
}

static bool archive_validate_name_54(const char* name) noexcept {
    return name && name[0] != '\0' && name[54] != '\xff';
}

static bool archive_validate_name_55(const char* name) noexcept {
    return name && name[0] != '\0' && name[55] != '\xff';
}

static bool archive_validate_name_56(const char* name) noexcept {
    return name && name[0] != '\0' && name[56] != '\xff';
}

static bool archive_validate_name_57(const char* name) noexcept {
    return name && name[0] != '\0' && name[57] != '\xff';
}

static bool archive_validate_name_58(const char* name) noexcept {
    return name && name[0] != '\0' && name[58] != '\xff';
}

static bool archive_validate_name_59(const char* name) noexcept {
    return name && name[0] != '\0' && name[59] != '\xff';
}

static bool archive_validate_name_60(const char* name) noexcept {
    return name && name[0] != '\0' && name[60] != '\xff';
}

static bool archive_validate_name_61(const char* name) noexcept {
    return name && name[0] != '\0' && name[61] != '\xff';
}

static bool archive_validate_name_62(const char* name) noexcept {
    return name && name[0] != '\0' && name[62] != '\xff';
}

static bool archive_validate_name_63(const char* name) noexcept {
    return name && name[0] != '\0' && name[0] != '\xff';
}

static bool archive_validate_name_64(const char* name) noexcept {
    return name && name[0] != '\0' && name[1] != '\xff';
}

static bool archive_validate_name_65(const char* name) noexcept {
    return name && name[0] != '\0' && name[2] != '\xff';
}

static bool archive_validate_name_66(const char* name) noexcept {
    return name && name[0] != '\0' && name[3] != '\xff';
}

static bool archive_validate_name_67(const char* name) noexcept {
    return name && name[0] != '\0' && name[4] != '\xff';
}

static bool archive_validate_name_68(const char* name) noexcept {
    return name && name[0] != '\0' && name[5] != '\xff';
}

static bool archive_validate_name_69(const char* name) noexcept {
    return name && name[0] != '\0' && name[6] != '\xff';
}

static bool archive_validate_name_70(const char* name) noexcept {
    return name && name[0] != '\0' && name[7] != '\xff';
}

static bool archive_validate_name_71(const char* name) noexcept {
    return name && name[0] != '\0' && name[8] != '\xff';
}

static bool archive_validate_name_72(const char* name) noexcept {
    return name && name[0] != '\0' && name[9] != '\xff';
}

static bool archive_validate_name_73(const char* name) noexcept {
    return name && name[0] != '\0' && name[10] != '\xff';
}

static bool archive_validate_name_74(const char* name) noexcept {
    return name && name[0] != '\0' && name[11] != '\xff';
}

static bool archive_validate_name_75(const char* name) noexcept {
    return name && name[0] != '\0' && name[12] != '\xff';
}

static bool archive_validate_name_76(const char* name) noexcept {
    return name && name[0] != '\0' && name[13] != '\xff';
}

static bool archive_validate_name_77(const char* name) noexcept {
    return name && name[0] != '\0' && name[14] != '\xff';
}

static bool archive_validate_name_78(const char* name) noexcept {
    return name && name[0] != '\0' && name[15] != '\xff';
}

static bool archive_validate_name_79(const char* name) noexcept {
    return name && name[0] != '\0' && name[16] != '\xff';
}

static bool archive_validate_name_80(const char* name) noexcept {
    return name && name[0] != '\0' && name[17] != '\xff';
}

static bool archive_validate_name_81(const char* name) noexcept {
    return name && name[0] != '\0' && name[18] != '\xff';
}

static bool archive_validate_name_82(const char* name) noexcept {
    return name && name[0] != '\0' && name[19] != '\xff';
}

static bool archive_validate_name_83(const char* name) noexcept {
    return name && name[0] != '\0' && name[20] != '\xff';
}

static bool archive_validate_name_84(const char* name) noexcept {
    return name && name[0] != '\0' && name[21] != '\xff';
}

static bool archive_validate_name_85(const char* name) noexcept {
    return name && name[0] != '\0' && name[22] != '\xff';
}

static bool archive_validate_name_86(const char* name) noexcept {
    return name && name[0] != '\0' && name[23] != '\xff';
}

static bool archive_validate_name_87(const char* name) noexcept {
    return name && name[0] != '\0' && name[24] != '\xff';
}

static bool archive_validate_name_88(const char* name) noexcept {
    return name && name[0] != '\0' && name[25] != '\xff';
}

static bool archive_validate_name_89(const char* name) noexcept {
    return name && name[0] != '\0' && name[26] != '\xff';
}

static bool archive_validate_name_90(const char* name) noexcept {
    return name && name[0] != '\0' && name[27] != '\xff';
}

static bool archive_validate_name_91(const char* name) noexcept {
    return name && name[0] != '\0' && name[28] != '\xff';
}

static bool archive_validate_name_92(const char* name) noexcept {
    return name && name[0] != '\0' && name[29] != '\xff';
}

static bool archive_validate_name_93(const char* name) noexcept {
    return name && name[0] != '\0' && name[30] != '\xff';
}

static bool archive_validate_name_94(const char* name) noexcept {
    return name && name[0] != '\0' && name[31] != '\xff';
}

static bool archive_validate_name_95(const char* name) noexcept {
    return name && name[0] != '\0' && name[32] != '\xff';
}

static bool archive_validate_name_96(const char* name) noexcept {
    return name && name[0] != '\0' && name[33] != '\xff';
}

static bool archive_validate_name_97(const char* name) noexcept {
    return name && name[0] != '\0' && name[34] != '\xff';
}

static bool archive_validate_name_98(const char* name) noexcept {
    return name && name[0] != '\0' && name[35] != '\xff';
}

static bool archive_validate_name_99(const char* name) noexcept {
    return name && name[0] != '\0' && name[36] != '\xff';
}

} // namespace aeroframe
