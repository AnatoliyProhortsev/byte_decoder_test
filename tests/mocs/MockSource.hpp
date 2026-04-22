#ifndef MOCK_SOURCE_HPP
#define MOCK_SOURCE_HPP

#include <cstdint>
#include <optional>

#include <gmock/gmock.h>

#include "core/interface/ISource.hpp"

class MockSource : public ISource<std::uint8_t>
{
public:
    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(bool, isOpen, (), (const, override));
    MOCK_METHOD((std::optional<std::uint8_t>), read, (), (override));
    MOCK_METHOD(bool, eos, (), (const, override));
};

#endif