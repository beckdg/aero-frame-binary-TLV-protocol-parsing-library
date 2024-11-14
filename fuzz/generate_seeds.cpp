// Standalone utility — compile and run once to produce seed corpus files
// that let LibFuzzer bypass shallow magic-header checks immediately.
//
// Build:
//   g++ -std=c++17 -Iinclude fuzz/generate_seeds.cpp -o generate_seeds
//   ./generate_seeds

#include "aeroframe.h"

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// -----------------------------------------------------------------------
// CRC-16-CCITT using the compile-time table from the project header.
// -----------------------------------------------------------------------
static uint16_t crc16(const uint8_t* buf, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc = (crc << 8) ^ aeroframe::CRC16_TABLE[((crc >> 8) ^ buf[i]) & 0xFF];
    }
    return crc;
}

// -----------------------------------------------------------------------
// Write binary data to a file, creating parent directories as needed.
// -----------------------------------------------------------------------
static bool write_binary(const char* path,
                         const uint8_t* data, size_t len)
{
    fs::path p(path);
    fs::create_directories(p.parent_path());

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        std::fprintf(stderr, "error: cannot write '%s'\n", path);
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(len));
    return ofs.good();
}

// -----------------------------------------------------------------------
// Seed 1 – valid AeroFrame packet
// -----------------------------------------------------------------------
// Layout:
//   [0..3]   Magic       0xAF01AF01
//   [4..5]   Sequence ID 0x0001
//   [6..7]   Length      0x0013 (19)
//   [8..16]  Payload     TLV type=0x0001, len=5, value="Hello"
//   [17..18] CRC16       computed over bytes [0..16]
// -----------------------------------------------------------------------
static bool generate_packet_seed() {
    // 19 bytes total; CRC occupies the last two.
    std::vector<uint8_t> pkt(19, 0);

    pkt[0] = 0xAF; pkt[1] = 0x01; pkt[2] = 0xAF; pkt[3] = 0x01; // magic
    pkt[4] = 0x00; pkt[5] = 0x01;                               // seq_id
    pkt[6] = 0x00; pkt[7] = 0x13;                               // total_length (19)

    // TLV: type=0x0001, length=5, value="Hello"
    pkt[8]  = 0x00; pkt[9]  = 0x01; // type
    pkt[10] = 0x00; pkt[11] = 0x05; // length
    pkt[12] = 'H';  pkt[13] = 'e';
    pkt[14] = 'l';  pkt[15] = 'l';
    pkt[16] = 'o';

    // CRC over everything before the CRC field (bytes 0..16).
    uint16_t crc = crc16(pkt.data(), pkt.size() - 2);
    pkt[17] = static_cast<uint8_t>(crc >> 8);
    pkt[18] = static_cast<uint8_t>(crc & 0xFF);

    return write_binary("fuzz/corpus/packet_fuzzer/seed1.bin",
                        pkt.data(), pkt.size());
}

// -----------------------------------------------------------------------
// Seed 2 – valid RLE-compressed byte sequence
// -----------------------------------------------------------------------
// Encodes:  "ABC" + 5× '-' + "DE"  =  "ABC-----DE"  (9 bytes)
//
//   tag 0x03   literals 41 42 43
//   tag 0x85   run of 5 × 0x2D  ('-')
//   tag 0x02   literals 44 45
// -----------------------------------------------------------------------
static bool generate_compression_seed() {
    const uint8_t rle[] = {
        0x03, 0x41, 0x42, 0x43,   // literal "ABC"
        0x85, 0x2D,               // repeat '-' five times
        0x02, 0x44, 0x45,         // literal "DE"
    };

    return write_binary("fuzz/corpus/compression_fuzzer/seed1.bin",
                        rle, sizeof(rle));
}

