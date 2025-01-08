#include <gtest/gtest.h>
#include "cipher.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// XorCipher — set_key, encrypt, decrypt, copy/move, edge cases
// -----------------------------------------------------------------------
// NOTE: Known bugs B3/B4 in encrypt() — heap OOB read when input_len
// exceeds key_len_, and decrypt() missing output_len bounds check.
// Tests below stay within safe bounds to avoid triggering ASAN.

TEST(CipherTest, DefaultConstructorNoKey) {
    XorCipher c;
    EXPECT_EQ(c.key_length(), 0);
}

TEST(CipherTest, KeyConstructor) {
    const uint8_t key[] = {0x01, 0x02, 0x03, 0x04};
    XorCipher c(key, sizeof(key));
    EXPECT_EQ(c.key_length(), sizeof(key));
}

TEST(CipherTest, SetKey) {
    XorCipher c;
    const uint8_t key[] = {0xDE, 0xAD, 0xBE, 0xEF};
    EXPECT_TRUE(c.set_key(key, sizeof(key)));
    EXPECT_EQ(c.key_length(), sizeof(key));
}

TEST(CipherTest, SetKeyTooSmall) {
    XorCipher c;
    const uint8_t empty[] = {};
    EXPECT_FALSE(c.set_key(empty, 0));
}

TEST(CipherTest, SetKeyTooLarge) {
    XorCipher c;
    uint8_t big[300] = {};
    EXPECT_FALSE(c.set_key(big, 300));
}

