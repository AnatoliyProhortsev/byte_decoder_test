#ifndef QIODEVICE_SINK_HPP
#define QIODEVICE_SINK_HPP

#include <string>
#include <QIODevice>

#include "core/interface/ISink.hpp"

// QIODeviceSink realizes ISink interface with QIODevice class
class QIODeviceSink : public ISink<std::string>
{
public:
    explicit QIODeviceSink(QIODevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() const override;

    void write(const std::string& data) override;
    void flush() override;

private:
    QIODevice* m_device;
};

#endif