// -----------------------------------------------------------------------
// Seed 3 – fragment reassembly: push two fragments, then assemble
// -----------------------------------------------------------------------
// Command format: [1B cmd] [args...]
//   0x00 push: 4B msg_id 2B frag_id 2B total 2B offset 2B len [payload]
//   0x04 assemble: 4B msg_id
// -----------------------------------------------------------------------
static bool generate_fragment_seed() {
    std::vector<uint8_t> seed;
    auto push = [&](uint32_t mid, uint16_t fid, uint16_t tot,
                    uint16_t off, const uint8_t* data, uint16_t len) {
        seed.push_back(0x00);
        auto w32 = [&](uint32_t v) { seed.push_back((v>>24)&0xFF); seed.push_back((v>>16)&0xFF); seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };
        auto w16 = [&](uint16_t v) { seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };
        w32(mid); w16(fid); w16(tot); w16(off); w16(len);
        seed.insert(seed.end(), data, data + len);
    };
    auto assemble = [&](uint32_t mid) {
        seed.push_back(0x04);
        seed.push_back((mid>>24)&0xFF); seed.push_back((mid>>16)&0xFF);
        seed.push_back((mid>>8)&0xFF); seed.push_back(mid&0xFF);
    };
    uint8_t frag0[] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    uint8_t frag1[] = {'!',' ','T','e','s','t','i','n','g','1','2','3'};
    push(1, 0, 2, 0, frag0, sizeof(frag0));
    push(1, 1, 2, 11, frag1, sizeof(frag1));
    assemble(1);
    return write_binary("fuzz/corpus/fragment_fuzzer/seed1.bin",
                        seed.data(), seed.size());
}

// -----------------------------------------------------------------------
// Seed 4 – protocol engine: version declare + capability declare + heartbeat
// -----------------------------------------------------------------------
// [1B msg_type] [body...] per iteration (protocol_fuzzer reads sequentially)
// -----------------------------------------------------------------------
static bool generate_protocol_seed() {
    uint8_t seed[] = {
        0x10, 0x01,                   // VersionDeclare, version=1
        0x20, 0x00, 0x01, 0x00, 0x01, // CapabilityDeclare, id=0, v=1, flags=1
        0x20, 0x01, 0x01, 0x00, 0x02, // CapabilityDeclare, id=1, v=1, flags=2
        0x04,                         // Heartbeat
        0x01, 0x48, 0x65, 0x6C, 0x6C, 0x6F, // Data: "Hello"
    };
    return write_binary("fuzz/corpus/protocol_fuzzer/seed1.bin",
                        seed, sizeof(seed));
}

// -----------------------------------------------------------------------
// Seed 5 – routing table: add a route, lookup, age
// -----------------------------------------------------------------------
// [1B cmd] [args...]
//   0x00 add: 4B id 2B dest 1B prio 1B ttl 2B hop_cnt [hops...]
//   0x02 lookup: 2B dest
//   0x04 age
// -----------------------------------------------------------------------
static bool generate_routing_seed() {
    std::vector<uint8_t> seed;
    auto w32 = [&](uint32_t v) { seed.push_back((v>>24)&0xFF); seed.push_back((v>>16)&0xFF); seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };
    auto w16 = [&](uint16_t v) { seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };

    seed.push_back(0x00); // add route
    w32(100);             // id = 100
    w16(0x0A00);          // dest = 0x0A00
    seed.push_back(2);    // priority = high
    seed.push_back(10);   // ttl = 10
    w16(2);               // hop_count = 2
    w32(0x01010101);      // hop[0]
    w32(0x02020202);      // hop[1]

    seed.push_back(0x02); // lookup
    w16(0x0A00);          // dest = 0x0A00

    seed.push_back(0x04); // age routes

    return write_binary("fuzz/corpus/routing_fuzzer/seed1.bin",
                        seed.data(), seed.size());
}

// -----------------------------------------------------------------------
// Seed 6 – priority queue: enqueue, promote, dequeue
// -----------------------------------------------------------------------
// [1B cmd] [args...]
//   0x00 enqueue: 4B id 2B len [payload]
//   0x02 promote: 4B id
//   0x01 dequeue
// -----------------------------------------------------------------------
static bool generate_queue_seed() {
    std::vector<uint8_t> seed;
    auto w32 = [&](uint32_t v) { seed.push_back((v>>24)&0xFF); seed.push_back((v>>16)&0xFF); seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };
    auto w16 = [&](uint16_t v) { seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };

    seed.push_back(0x00); // enqueue
    w32(1);               // id = 1
    w16(5);               // len = 5
    seed.insert(seed.end(), {'H','e','l','l','o'}); // payload

    seed.push_back(0x00); // enqueue
    w32(2);               // id = 2
    w16(5);               // len = 5
    seed.insert(seed.end(), {'W','o','r','l','d'}); // payload

    seed.push_back(0x02); // promote
    w32(2);               // id = 2

    seed.push_back(0x01); // dequeue

    return write_binary("fuzz/corpus/queue_fuzzer/seed1.bin",
                        seed.data(), seed.size());
}

