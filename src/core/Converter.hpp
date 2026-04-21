#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <memory>
#include <QIODevice>

    /// interface
#include "core/interface/ISink.hpp"
#include "core/interface/ISource.hpp"
    /// worker
#include "core/workers/ConverterWorker.hpp"

class Converter
{
public:
    Converter(QIODevice* inputDevice, QIODevice* outputDevice);

    void start();
    void stop();
    bool isRunning() const;

private:
    std::shared_ptr<ISource<std::uint8_t>> m_source;
    std::shared_ptr<ISink<std::string>> m_sink;
    ConverterWorker m_worker;
};

#endif