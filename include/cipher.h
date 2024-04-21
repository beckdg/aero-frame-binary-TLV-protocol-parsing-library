#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace aeroframe {

constexpr size_t CIPHER_MIN_KEY_LEN = 1;
constexpr size_t CIPHER_MAX_KEY_LEN = 256;
constexpr size_t CIPHER_BLOCK_SIZE = 16;

class XorCipher {
public:
    XorCipher();
    explicit XorCipher(const uint8_t* key, size_t key_len);
    ~XorCipher();

    XorCipher(const XorCipher& other);
    XorCipher& operator=(const XorCipher& other);
    XorCipher(XorCipher&& other) noexcept;
    XorCipher& operator=(XorCipher&& other) noexcept;

    bool set_key(const uint8_t* key, size_t key_len);
    size_t key_length() const { return key_len_; }

    bool encrypt(const uint8_t* input, size_t input_len,
                 uint8_t* output, size_t& output_len) const;

    bool decrypt(const uint8_t* input, size_t input_len,
                 uint8_t* output, size_t& output_len) const;

    void apply_xor_block(const uint8_t* input, uint8_t* output,
                         size_t length, size_t block_offset) const;

private:
    void derive_round_key(size_t round, uint8_t* round_key, size_t key_len) const;

    uint8_t* key_;
    size_t key_len_;
    uint32_t* key_derived_;
    size_t derived_len_;
};

} // namespace aeroframe
