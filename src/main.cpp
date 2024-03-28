#include "aeroframe.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

// -----------------------------------------------------------------------
// Read entire binary file into a byte vector, or return empty on failure.
// -----------------------------------------------------------------------
static std::vector<uint8_t> readFile(const char* path) {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::fprintf(stderr, "error: cannot open '%s'\n", path);
        return {};
    }
    auto end = ifs.tellg();
    if (end <= 0) {
        std::fprintf(stderr, "error: file is empty\n");
        return {};
    }
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buf(static_cast<size_t>(end));
    if (!ifs.read(reinterpret_cast<char*>(buf.data()), buf.size())) {
        std::fprintf(stderr, "error: failed to read '%s'\n", path);
        return {};
    }
    return buf;
}

// -----------------------------------------------------------------------
// Pretty-print parsed results
// -----------------------------------------------------------------------
static void printPacket(const aeroframe::ParsedPacket& pkt) {
    using aeroframe::errorToString;

    std::printf("=== AeroFrame Packet ===\n");

    if (pkt.error != aeroframe::ErrorCode::None) {
        std::printf("Status: ERROR — %s\n\n", errorToString(pkt.error).data());
        return;
    }

    std::printf("Magic:        0x%08X\n", pkt.header.magic);
    std::printf("Sequence ID:  0x%04X\n", pkt.header.sequence_id);
    std::printf("Total Length: %u\n",     pkt.header.total_length);
    std::printf("CRC:          0x%04X (valid)\n", pkt.crc);
    std::printf("Status:       OK\n\n");

    if (pkt.tlvs.empty()) {
        std::printf("(no TLV chunks)\n");
        return;
    }

    std::printf("TLV Chunks (%zu):\n", pkt.tlvs.size());
    for (size_t i = 0; i < pkt.tlvs.size(); ++i) {
        const auto& tlv = pkt.tlvs[i];
        std::printf("  [%zu] Type: 0x%04X  Length: %u\n", i, tlv.type, tlv.length);

        // Print value bytes as hex, up to a reasonable limit
        constexpr size_t MAX_PRINT = 32;
        size_t show = (tlv.value.size() <= MAX_PRINT) ? tlv.value.size() : MAX_PRINT;

        std::printf("        Value: ");
        for (size_t j = 0; j < show; ++j) {
            std::printf("%02X ", tlv.value[j]);
        }
        if (tlv.value.size() > MAX_PRINT) {
            std::printf("... (+%zu more bytes)", tlv.value.size() - MAX_PRINT);
        }
        std::printf("\n");
    }
}

// -----------------------------------------------------------------------
int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: aeroframe_parser <binary-file>\n");
        return 1;
    }

    auto buf = readFile(argv[1]);
    if (buf.empty()) {
        return 1;
    }

    aeroframe::AeroParser parser;
    const auto result = parser.parse(buf);
    printPacket(result);

    return (result.error == aeroframe::ErrorCode::None) ? 0 : 1;
}
