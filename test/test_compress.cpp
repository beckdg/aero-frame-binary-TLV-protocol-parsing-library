#include <gtest/gtest.h>
#include "compress.h"

using namespace aeroframe;

// -----------------------------------------------------------------------
// RLE decompression
// -----------------------------------------------------------------------

TEST(RleTest, EmptyInput) {
    const uint8_t src[] = {};
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_rle(src, 0, dest, sizeof(dest), written));
    EXPECT_EQ(written, 0);
}

TEST(RleTest, SingleLiteral) {
    const uint8_t src[] = {0x01, 0x41}; // tag=1 literal, value='A'
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
    ASSERT_EQ(written, 1);
    EXPECT_EQ(dest[0], 'A');
}

TEST(RleTest, RepeatRun) {
    const uint8_t src[] = {0x85, 0x2D}; // tag=5 repeats, value='-'
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
    ASSERT_EQ(written, 5);
    for (size_t i = 0; i < 5; ++i)
        EXPECT_EQ(dest[i], '-');
}

TEST(RleTest, MixedLiteralAndRepeat) {
    // literals "ABC" + 5× '-' + literals "DE"
    const uint8_t src[] = {
        0x03, 0x41, 0x42, 0x43,   // literal "ABC"
        0x85, 0x2D,               // repeat '-' five times
        0x02, 0x44, 0x45,         // literal "DE"
    };
    uint8_t dest[32] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
    ASSERT_EQ(written, 10);
    EXPECT_EQ(std::memcmp(dest, "ABC-----DE", 10), 0);
}

TEST(RleTest, ZeroCountLiteralDoesNothing) {
    const uint8_t src[] = {0x00}; // tag=0 literal
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
    EXPECT_EQ(written, 0);
}

TEST(RleTest, ZeroCountRepeatDoesNothing) {
    const uint8_t src[] = {0x80, 0xFF}; // tag=0 repeat, value=0xFF
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
    EXPECT_EQ(written, 0);
}

TEST(RleTest, DestTooSmall) {
    const uint8_t src[] = {0x85, 0x2D}; // 5 repeats
    uint8_t dest[3] = {};
    size_t written = 0;
    EXPECT_FALSE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
}

TEST(RleTest, TruncatedSource) {
    const uint8_t src[] = {0x81}; // tag says repeat, but no value follows
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_FALSE(decompress_rle(src, sizeof(src), dest, sizeof(dest), written));
}

// -----------------------------------------------------------------------
// LZSS decompression
// -----------------------------------------------------------------------

TEST(LzssTest, EmptyInput) {
    const uint8_t src[] = {};
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_lzss(src, 0, dest, sizeof(dest), written));
    EXPECT_EQ(written, 0);
}

TEST(LzssTest, AllLiterals) {
    const uint8_t src[] = {
        0b00000000, // control byte: 8 literals
        'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r',
    };
    uint8_t dest[32] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_lzss(src, sizeof(src), dest, sizeof(dest), written));
    ASSERT_EQ(written, 8);
    EXPECT_EQ(std::memcmp(dest, "HelloWor", 8), 0);
}

TEST(LzssTest, SingleMatch) {
    // Control byte: bits 7..0, MSB first
    // 0b10000000: one match reference + 7 padding bits
    // Match reference: [off_hi(4)|len_enc(4)] [off_lo]
    // We need ring buffer to have data first via literals.
    uint8_t src[] = {
        0b00000000, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', // 8 literals
        0b10000000,                                           // control: 1 match
        0x00 | (3 - LZSS_MIN_MATCH), 0x01,                    // off=1, len=3
    };
    uint8_t dest[64] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress_lzss(src, sizeof(src), dest, sizeof(dest), written));
    ASSERT_GE(written, 11);
    // Byte at dest[8] should match byte at dest[7] (off=1)
    EXPECT_EQ(dest[8], dest[7]);
}

TEST(LzssTest, DestTooSmall) {
    const uint8_t src[] = {
        0b00000000,
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    };
    uint8_t dest[4] = {};
    size_t written = 0;
    EXPECT_FALSE(decompress_lzss(src, sizeof(src), dest, sizeof(dest), written));
}

TEST(LzssTest, TruncatedMatch) {
    const uint8_t src[] = {0b10000000, 0x10}; // control says match, 2 bytes needed but only 1
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_FALSE(decompress_lzss(src, sizeof(src), dest, sizeof(dest), written));
}

TEST(LzssTest, TruncatedLiteral) {
    const uint8_t src[] = {0b00000001, 0x41}; // control says literal but truncated
    // Actually: control=0b00000001 -> bit 7=0 (literal), then bit 6=0, ..., bit 0=1 (literal)
    // After reading literal at bit 7 (one byte), then bit 6,5,4,3,2,1 (7 more literals needed)
    // Then bit 0 is literal too, needs one more byte.
    // Total: 8 literals needed, but only 1 byte available -> truncated
    uint8_t dest[32] = {};
    size_t written = 0;
    EXPECT_FALSE(decompress_lzss(src, sizeof(src), dest, sizeof(dest), written));
}

// -----------------------------------------------------------------------
// Dispatch decompress
// -----------------------------------------------------------------------

TEST(DecompressDispatchTest, RleDispatch) {
    const uint8_t src[] = {0x03, 0x41, 0x42, 0x43};
    uint8_t dest[16] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress(CompressMethod::RLE, src, sizeof(src),
                           dest, sizeof(dest), written));
    ASSERT_EQ(written, 3);
    EXPECT_EQ(std::memcmp(dest, "ABC", 3), 0);
}

TEST(DecompressDispatchTest, LzssDispatch) {
    const uint8_t src[] = {
        0b00000000,
        'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r',
    };
    uint8_t dest[32] = {};
    size_t written = 0;
    EXPECT_TRUE(decompress(CompressMethod::LZSS, src, sizeof(src),
                           dest, sizeof(dest), written));
    ASSERT_EQ(written, 8);
    EXPECT_EQ(std::memcmp(dest, "HelloWor", 8), 0);
}
