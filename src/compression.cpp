#include "compression.h"
#include <cstring>
#include <cstdint>

namespace aeroframe {

/* ---------------------------------------------------------------------------
 * RLE decompression
 *
 * Wire format (tag byte followed by payload):
 *
 *   Tag & 0x80  ->  repeated run
 *      count = tag & 0x7F          (1-127, 0 is degenerate / skipped)
 *      next byte is the value to replicate 'count' times
 *
 *   !(Tag & 0x80) ->  literal span
 *      count = tag & 0x7F          (1-127, 0 is degenerate / skipped)
 *      next 'count' bytes are copied verbatim
 *
 * Returns false on any boundary violation or output overflow.
 * -------------------------------------------------------------------------*/

bool decompress_payload(const uint8_t* src, size_t src_len,
                        uint8_t* dest, size_t max_dest_len,
                        size_t& out_len) noexcept
{
    size_t rp = 0;   // read  cursor (src)
    size_t wp = 0;   // write cursor (dest)

    while (rp < src_len) {
        uint8_t tag = src[rp++];

        if (tag & 0x80) {
            // ---- repeated run ----
            uint8_t count = tag & 0x7Fu;

            if (rp >= src_len) return false;          // need value byte
            uint8_t value = src[rp++];

            if (count == 0) continue;                 // degenerate; skip
            if (wp + static_cast<size_t>(count) > max_dest_len) return false;

            std::memset(dest + wp, value, count);
            wp += count;
        } else {
            // ---- literal span ----
            uint8_t count = tag & 0x7Fu;

            if (count == 0) continue;                 // degenerate; skip

            if (rp + static_cast<size_t>(count) > src_len) return false;
            if (wp + static_cast<size_t>(count) > max_dest_len) return false;

            std::memcpy(dest + wp, src + rp, count);
            rp += count;
            wp += count;
        }
    }

    out_len = wp;
    return true;
}

} // namespace aeroframe