// -----------------------------------------------------------------------
// Seed 7 – cipher fuzzer: simple encrypt/decrypt round-trip with 4B key
// -----------------------------------------------------------------------
// Format: [4B key_len (BE)] [key...] [4B input_len (BE)] [input...]
// -----------------------------------------------------------------------
static bool generate_cipher_seed() {
    std::vector<uint8_t> seed;
    auto w32 = [&](uint32_t v) { seed.push_back((v>>24)&0xFF); seed.push_back((v>>16)&0xFF); seed.push_back((v>>8)&0xFF); seed.push_back(v&0xFF); };
    w32(4); // key_len
    seed.insert(seed.end(), {0xDE, 0xAD, 0xBE, 0xEF});
    w32(32); // input_len
    seed.insert(seed.end(), {'H','e','l','l','o','F','u','z','z','e','r','E','n','c','r','y','p','t','T','e','s','t','3','2','B','y','t','e','s','!','!','!'});
    return write_binary("fuzz/corpus/cipher_fuzzer/seed1.bin",
                        seed.data(), seed.size());
}

// -----------------------------------------------------------------------
// Seed 8 – packet_fuzzer: encrypted TLV (TYPE_CIPHERED_DATA = 0x07)
// -----------------------------------------------------------------------
// Layout:
//   [0..3]   Magic       0xAF01AF01
//   [4..5]   Sequence ID 0x0002
//   [6..7]   Length      0x0035 (53)
//   [8..n]   TLV: type=0x07, len=0x0029 (41)
//            value: [4B session_id=42] [4B key_material] [ciphertext...]
//   [n+1..n+2] CRC16
// -----------------------------------------------------------------------
static bool generate_cipher_packet_seed() {
    std::vector<uint8_t> pkt(53, 0);
    pkt[0] = 0xAF; pkt[1] = 0x01; pkt[2] = 0xAF; pkt[3] = 0x01;
    pkt[4] = 0x00; pkt[5] = 0x02;
    pkt[6] = 0x00; pkt[7] = 0x35;

    // TLV: TYPE_CIPHERED_DATA (0x07), length=41
    pkt[8]  = 0x00; pkt[9]  = 0x07;
    pkt[10] = 0x00; pkt[11] = 0x29;

    // Value: session_id = 42
    pkt[12] = 0x00; pkt[13] = 0x00; pkt[14] = 0x00; pkt[15] = 0x2A;

    // Ciphertext padding to exercise decrypt path
    for (int i = 16; i < 51; ++i)
        pkt[i] = static_cast<uint8_t>(i & 0xFF);

    uint16_t crc = crc16(pkt.data(), pkt.size() - 2);
    pkt[51] = static_cast<uint8_t>(crc >> 8);
    pkt[52] = static_cast<uint8_t>(crc & 0xFF);

    return write_binary("fuzz/corpus/packet_fuzzer/cipher_seed.bin",
                        pkt.data(), pkt.size());
}

// -----------------------------------------------------------------------
// Seed 9 – packet_fuzzer: routing update TLV (TYPE_ROUTING_UPDATE = 0x08)
// -----------------------------------------------------------------------
// Layout:
//   [0..3]   Magic       0xAF01AF01
//   [4..5]   Sequence ID 0x0003
//   [6..7]   Length      0x0022 (34)
//   [8..n]   TLV: type=0x08, len=0x0016 (22)
//            value: [4B route_id] [2B dest] [1B prio] [1B ttl] [2B hops] [hop*4B]
//   [n+1..n+2] CRC16
// -----------------------------------------------------------------------
static bool generate_routing_packet_seed() {
    std::vector<uint8_t> pkt(34, 0);
    pkt[0] = 0xAF; pkt[1] = 0x01; pkt[2] = 0xAF; pkt[3] = 0x01;
    pkt[4] = 0x00; pkt[5] = 0x03;
    pkt[6] = 0x00; pkt[7] = 0x22;

    // TLV: TYPE_ROUTING_UPDATE (0x08), length=22
    pkt[8]  = 0x00; pkt[9]  = 0x08;
    pkt[10] = 0x00; pkt[11] = 0x16;

    // route_id=100, dest=0x0A00, prio=2, ttl=10, hop_count=2
    pkt[12] = 0x00; pkt[13] = 0x00; pkt[14] = 0x00; pkt[15] = 0x64;
    pkt[16] = 0x0A; pkt[17] = 0x00;
    pkt[18] = 0x02;
    pkt[19] = 0x0A;
    pkt[20] = 0x00; pkt[21] = 0x02;
    pkt[22] = 0x01; pkt[23] = 0x01; pkt[24] = 0x01; pkt[25] = 0x01;
    pkt[26] = 0x02; pkt[27] = 0x02; pkt[28] = 0x02; pkt[29] = 0x02;

    uint16_t crc = crc16(pkt.data(), pkt.size() - 2);
    pkt[32] = static_cast<uint8_t>(crc >> 8);
    pkt[33] = static_cast<uint8_t>(crc & 0xFF);

    return write_binary("fuzz/corpus/packet_fuzzer/routing_seed.bin",
                        pkt.data(), pkt.size());
}

