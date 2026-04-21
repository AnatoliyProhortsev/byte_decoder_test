#include "QIODeviceSink.hpp"

QIODeviceSink::QIODeviceSink(QIODevice* device)
    : m_device(device)
{
}

bool QIODeviceSink::open()
{
    if (!m_device)
        return false;

    if (m_device->isOpen())
        return true;

    return m_device->open(QIODevice::WriteOnly);
}

bool QIODeviceSink::close()
{
    if (m_device && m_device->isOpen())
    {
        m_device->close();
        return true;
    }

    return false;
}

bool QIODeviceSink::isOpen() const
{
    return m_device && m_device->isOpen();
}

void QIODeviceSink::write(const std::string& data)
{
    if (!isOpen())
        return;

    m_device->write(data.data(), static_cast<qint64>(data.size()));
}

void QIODeviceSink::flush()
{
    if (!isOpen())
        return;

    m_device->waitForBytesWritten(1000);
}