#include <gtest/gtest.h>

#include <QBuffer>
#include <QByteArray>
#include <QIODevice>
#include <QThread>

#include <chrono>
#include <cstdint>

#include "core/Converter.hpp"

namespace
{

std::uint8_t makeByte(std::uint8_t typeBits, std::uint8_t payloadBits)
{
    return static_cast<std::uint8_t>(((typeBits & 0b11) << 6) | (payloadBits & 0b00111111));
}

template <typename TConverter>
void waitUntilStopped(TConverter& converter,
                      std::chrono::milliseconds timeout = std::chrono::milliseconds(500))
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    while (converter.isRunning() && std::chrono::steady_clock::now() < deadline)
        QThread::msleep(1);
}

} // namespace

TEST(ConverterTests, ConvertsFromInputDeviceToOutputDevice)
{
    QByteArray inputData;
    inputData.append(static_cast<char>(makeByte(0b00, 42))); // "42\n"
    inputData.append(static_cast<char>(makeByte(0b10, 0)));  // "a\n"
    inputData.append(static_cast<char>(makeByte(0b01, 63))); // "-1\n"

    QBuffer inputBuffer(&inputData);
    QByteArray outputData;
    QBuffer outputBuffer(&outputData);

    Converter converter(&inputBuffer, &outputBuffer);

    converter.start();
    waitUntilStopped(converter);

    EXPECT_FALSE(converter.isRunning());
    EXPECT_EQ(outputData.toStdString(), "42\na\n-1\n");
}

TEST(ConverterTests, SkipsInvalidValuesAndWritesOnlyValidDecodedValues)
{
    QByteArray inputData;
    inputData.append(static_cast<char>(makeByte(0b11, 5)));  // invalid reserved type
    inputData.append(static_cast<char>(makeByte(0b10, 26))); // invalid latin letter
    inputData.append(static_cast<char>(makeByte(0b10, 1)));  // "b\n"

    QBuffer inputBuffer(&inputData);
    QByteArray outputData;
    QBuffer outputBuffer(&outputData);

    Converter converter(&inputBuffer, &outputBuffer);

    converter.start();
    waitUntilStopped(converter);

    EXPECT_FALSE(converter.isRunning());
    EXPECT_EQ(outputData.toStdString(), "b\n");
}

TEST(ConverterTests, StopCallStopsLongRunningConversion)
{
    QByteArray inputData;
    for (int i = 0; i < 1000; ++i)
        inputData.append(static_cast<char>(makeByte(0b00, 1))); // "1\n"

    QBuffer inputBuffer(&inputData);
    QByteArray outputData;
    QBuffer outputBuffer(&outputData);

    Converter converter(&inputBuffer, &outputBuffer);

    converter.start();
    converter.stop();
    waitUntilStopped(converter);

    EXPECT_FALSE(converter.isRunning());
}