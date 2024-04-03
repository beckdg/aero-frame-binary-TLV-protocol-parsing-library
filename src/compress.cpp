#include "compress.h"
#include <cstring>

namespace aeroframe {

// ---- RLE (existing logic, re-exported under canonical name) ----
bool decompress_rle(const uint8_t* src, size_t src_len,
                    uint8_t* dest, size_t max_dest_len,
                    size_t& out_len) noexcept
{
    size_t rp = 0;
    size_t wp = 0;

    while (rp < src_len) {
        uint8_t tag = src[rp++];
        if (tag & 0x80) {
            uint8_t count = tag & 0x7Fu;
            if (rp >= src_len) return false;
            uint8_t value = src[rp++];
            if (count == 0) continue;
            if (wp + static_cast<size_t>(count) > max_dest_len) return false;
            std::memset(dest + wp, value, count);
            wp += count;
        } else {
            uint8_t count = tag & 0x7Fu;
            if (count == 0) continue;
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

// ---- LZSS ----
// Control-byte format (one byte per 8 operations, MSB first):
//   Bit = 1  -> match reference: next 2 bytes are [off_hi(4)|len_enc(4)] [off_lo]
//   Bit = 0  -> literal: next byte is the literal value
//
//   match_offset = (off_hi << 8) | off_lo   (12-bit, 0-4095)
//   match_length = len_enc + 3               (3..18)

bool decompress_lzss(const uint8_t* src, size_t src_len,
                     uint8_t* dest, size_t max_dest_len,
                     size_t& out_len) noexcept
{
    size_t sp = 0;
    size_t dp = 0;
    uint8_t ring_buf[LZSS_WINDOW_SIZE];
    size_t rp = 0;
    std::memset(ring_buf, 0, LZSS_WINDOW_SIZE);

    while (sp < src_len) {
        uint8_t control = src[sp++];

        for (int bit = 7; bit >= 0 && sp < src_len; --bit) {
            if (control & (1u << bit)) {
                // --- Match reference ---
                if (sp + 2 > src_len) return false;

                uint8_t hi_n_len = src[sp++];
                uint8_t off_lo   = src[sp++];

                uint8_t  len_enc      = hi_n_len & 0x0F;
                uint8_t  off_hi       = (hi_n_len >> 4) & 0x0F;
                uint16_t match_offset = (static_cast<uint16_t>(off_hi) << 8) | off_lo;
                uint16_t match_length = len_enc + LZSS_MIN_MATCH;

                if (dp + match_length > max_dest_len) return false;

                for (uint16_t i = 0; i < match_length; ++i) {
                    size_t read_idx = (rp - match_offset + LZSS_WINDOW_SIZE)
                                      & (LZSS_WINDOW_SIZE - 1);
                    uint8_t byte = ring_buf[read_idx];
                    dest[dp++] = byte;
                    ring_buf[rp] = byte;
                    rp = (rp + 1) & (LZSS_WINDOW_SIZE - 1);
                }
            } else {
                // --- Literal ---
                if (sp >= src_len) return false;
                uint8_t byte = src[sp++];
                if (dp >= max_dest_len) return false;
                dest[dp++] = byte;
                ring_buf[rp] = byte;
                rp = (rp + 1) & (LZSS_WINDOW_SIZE - 1);
            }
        }
    }

    out_len = dp;
    return true;
}

// ---- Dispatch ----
bool decompress(CompressMethod method,
                const uint8_t* src, size_t src_len,
                uint8_t* dest, size_t max_dest_len,
                size_t& out_len) noexcept
{
    switch (method) {
        case CompressMethod::RLE:
            return decompress_rle(src, src_len, dest, max_dest_len, out_len);
        case CompressMethod::LZSS:
            return decompress_lzss(src, src_len, dest, max_dest_len, out_len);
        default:
            return false;
    }
}

} // namespace aeroframe
