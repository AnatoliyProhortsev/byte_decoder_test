#include <cstdint>
#include <optional>
#include <string>

#include <gtest/gtest.h>

#include "core/utils/ByteDecoder.hpp"

namespace
{
    std::uint8_t makeByte(std::uint8_t type, std::uint8_t data)
    {
        return static_cast<std::uint8_t>((type << 6) | (data & 0b00111111));
    }
}

TEST(ByteDecoderTests, DecodesUnsignedZero)
{
    const auto result = ByteDecoder::decode(makeByte(0b00, 0));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "0");
}

TEST(ByteDecoderTests, DecodesUnsignedMax)
{
    const auto result = ByteDecoder::decode(makeByte(0b00, 63));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "63");
}

TEST(ByteDecoderTests, DecodesUnsignedArbitraryValue)
{
    const auto result = ByteDecoder::decode(makeByte(0b00, 42));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "42");
}

TEST(ByteDecoderTests, DecodesSignedZero)
{
    const auto result = ByteDecoder::decode(makeByte(0b01, 0b000000));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "0");
}

TEST(ByteDecoderTests, DecodesSignedPositiveMax)
{
    const auto result = ByteDecoder::decode(makeByte(0b01, 0b011111));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "31");
}

TEST(ByteDecoderTests, DecodesSignedMinusOne)
{
    const auto result = ByteDecoder::decode(makeByte(0b01, 0b111111));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "-1");
}

TEST(ByteDecoderTests, DecodesSignedNegativeMin)
{
    const auto result = ByteDecoder::decode(makeByte(0b01, 0b100000));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "-32");
}

TEST(ByteDecoderTests, DecodesLatinLetterA)
{
    const auto result = ByteDecoder::decode(makeByte(0b10, 0));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "a");
}

TEST(ByteDecoderTests, DecodesLatinLetterB)
{
    const auto result = ByteDecoder::decode(makeByte(0b10, 1));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "b");
}

TEST(ByteDecoderTests, DecodesLatinLetterZ)
{
    const auto result = ByteDecoder::decode(makeByte(0b10, 25));

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, "z");
}

TEST(ByteDecoderTests, RejectsInvalidLatinLetterCode)
{
    const auto result = ByteDecoder::decode(makeByte(0b10, 26));

    EXPECT_FALSE(result.has_value());
}

TEST(ByteDecoderTests, RejectsReservedType)
{
    const auto result = ByteDecoder::decode(makeByte(0b11, 0));

    EXPECT_FALSE(result.has_value());
}