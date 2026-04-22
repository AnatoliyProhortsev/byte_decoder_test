#include "ConverterWorker.hpp"

#include <chrono>
#include <functional>
#include <utility>

#include "core/utils/ByteDecoder.hpp"

ConverterWorker::ConverterWorker(std::shared_ptr<ISource<std::uint8_t>> source,
                                 std::shared_ptr<ISink<std::string>> sink)
    : m_source(std::move(source))
    , m_sink(std::move(sink))
{
}

ConverterWorker::~ConverterWorker()
{
    stop();
}

void ConverterWorker::start()
{
    if (m_workerThread.joinable())
        return;

    auto runFn = std::mem_fn(&ConverterWorker::run);
    m_workerThread = std::jthread([this](std::stop_token stopToken)
    {
        run(stopToken);
    });
}

void ConverterWorker::stop()
{
    if (m_workerThread.joinable())
        m_workerThread.request_stop();
}

bool ConverterWorker::isRunning() const
{
    return m_workerThread.joinable();
}

void ConverterWorker::run(std::stop_token stoken)
{
    if (!m_source || !m_sink)
        return;

    if (!m_source->open())
        return;

    if (!m_sink->open())
    {
        m_source->close();
        return;
    }

    while (!stoken.stop_requested())
    {
        const auto rawByte = m_source->read();

        if (!rawByte.has_value())
        {
            if (m_source->eos())
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto decoded = ByteDecoder::decode(*rawByte);
        if (!decoded)
            continue;

        
        decoded->push_back(' ');

        m_sink->write(*decoded);
    }

    m_sink->flush();
    m_sink->close();
    m_source->close();
}