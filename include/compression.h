#pragma once

#include <cstdint>
#include <cstddef>

namespace aeroframe {

bool decompress_payload(const uint8_t* src, size_t src_len,
                        uint8_t* dest, size_t max_dest_len,
                        size_t& out_len) noexcept;

} // namespace aeroframe
