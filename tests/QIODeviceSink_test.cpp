#include <QBuffer>
#include <QByteArray>

#include <gtest/gtest.h>

#include "core/io/QIODevice/QIODeviceSink.hpp"

TEST(QIODeviceSinkTests, OpenReturnsFalseForNullDevice)
{
    QIODeviceSink sink(nullptr);

    EXPECT_FALSE(sink.open());
    EXPECT_FALSE(sink.isOpen());
}

TEST(QIODeviceSinkTests, OpenReturnsTrueForValidDevice)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    EXPECT_TRUE(sink.open());
    EXPECT_TRUE(sink.isOpen());
}

TEST(QIODeviceSinkTests, WriteDoesNothingWhenDeviceIsNotOpen)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);
    sink.write("hello");

    EXPECT_TRUE(output.isEmpty());
}

TEST(QIODeviceSinkTests, WriteStoresSingleStringIntoBuffer)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    ASSERT_TRUE(sink.open());
    sink.write("hello");

    EXPECT_EQ(output.toStdString(), "hello");
}

TEST(QIODeviceSinkTests, WriteAppendsMultipleStrings)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    ASSERT_TRUE(sink.open());
    sink.write("abc");
    sink.write("123");
    sink.write("xyz");

    EXPECT_EQ(output.toStdString(), "abc123xyz");
}

TEST(QIODeviceSinkTests, FlushDoesNothingWhenDeviceIsNotOpen)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    sink.flush();

    EXPECT_TRUE(output.isEmpty());
}

TEST(QIODeviceSinkTests, FlushDoesNotCorruptWrittenData)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    ASSERT_TRUE(sink.open());
    sink.write("data");
    sink.flush();

    EXPECT_EQ(output.toStdString(), "data");
}

TEST(QIODeviceSinkTests, CloseReturnsTrueForOpenedDevice)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    ASSERT_TRUE(sink.open());
    ASSERT_TRUE(sink.isOpen());

    EXPECT_TRUE(sink.close());
    EXPECT_FALSE(sink.isOpen());
}

TEST(QIODeviceSinkTests, CloseReturnsFalseForAlreadyClosedDevice)
{
    QByteArray output;
    QBuffer buffer(&output);

    QIODeviceSink sink(&buffer);

    EXPECT_FALSE(sink.close());
}

TEST(QIODeviceSinkTests, CloseReturnsFalseForNullDevice)
{
    QIODeviceSink sink(nullptr);

    EXPECT_FALSE(sink.close());
}