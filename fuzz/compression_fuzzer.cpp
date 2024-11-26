#include <cstdint>
#include <cstddef>
#include <vector>

#include "compression.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Feed the fuzz data directly as an RLE source stream.  The decompressor
    // must survive arbitrary tag-byte combinations, boundary straddles, and
    // degenerate count values without over-reading src or over-writing dest.
    constexpr size_t DEST_CAPACITY = 8192;

    uint8_t dest[DEST_CAPACITY];
    size_t  written = 0;

    bool ok = aeroframe::decompress_payload(
        data, size,
        dest, DEST_CAPACITY,
        written);

    // Sanity-check the post-condition: written must never exceed capacity,
    // and on success all bytes up to written must be accessible.
    if (ok) {
        if (written > DEST_CAPACITY) __builtin_trap();
        // Touch the output bytes to prevent dead-code elimination.
        volatile uint8_t sink = 0;
        for (size_t i = 0; i < written; ++i) {
            sink ^= dest[i];
        }
        (void)sink;
    }

    return 0;
}