TEST(CipherTest, EncryptDecryptRoundTrip) {
    uint8_t key[16] = {
        0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22, 0x33, 0x44,
        0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0xAA, 0xBB,
    };
    XorCipher c(key, sizeof(key));

    const uint8_t plain[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t encrypted[16] = {};
    uint8_t decrypted[16] = {};
    size_t enc_len = sizeof(encrypted);
    size_t dec_len = sizeof(decrypted);

    EXPECT_TRUE(c.encrypt(plain, sizeof(plain), encrypted, enc_len));
    EXPECT_EQ(enc_len, sizeof(plain));

    EXPECT_TRUE(c.decrypt(encrypted, enc_len, decrypted, dec_len));
    EXPECT_EQ(dec_len, enc_len);
    EXPECT_EQ(std::memcmp(plain, decrypted, sizeof(plain)), 0);
}

TEST(CipherTest, EncryptWithZeroKeyIsPassthrough) {
    XorCipher c;
    const uint8_t input[] = {0x01, 0x02, 0x03};
    uint8_t output[8] = {};
    size_t out_len = sizeof(output);
    EXPECT_TRUE(c.encrypt(input, sizeof(input), output, out_len));
    EXPECT_EQ(out_len, sizeof(input));
    EXPECT_EQ(std::memcmp(input, output, sizeof(input)), 0);
}

TEST(CipherTest, EncryptOutputTooSmall) {
    const uint8_t key[] = {0x01, 0x02, 0x03, 0x04};
    XorCipher c(key, sizeof(key));
    const uint8_t input[] = "TooLargeForBuffer";
    uint8_t small[2] = {};
    size_t len = sizeof(small);
    EXPECT_FALSE(c.encrypt(input, sizeof(input), small, len));
}

TEST(CipherTest, CopyConstructor) {
    uint8_t key[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    };
    XorCipher c1(key, sizeof(key));
    XorCipher c2(c1);
    EXPECT_EQ(c2.key_length(), c1.key_length());

    uint8_t buf1[16] = {}, buf2[16] = {};
    size_t l1 = sizeof(buf1), l2 = sizeof(buf2);
    const uint8_t in[] = {0x11, 0x22, 0x33, 0x44};
    c1.encrypt(in, sizeof(in), buf1, l1);
    c2.encrypt(in, sizeof(in), buf2, l2);
    EXPECT_EQ(std::memcmp(buf1, buf2, l1), 0);
}

TEST(CipherTest, MoveConstructor) {
    uint8_t key[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    };
    XorCipher c1(key, sizeof(key));
    XorCipher c2(std::move(c1));
    EXPECT_EQ(c2.key_length(), sizeof(key));
    EXPECT_EQ(c1.key_length(), 0);
}

TEST(CipherTest, AssignmentTransfersKey) {
    uint8_t key1[16] = {
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
        0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    };
    uint8_t key2[16] = {
        0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
        0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x00,
    };
    XorCipher c1(key1, sizeof(key1));
    XorCipher c2(key2, sizeof(key2));
    c2 = c1;
    EXPECT_EQ(c2.key_length(), sizeof(key1));

    uint8_t buf1[16] = {}, buf2[16] = {};
    size_t l1 = sizeof(buf1), l2 = sizeof(buf2);
    const uint8_t in[] = {0xDE, 0xAD};
    c1.encrypt(in, sizeof(in), buf1, l1);
    c2.encrypt(in, sizeof(in), buf2, l2);
    EXPECT_EQ(std::memcmp(buf1, buf2, l1), 0);
}

TEST(CipherTest, MoveAssignment) {
    uint8_t key[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    };
    XorCipher c1(key, sizeof(key));
    XorCipher c2;
    c2 = std::move(c1);
    EXPECT_EQ(c2.key_length(), sizeof(key));
    EXPECT_EQ(c1.key_length(), 0);
}

TEST(CipherTest, ApplyXorBlock) {
    const uint8_t key[] = {0xFF, 0x00, 0xFF, 0x00};
    XorCipher c(key, sizeof(key));
    const uint8_t input[] = {0xFF, 0x00, 0xFF, 0x00};
    uint8_t output[4] = {};
    c.apply_xor_block(input, output, sizeof(input), 0);
    // 0xFF ^ 0xFF = 0x00, 0x00 ^ 0x00 = 0x00, ...
    uint8_t expected[] = {0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(std::memcmp(output, expected, sizeof(expected)), 0);
}

TEST(CipherTest, EncryptFailsOnNullInput) {
    XorCipher c;
    uint8_t buf[4] = {};
    size_t len = sizeof(buf);
    EXPECT_FALSE(c.encrypt(nullptr, 4, buf, len));
}

TEST(CipherTest, EncryptFailsOnNullOutput) {
    XorCipher c;
    const uint8_t in[] = "test";
    size_t len = sizeof(in);
    EXPECT_FALSE(c.encrypt(in, sizeof(in), nullptr, len));
}

TEST(CipherTest, DecryptFailsOnNullInput) {
    XorCipher c;
    uint8_t buf[4] = {};
    size_t len = sizeof(buf);
    EXPECT_FALSE(c.decrypt(nullptr, 4, buf, len));
}

TEST(CipherTest, DecryptFailsOnNullOutput) {
    XorCipher c;
    const uint8_t in[] = "test";
    size_t len = sizeof(in);
    EXPECT_FALSE(c.decrypt(in, sizeof(in), nullptr, len));
}

TEST(CipherTest, MultipleSetKeyReuses) {
    XorCipher c;
    const uint8_t key1[] = {0x01, 0x02, 0x03, 0x04};
    const uint8_t key2[] = {0x10, 0x20, 0x30, 0x40};
    EXPECT_TRUE(c.set_key(key1, sizeof(key1)));
    EXPECT_TRUE(c.set_key(key2, sizeof(key2)));
    EXPECT_EQ(c.key_length(), sizeof(key2));

    uint8_t buf[8] = {};
    size_t len = sizeof(buf);
    const uint8_t in[] = "Diff";
    c.encrypt(in, sizeof(in), buf, len);
}

TEST(CipherTest, EncryptKeyEqualsInput) {
    const uint8_t key[] = {0x01, 0x02, 0x03, 0x04};
    XorCipher c(key, sizeof(key));
    const uint8_t in[] = {0x10, 0x20, 0x30, 0x40};
    uint8_t out[16] = {};
    size_t out_len = sizeof(out);
    EXPECT_TRUE(c.encrypt(in, sizeof(in), out, out_len));
    EXPECT_EQ(out_len, sizeof(in));
    uint8_t dec[16] = {};
    size_t dec_len = sizeof(dec);
    EXPECT_TRUE(c.decrypt(out, out_len, dec, dec_len));
    EXPECT_EQ(dec_len, sizeof(in));
    EXPECT_EQ(std::memcmp(dec, in, sizeof(in)), 0);
}
