#ifndef CONVERTER_WORKER_HPP
#define CONVERTER_WORKER_HPP

#include <memory>
#include <thread>

#include "core/interface/ISink.hpp"
#include "core/interface/ISource.hpp"

class ConverterWorker
{
public:
    ConverterWorker(std::shared_ptr<ISource<std::uint8_t>> source,
                    std::shared_ptr<ISink<std::string>> sink);
    ~ConverterWorker();

    void start();
    void stop();

    bool isRunning() const;

private:
    void run(std::stop_token stoken);

private:
    std::shared_ptr<ISource<std::uint8_t>> m_source;
    std::shared_ptr<ISink<std::string>> m_sink;

    std::jthread m_workerThread;
};

#endif