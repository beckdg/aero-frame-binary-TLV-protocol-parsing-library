#pragma once

#include <cstdint>
#include <cstddef>

namespace aeroframe {

enum class CompressMethod : uint8_t {
    None = 0,
    RLE = 1,
    LZSS = 2,
};

// ---- RLE (existing) ----
bool decompress_rle(const uint8_t* src, size_t src_len,
                    uint8_t* dest, size_t max_dest_len,
                    size_t& out_len) noexcept;

// ---- LZSS ----
// Wire format:
//   Control byte (1)  ->  bits 0-3 = match_length-3, bits 4-7 = match_offset_hi
//   Followed by match_offset_lo (1 byte) for matches
//   Or literal bytes for literal flags
//
//   A 4-bit match_length field encodes 3..18 bytes.
//   A 12-bit match_offset encodes 1..4096 positions back.

constexpr size_t LZSS_WINDOW_SIZE = 4096;
constexpr size_t LZSS_MIN_MATCH = 3;
constexpr size_t LZSS_MAX_MATCH = 18;

bool decompress_lzss(const uint8_t* src, size_t src_len,
                     uint8_t* dest, size_t max_dest_len,
                     size_t& out_len) noexcept;

// ---- Dispatch ----
bool decompress(CompressMethod method,
                const uint8_t* src, size_t src_len,
                uint8_t* dest, size_t max_dest_len,
                size_t& out_len) noexcept;

} // namespace aeroframe
