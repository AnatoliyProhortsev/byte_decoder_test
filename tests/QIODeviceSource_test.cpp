#include <cstdint>
#include <optional>

#include <QBuffer>
#include <QByteArray>

#include <gtest/gtest.h>

#include "core/io/QIODevice/QIODeviceSource.hpp"

TEST(QIODeviceSourceTests, OpenReturnsFalseForNullDevice)
{
    QIODeviceSource source(nullptr);

    EXPECT_FALSE(source.open());
    EXPECT_FALSE(source.isOpen());
}

TEST(QIODeviceSourceTests, OpenReturnsTrueForValidDevice)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    EXPECT_TRUE(source.open());
    EXPECT_TRUE(source.isOpen());
}

TEST(QIODeviceSourceTests, OpenReturnsTrueIfDeviceAlreadyOpen)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    ASSERT_TRUE(buffer.open(QIODevice::ReadOnly));

    QIODeviceSource source(&buffer);

    EXPECT_TRUE(source.open());
    EXPECT_TRUE(source.isOpen());
}

TEST(QIODeviceSourceTests, CloseReturnsFalseForNullDevice)
{
    QIODeviceSource source(nullptr);

    EXPECT_FALSE(source.close());
}

TEST(QIODeviceSourceTests, CloseReturnsTrueForClosedDevice)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    EXPECT_TRUE(source.close());
    EXPECT_FALSE(source.isOpen());
}

TEST(QIODeviceSourceTests, CloseReturnsTrueForOpenedDevice)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());
    ASSERT_TRUE(source.isOpen());

    EXPECT_TRUE(source.close());
    EXPECT_FALSE(source.isOpen());
}

TEST(QIODeviceSourceTests, ReadReturnsNulloptWhenDeviceIsNotOpen)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    const auto result = source.read();

    EXPECT_FALSE(result.has_value());
}

TEST(QIODeviceSourceTests, ReadReturnsSingleByte)
{
    QByteArray input;
    input.append(static_cast<char>(0x2A));

    QBuffer buffer(&input);
    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());

    const auto result = source.read();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, static_cast<std::uint8_t>(0x2A));
}

TEST(QIODeviceSourceTests, ReadReturnsMultipleBytesSequentially)
{
    QByteArray input;
    input.append(static_cast<char>(0x01));
    input.append(static_cast<char>(0x7F));
    input.append(static_cast<char>(0xA5));

    QBuffer buffer(&input);
    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());

    const auto first = source.read();
    const auto second = source.read();
    const auto third = source.read();

    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    ASSERT_TRUE(third.has_value());

    EXPECT_EQ(*first, static_cast<std::uint8_t>(0x01));
    EXPECT_EQ(*second, static_cast<std::uint8_t>(0x7F));
    EXPECT_EQ(*third, static_cast<std::uint8_t>(0xA5));
}

TEST(QIODeviceSourceTests, ReadReturnsNulloptAtEndOfStream)
{
    QByteArray input;
    input.append(static_cast<char>(0x55));

    QBuffer buffer(&input);
    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());

    const auto first = source.read();
    const auto second = source.read();

    ASSERT_TRUE(first.has_value());
    EXPECT_FALSE(second.has_value());
}

TEST(QIODeviceSourceTests, EosReturnsTrueForNullDevice)
{
    QIODeviceSource source(nullptr);

    EXPECT_TRUE(source.eos());
}

TEST(QIODeviceSourceTests, EosReturnsTrueForEmptyBuffer)
{
    QByteArray input;
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());
    EXPECT_TRUE(source.eos());
}

TEST(QIODeviceSourceTests, EosReturnsFalseBeforeReadingAllData)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());
    EXPECT_FALSE(source.eos());
}

TEST(QIODeviceSourceTests, EosReturnsTrueAfterReadingAllData)
{
    QByteArray input("abc");
    QBuffer buffer(&input);

    QIODeviceSource source(&buffer);

    ASSERT_TRUE(source.open());

    EXPECT_FALSE(source.eos());

    ASSERT_TRUE(source.read().has_value());
    EXPECT_FALSE(source.eos());

    ASSERT_TRUE(source.read().has_value());
    EXPECT_FALSE(source.eos());

    ASSERT_TRUE(source.read().has_value());
    EXPECT_TRUE(source.eos());
}