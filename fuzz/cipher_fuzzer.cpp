#include <cstdint>
#include <cstddef>
#include <cstring>

#include "cipher.h"

// Fuzz the XorCipher implementation.
//
// Input format:
//   [4B key_len (BE)] [key...] [4B input_len (BE)] [input...]
//
// The fuzzer exercises set_key, encrypt, and decrypt with varying
// key sizes and input lengths to probe the derived-key fast path
// and the asymmetric encrypt/decrypt bounds checking.

static uint32_t r32(const uint8_t* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) <<  8) |
            static_cast<uint32_t>(p[3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 8) return 0;

    size_t off = 0;
    uint32_t key_len = r32(data + off); off += 4;
    if (off + key_len > size) return 0;

    const uint8_t* key = data + off; off += key_len;

    if (off + 4 > size) return 0;
    uint32_t input_len = r32(data + off); off += 4;
    if (off + input_len > size) return 0;

    const uint8_t* input = data + off;

    aeroframe::XorCipher cipher;
    cipher.set_key(key, static_cast<size_t>(key_len));

    // Encrypt
    uint8_t enc_out[8192];
    size_t enc_len = sizeof(enc_out);
    cipher.encrypt(input, input_len, enc_out, enc_len);

    // Decrypt back
    uint8_t dec_out[8192];
    size_t dec_len = sizeof(dec_out);
    cipher.decrypt(enc_out, enc_len, dec_out, dec_len);

    // Also test with a second key to exercise set_key re-initialization
    if (size > 16) {
        uint8_t alt_key[4] = {0x01, 0x02, 0x03, 0x04};
        cipher.set_key(alt_key, 4);
        size_t alt_len = sizeof(enc_out);
        cipher.encrypt(input, input_len, enc_out, alt_len);
    }

    // Test decrypt with small output buffer to exercise the missing
    // bounds check path
    if (input_len > 0) {
        uint8_t small_buf[4];
        size_t small_len = sizeof(small_buf);
        cipher.decrypt(input, input_len, small_buf, small_len);
    }

    return 0;
}
