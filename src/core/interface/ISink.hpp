#ifndef SINK_INTERFACE_HPP
#define SINK_INTERFACE_HPP

template <typename T>
class ISink
{
public:
    virtual ~ISink() = default;

    // setup
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool isOpen() const = 0;

    // sequence work
        /// write one value at time
    virtual void write(const T& data) = 0;
        /// (optional) flush buffer if present
    virtual void flush() {};
};

#endif