// -----------------------------------------------------------------------
// Seed 10 – compress fuzzer: valid LZSS sequence
// -----------------------------------------------------------------------
// [1B method] [payload...]
// LZSS: control byte bits 7..0, MSB first
//   bit=1 -> match: [off_hi|len_enc] [off_lo]
//   bit=0 -> literal: [byte]
// Encodes "HelloWorld" with one match reference at the end.
// -----------------------------------------------------------------------
static bool generate_compress_seed() {
    uint8_t seed[] = {
        0x02,                         // method = LZSS
        0b00000000,                   // ctrl: 8 literals
        'H','e','l','l','o','W','o','r','l','d',
    };
    return write_binary("fuzz/corpus/compress_fuzzer/seed1.bin",
                        seed, sizeof(seed));
}

// -----------------------------------------------------------------------
// Seed 11 – telemetry fuzzer: GPS fix record batch
// -----------------------------------------------------------------------
static bool generate_telemetry_seed() {
    uint8_t batch[] = {
        0x00, 0x16,  // record length = 22
        0x01,        // GpsFix
        0x00, 0x00, 0x03, 0xE8,  // timestamp 1000
        0x00, 0x98, 0x96, 0x80,  // lat
        0xFF, 0x6A, 0xCB, 0x00,  // lon
        0x00, 0x00, 0x03, 0xE8,  // alt
        0x00, 0x64,              // hdop
        0x03, 0x08,              // fix quality, sats
    };
    return write_binary("fuzz/corpus/telemetry_fuzzer/seed1.bin",
                        batch, sizeof(batch));
}

// -----------------------------------------------------------------------
// Seed 12 – ingest fuzzer: minimal ingest packet
// -----------------------------------------------------------------------
static bool generate_ingest_seed() {
    uint8_t pkt[] = {
        0x49, 0x47,        // magic "IG"
        0x00, 0x01,        // 1 section
        0x00, 0x00, 0x03, 0xE8,  // timestamp
        0x01,              // Config section
        0x00, 0x08,        // section length
        0xCF, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x04,
    };
    return write_binary("fuzz/corpus/ingest_fuzzer/seed1.bin",
                        pkt, sizeof(pkt));
}

// -----------------------------------------------------------------------
int main() {
    bool ok = true;

    std::printf("Generating fuzz seeds...\n");

    ok &= generate_packet_seed();
    if (ok) std::printf("  [OK] packet_fuzzer/seed1.bin\n");

    ok &= generate_compression_seed();
    if (ok) std::printf("  [OK] compression_fuzzer/seed1.bin\n");

    ok &= generate_fragment_seed();
    if (ok) std::printf("  [OK] fragment_fuzzer/seed1.bin\n");

    ok &= generate_protocol_seed();
    if (ok) std::printf("  [OK] protocol_fuzzer/seed1.bin\n");

    ok &= generate_routing_seed();
    if (ok) std::printf("  [OK] routing_fuzzer/seed1.bin\n");

    ok &= generate_queue_seed();
    if (ok) std::printf("  [OK] queue_fuzzer/seed1.bin\n");

    ok &= generate_compress_seed();
    if (ok) std::printf("  [OK] compress_fuzzer/seed1.bin\n");

    ok &= generate_cipher_seed();
    if (ok) std::printf("  [OK] cipher_fuzzer/seed1.bin\n");

    ok &= generate_cipher_packet_seed();
    if (ok) std::printf("  [OK] packet_fuzzer/cipher_seed.bin\n");

    ok &= generate_routing_packet_seed();
    if (ok) std::printf("  [OK] packet_fuzzer/routing_seed.bin\n");

    ok &= generate_telemetry_seed();
    if (ok) std::printf("  [OK] telemetry_fuzzer/seed1.bin\n");

    ok &= generate_ingest_seed();
    if (ok) std::printf("  [OK] ingest_fuzzer/seed1.bin\n");

    if (!ok) {
        std::fprintf(stderr, "seed generation FAILED\n");
        return 1;
    }
    std::printf("done.\n");
    return 0;
}
