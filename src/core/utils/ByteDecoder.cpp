#include "core/utils/ByteDecoder.hpp"

#include <string>

std::optional<std::string> ByteDecoder::decode(std::uint8_t src)
{
    const Type type = extractType(src);
    const std::uint8_t dataBits = exctractData(src);

    switch (type)
    {
    case Type::Unsigned:
        return decodeUnsigned(dataBits);

    case Type::Signed:
        return decodeSigned(dataBits);

    case Type::Letter:
        return decodeLetter(dataBits);

    case Type::Reserved:
    default:
        return std::nullopt;
    }
}

ByteDecoder::Type ByteDecoder::extractType(std::uint8_t src)
{
    const std::uint8_t rawType 
        = static_cast<std::uint8_t>((src & kTypeMask) >> kTypeShift);

    return static_cast<Type>(rawType);
}

std::uint8_t ByteDecoder::exctractData(std::uint8_t src)
{
    return static_cast<std::uint8_t>(src & kDataMask);
}

std::string ByteDecoder::decodeUnsigned(std::uint8_t dataBits)
{
    return std::to_string(static_cast<unsigned>(dataBits));
}

std::string ByteDecoder::decodeSigned(std::uint8_t src)
{
    return std::to_string(decodeSigned6Bit(src));
}

std::optional<std::string> ByteDecoder::decodeLetter(std::uint8_t dataBits)
{
    if (dataBits >= kLettersCount)
        return std::nullopt;

    const char letter = static_cast<char>(kFirstLetter + dataBits);
    return std::string(1, letter);
}

int ByteDecoder::decodeSigned6Bit(std::uint8_t src)
{
    const bool isNegative = (src & kSignBit) != 0;
    if (!isNegative)
        return static_cast<int>(src);

    return static_cast<int>(src) - kSignedRangeSize;
}