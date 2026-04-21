#ifndef SOURCE_INTERFACE_HPP
#define SOURCE_INTERFACE_HPP

#include <optional>

template <typename T>
class ISource
{
public:
    virtual ~ISource() = default;

    // setup
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool isOpen() const = 0;

    // sequence work
        /// read one value at time, std::nullopt if not available or value error
    virtual std::optional<T> read() = 0;
        /// true if end of input sequence
    virtual bool eos() const = 0;
};

#endif