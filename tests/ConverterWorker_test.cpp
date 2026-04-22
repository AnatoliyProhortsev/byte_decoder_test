#include <chrono>
#include <cstdint>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/workers/ConverterWorker.hpp"
#include "mocks/MockSink.hpp"
#include "mocks/MockSource.hpp"

using ::testing::InSequence;
using ::testing::Return;

namespace
{
    std::uint8_t makeByte(std::uint8_t type, std::uint8_t data)
    {
        return static_cast<std::uint8_t>((type << 6) | (data & 0b00111111));
    }

    void waitUntilStopped(ConverterWorker& worker,
                          std::chrono::milliseconds timeout = std::chrono::milliseconds(200))
    {
        const auto deadline = std::chrono::steady_clock::now() + timeout;

        while (worker.isRunning() && std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

TEST(ConverterWorkerTests, IsNotRunningBeforeStart)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    ConverterWorker worker(source, sink);

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, OpensSourceAndSinkAndStopsOnEos)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    {
        InSequence seq;

        EXPECT_CALL(*source, open()).WillOnce(Return(true));
        EXPECT_CALL(*sink, open()).WillOnce(Return(true));

        EXPECT_CALL(*source, read()).WillOnce(Return(std::nullopt));
        EXPECT_CALL(*source, eos()).WillOnce(Return(true));

        EXPECT_CALL(*sink, flush());
        EXPECT_CALL(*sink, close()).WillOnce(Return(true));
        EXPECT_CALL(*source, close()).WillOnce(Return(true));
    }

    ConverterWorker worker(source, sink);

    worker.start();
    waitUntilStopped(worker);

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, WritesDecodedValuesWithNewlineSeparator)
{
    auto source = std::make_shared<MockSource>();
    auto sink   = std::make_shared<MockSink>();

    ConverterWorker worker(source, sink);

    {
        ::testing::InSequence seq;

        EXPECT_CALL(*source, open()).WillOnce(::testing::Return(true));
        EXPECT_CALL(*sink, open()).WillOnce(::testing::Return(true));

        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b00, 42)}));
        EXPECT_CALL(*sink, write("42\n"));

        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b10, 0)}));
        EXPECT_CALL(*sink, write("a\n"));

        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b01, 63)}));
        EXPECT_CALL(*sink, write("-1\n"));

        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::nullopt));

        EXPECT_CALL(*source, eos()).WillOnce(Return(true));

        EXPECT_CALL(*sink, flush());
        EXPECT_CALL(*sink, close());
        EXPECT_CALL(*source, close());
    }

    worker.start();
    waitUntilStopped(worker);

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, SkipsInvalidDecodedValues)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    ConverterWorker worker(source, sink);

    {
        ::testing::InSequence seq;

        EXPECT_CALL(*source, open()).WillOnce(Return(true));
        EXPECT_CALL(*sink, open()).WillOnce(Return(true));

        // invalid: reserved type 0b11 -> skip
        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b11, 42)}));

        // invalid: latin index out of range -> skip
        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b10, 26)}));

        // valid signed -> write
        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b01, 0b100000)}));
        EXPECT_CALL(*sink, write("-32\n"));

        // valid latin -> write
        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::optional<std::uint8_t>{makeByte(0b10, 0)}));
        EXPECT_CALL(*sink, write("a\n"));

        // end of stream
        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::nullopt));
        EXPECT_CALL(*source, eos()).WillOnce(Return(true));

        EXPECT_CALL(*sink, flush());
        EXPECT_CALL(*sink, close()).WillOnce(Return(true));
        EXPECT_CALL(*source, close()).WillOnce(Return(true));
    }

    worker.start();
    waitUntilStopped(worker);

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, StopsImmediatelyIfSourceOpenFails)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    EXPECT_CALL(*source, open()).WillOnce(Return(false));
    EXPECT_CALL(*sink, open()).Times(0);
    EXPECT_CALL(*source, close()).Times(0);
    EXPECT_CALL(*sink, flush()).Times(0);
    EXPECT_CALL(*sink, close()).Times(0);

    ConverterWorker worker(source, sink);

    worker.start();
    waitUntilStopped(worker);

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, StopsIfSinkOpenFailsAndClosesSource)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    {
        InSequence seq;

        EXPECT_CALL(*source, open()).WillOnce(Return(true));
        EXPECT_CALL(*sink, open()).WillOnce(Return(false));
        EXPECT_CALL(*source, close()).WillOnce(Return(true));
    }

    EXPECT_CALL(*sink, flush()).Times(0);
    EXPECT_CALL(*sink, close()).Times(0);

    ConverterWorker worker(source, sink);

    worker.start();
    waitUntilStopped(worker);

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, StopCanBeCalledBeforeStart)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    ConverterWorker worker(source, sink);

    worker.stop();

    EXPECT_FALSE(worker.isRunning());
}

TEST(ConverterWorkerTests, SecondStartDoesNotStartSecondProcessingLoop)
{
    auto source = std::make_shared<MockSource>();
    auto sink = std::make_shared<MockSink>();

    {
        InSequence seq;

        EXPECT_CALL(*source, open()).WillOnce(Return(true));
        EXPECT_CALL(*sink, open()).WillOnce(Return(true));

        EXPECT_CALL(*source, read())
            .WillOnce(Return(std::nullopt));
        EXPECT_CALL(*source, eos())
            .WillOnce(Return(true));

        EXPECT_CALL(*sink, flush());
        EXPECT_CALL(*sink, close()).WillOnce(Return(true));
        EXPECT_CALL(*source, close()).WillOnce(Return(true));
    }

    ConverterWorker worker(source, sink);

    worker.start();
    worker.start();

    waitUntilStopped(worker);

    EXPECT_FALSE(worker.isRunning());
}