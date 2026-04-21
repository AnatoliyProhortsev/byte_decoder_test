#include "Converter.hpp"

#include "core/io/QIODevice/QIODeviceSink.hpp"
#include "core/io/QIODevice/QIODeviceSource.hpp"

Converter::Converter(QIODevice* inputDevice, QIODevice* outputDevice)
    : m_source(std::make_shared<QIODeviceSource>(inputDevice))
    , m_sink(std::make_shared<QIODeviceSink>(outputDevice))
    , m_worker(m_source, m_sink)
{
}

void Converter::start()
{
    m_worker.start();
}

void Converter::stop()
{
    m_worker.stop();
}

bool Converter::isRunning() const
{
    return m_worker.isRunning();
}