#ifndef BYTE_DECODER_HPP
#define BYTE_DECODER_HPP

#include <cstdint>
#include <optional>
#include <string>

class ByteDecoder
{
public:
    enum class Type : std::uint8_t
    {
        Unsigned = 0b00,
        Signed   = 0b01,
        Letter   = 0b10,
        Reserved = 0b11
    };

    static std::optional<std::string> decode(std::uint8_t byte);

// helpers
private:
    /// get two first bits - type
    static Type extractType(std::uint8_t src);
    /// get last six data bits
    static std::uint8_t exctractData(std::uint8_t src);

    /// 0b00 - unsigned data
    static std::string decodeUnsigned(std::uint8_t src);
    /// 0b01 - signed data
    static std::string decodeSigned(std::uint8_t src);
    /// 0b10 - letter data
    static std::optional<std::string> decodeLetter(std::uint8_t src);

    /// 
    static int decodeSigned6Bit(std::uint8_t src);

// constants
private:
    static constexpr std::uint8_t kTypeShift    = 6;
    static constexpr std::uint8_t kTypeMask     = 0b11000000;
    static constexpr std::uint8_t kDataMask     = 0b00111111;

    static constexpr std::uint8_t kSignBit      = 0b00100000;
    static constexpr std::uint8_t kDataBitWidth = 6;
    static constexpr int kSignedRangeSize       = 1 << kDataBitWidth;

    static constexpr char kFirstLetter = 'a';
    static constexpr char kLastLetter  = 'z';
    static constexpr std::uint8_t kLettersCount 
        = static_cast<std::uint8_t>(kFirstLetter - kLastLetter + 1);
};

#endif