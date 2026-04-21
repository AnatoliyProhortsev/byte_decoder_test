#include "QIODeviceSource.hpp"

QIODeviceSource::QIODeviceSource(QIODevice* device)
    : m_device(device)
{
}

bool QIODeviceSource::open()
{
    if (!m_device)
        return false;

    if (m_device->isOpen())
        return true;

    return m_device->open(QIODevice::ReadOnly);
}

bool QIODeviceSource::close()
{
    if (!m_device)
        return false;

    if (!m_device->isOpen())
        return true;

    m_device->close();
    return true;
}

bool QIODeviceSource::isOpen() const
{
    return m_device && m_device->isOpen();
}

std::optional<std::uint8_t> QIODeviceSource::read()
{
    if (!isOpen())
        return std::nullopt;

    char byte = 0;
    const qint64 bytesRead = m_device->read(&byte, 1);

    if (bytesRead != 1)
        return std::nullopt;

    return static_cast<std::uint8_t>(static_cast<unsigned char>(byte));
}

bool QIODeviceSource::eos() const
{
    if (!m_device)
        return true;

    return m_device->atEnd();
}