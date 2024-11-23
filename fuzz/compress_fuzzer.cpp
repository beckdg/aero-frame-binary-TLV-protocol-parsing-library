#include <cstdint>
#include <cstddef>

#include "compress.h"

// Fuzz all compression algorithms exposed through the compress dispatch.
//
// Input format:
//   [1B method]  [compressed payload...]
//
// Methods:
//   0x00 -> None   (passthrough — no compression)
//   0x01 -> RLE    (run-length encoding)
//   0x02 -> LZSS   (LZ77-style ring-buffer)
//   0x03..0xFF -> invalid (tests default branch)

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 1) return 0;

    uint8_t method_byte = data[0];
    const uint8_t* payload = data + 1;
    size_t payload_len = size - 1;

    auto method = static_cast<aeroframe::CompressMethod>(method_byte);

    // Decompress into a fixed buffer
    constexpr size_t BUF_SIZE = 16384;
    uint8_t dest[BUF_SIZE];
    size_t written = 0;

    // Also test the individual algorithm entry points directly
    // so the fuzzer can find bugs in each code path.

    if (method == aeroframe::CompressMethod::RLE ||
        method_byte > 0x02) {
        // Direct RLE test
        size_t rle_written = 0;
        aeroframe::decompress_rle(payload, payload_len,
                                  dest, BUF_SIZE, rle_written);
        volatile size_t v1 = rle_written;
        (void)v1;
    }

    if (method == aeroframe::CompressMethod::LZSS ||
        method_byte > 0x02) {
        // Direct LZSS test — this exercises the ring-buffer logic
        // including match-offset bounds and self-referencing copies.
        size_t lzss_written = 0;
        aeroframe::decompress_lzss(payload, payload_len,
                                   dest, BUF_SIZE, lzss_written);
        volatile size_t v2 = lzss_written;
        (void)v2;
    }

    // Dispatch-based path
    size_t dispatch_written = 0;
    aeroframe::decompress(method, payload, payload_len,
                          dest, BUF_SIZE, dispatch_written);
    volatile size_t v3 = dispatch_written;
    (void)v3;

    return 0;
}
