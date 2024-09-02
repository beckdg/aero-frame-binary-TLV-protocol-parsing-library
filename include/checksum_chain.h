#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>
namespace aeroframe {
enum class ChecksumAlgo : uint8_t {
    Crc16Ccitt, Crc32, Fletcher16, Fletcher32, Adler32, XorFold
};
class ChecksumChain {
public:
    ChecksumChain() noexcept;
    uint32_t compute(ChecksumAlgo algo, const uint8_t* data, size_t len) const noexcept;
    bool verify_chain(const uint8_t* data, size_t len) const noexcept;
    bool load_chain_spec(const uint8_t* data, size_t len) noexcept;
private:
    uint16_t crc16_ccitt(const uint8_t* data, size_t len) const noexcept;
    uint32_t crc32(const uint8_t* data, size_t len) const noexcept;
    uint16_t fletcher16(const uint8_t* data, size_t len) const noexcept;
    uint32_t fletcher32(const uint8_t* data, size_t len) const noexcept;
    uint32_t adler32(const uint8_t* data, size_t len) const noexcept;
    uint32_t xor_fold(const uint8_t* data, size_t len) const noexcept;
    std::vector<ChecksumAlgo> chain_;
    static std::array<uint32_t, 256> make_crc32_table() noexcept;
    static inline std::array<uint32_t, 256> CRC32_TABLE = make_crc32_table();
};
} // namespace aeroframe
