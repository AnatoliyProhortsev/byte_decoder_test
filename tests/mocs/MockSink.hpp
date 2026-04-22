#ifndef MOCK_SINK_HPP
#define MOCK_SINK_HPP

#include <string>

#include <gmock/gmock.h>

#include "core/interface/ISink.hpp"

class MockSink : public ISink<std::string>
{
public:
    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, isOpen, (), (const, override));
    MOCK_METHOD(void, write, (const std::string& data), (override));
    MOCK_METHOD(void, flush, (), (override));
};

#endif