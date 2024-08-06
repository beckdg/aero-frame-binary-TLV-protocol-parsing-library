#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
namespace aeroframe {
enum class CodecId : uint16_t {
    Raw = 0, Rle = 1, Lzss = 2, Delta = 3, Varint = 4, Bcd = 5
};
class CodecDispatch {
public:
    CodecDispatch() noexcept;
    bool decode(CodecId codec, const uint8_t* in, size_t in_len,
                std::vector<uint8_t>& out) const noexcept;
    bool encode(CodecId codec, const uint8_t* in, size_t in_len,
                std::vector<uint8_t>& out) const noexcept;
    bool load_codec_table(const uint8_t* data, size_t len) noexcept;
private:
    bool decode_delta(const uint8_t* in, size_t in_len, std::vector<uint8_t>& out) const noexcept;
    bool decode_varint(const uint8_t* in, size_t in_len, std::vector<uint8_t>& out) const noexcept;
    bool decode_bcd(const uint8_t* in, size_t in_len, std::vector<uint8_t>& out) const noexcept;
    bool encode_delta(const uint8_t* in, size_t in_len, std::vector<uint8_t>& out) const noexcept;
    bool encode_varint(const uint8_t* in, size_t in_len, std::vector<uint8_t>& out) const noexcept;
    std::vector<CodecId> preferred_order_;
};
} // namespace aeroframe
