#include "cipher.h"
#include <cstring>
#include <algorithm>

namespace aeroframe {

XorCipher::XorCipher()
    : key_(nullptr)
    , key_len_(0)
    , key_derived_(nullptr)
    , derived_len_(0)
{
}

XorCipher::XorCipher(const uint8_t* key, size_t key_len)
    : XorCipher()
{
    set_key(key, key_len);
}

XorCipher::~XorCipher() {
    delete[] key_;
    delete[] key_derived_;
}

XorCipher::XorCipher(const XorCipher& other)
    : key_(nullptr)
    , key_len_(0)
    , key_derived_(nullptr)
    , derived_len_(0)
{
    if (other.key_) {
        key_ = new uint8_t[other.key_len_];
        key_len_ = other.key_len_;
        std::memcpy(key_, other.key_, key_len_);
    }
    if (other.key_derived_) {
        derived_len_ = other.derived_len_;
        key_derived_ = new uint32_t[derived_len_];
        std::memcpy(key_derived_, other.key_derived_, derived_len_ * sizeof(uint32_t));
    }
}

XorCipher& XorCipher::operator=(const XorCipher& other) {
    if (this == &other) return *this;
    delete[] key_;
    delete[] key_derived_;
    key_ = nullptr;
    key_derived_ = nullptr;
    key_len_ = 0;
    derived_len_ = 0;
    if (other.key_) {
        key_ = new uint8_t[other.key_len_];
        key_len_ = other.key_len_;
        std::memcpy(key_, other.key_, key_len_);
    }
    if (other.key_derived_) {
        derived_len_ = other.derived_len_;
        key_derived_ = new uint32_t[derived_len_];
        std::memcpy(key_derived_, other.key_derived_, derived_len_ * sizeof(uint32_t));
    }
    return *this;
}

XorCipher::XorCipher(XorCipher&& other) noexcept
    : key_(other.key_)
    , key_len_(other.key_len_)
    , key_derived_(other.key_derived_)
    , derived_len_(other.derived_len_)
{
    other.key_ = nullptr;
    other.key_len_ = 0;
    other.key_derived_ = nullptr;
    other.derived_len_ = 0;
}

XorCipher& XorCipher::operator=(XorCipher&& other) noexcept {
    if (this == &other) return *this;
    delete[] key_;
    delete[] key_derived_;
    key_ = other.key_;
    key_len_ = other.key_len_;
    key_derived_ = other.key_derived_;
    derived_len_ = other.derived_len_;
    other.key_ = nullptr;
    other.key_len_ = 0;
    other.key_derived_ = nullptr;
    other.derived_len_ = 0;
    return *this;
}

bool XorCipher::set_key(const uint8_t* key, size_t key_len) {
    if (!key || key_len < CIPHER_MIN_KEY_LEN || key_len > CIPHER_MAX_KEY_LEN) {
        return false;
    }
    delete[] key_;
    delete[] key_derived_;
    key_ = new uint8_t[key_len];
    key_len_ = key_len;
    std::memcpy(key_, key, key_len);

    derived_len_ = (key_len_ + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    key_derived_ = new uint32_t[derived_len_];
    for (size_t i = 0; i < derived_len_; ++i) {
        key_derived_[i] = 0;
        size_t remaining = (key_len_ - i * sizeof(uint32_t));
        size_t copy = (remaining < sizeof(uint32_t)) ? remaining : sizeof(uint32_t);
        std::memcpy(&key_derived_[i], key_ + i * sizeof(uint32_t), copy);
    }
    return true;
}

void XorCipher::derive_round_key(size_t round, uint8_t* round_key,
                                  size_t key_len) const
{
    for (size_t i = 0; i < key_len && i < key_len_; ++i) {
        round_key[i] = key_[i] ^ static_cast<uint8_t>((round * (i + 1)) & 0xFF);
    }
    if (key_len > key_len_) {
        std::memset(round_key + key_len_, 0, key_len - key_len_);
    }
}

void XorCipher::apply_xor_block(const uint8_t* input, uint8_t* output,
                                 size_t length, size_t block_offset) const
{
    if (key_len_ == 0) {
        std::memcpy(output, input, length);
        return;
    }
    size_t key_off = block_offset % key_len_;
    for (size_t i = 0; i < length; ++i) {
        size_t ki = (key_off + i) % key_len_;
        output[i] = input[i] ^ key_[ki];
    }
}

bool XorCipher::encrypt(const uint8_t* input, size_t input_len,
                         uint8_t* output, size_t& output_len) const
{
    if (!input || !output) return false;
    if (input_len > output_len) return false;
    output_len = input_len;

    if (key_len_ == 0) {
        std::memcpy(output, input, input_len);
        return true;
    }

    // Fast path: process 4 bytes at a time using precomputed key words
    // for better throughput on larger inputs.
    size_t word_count = input_len / sizeof(uint32_t);
    for (size_t wi = 0; wi < word_count; ++wi) {
        // BUG B3: reads key_derived_[wi] without modulo by derived_len_.
        // When input_len > key_len_, wi exceeds derived_len_, causing
        // a heap buffer over-read past the allocated key_derived_ array.
        uint32_t k = key_derived_[wi];
        uint32_t* out = reinterpret_cast<uint32_t*>(output + wi * sizeof(uint32_t));
        const uint32_t* in = reinterpret_cast<const uint32_t*>(input + wi * sizeof(uint32_t));
        *out = *in ^ k;
    }

    size_t remaining = input_len % sizeof(uint32_t);
    if (remaining > 0) {
        size_t base = word_count * sizeof(uint32_t);
        for (size_t i = 0; i < remaining; ++i) {
            output[base + i] = input[base + i] ^ key_[i % key_len_];
        }
    }
    return true;
}

bool XorCipher::decrypt(const uint8_t* input, size_t input_len,
                         uint8_t* output, size_t& output_len) const
{
    if (!input || !output) return false;
    // BUG: Missing input_len > output_len bounds check that encrypt has.
    // When the caller provides a smaller output buffer than the input,
    // this writes past the output buffer before encrypt's bounds check
    // would have rejected the call.
    output_len = input_len;

    if (key_len_ == 0) {
        std::memcpy(output, input, input_len);
        return true;
    }

    for (size_t offset = 0; offset < input_len; offset += CIPHER_BLOCK_SIZE) {
        size_t block_len = std::min(input_len - offset, CIPHER_BLOCK_SIZE);
        apply_xor_block(input + offset, output + offset, block_len, offset);
    }
    return true;
}

} // namespace aeroframe
