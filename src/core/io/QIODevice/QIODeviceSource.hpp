#ifndef QIODEVICE_SOURCE_HPP
#define QIODEVICE_SOURCE_HPP

#include <QIODevice>
#include <cstdint>
#include <optional>

#include "core/interface/ISource.hpp"

class QIODeviceSource : public ISource<std::uint8_t>
{
public:
    explicit QIODeviceSource(QIODevice* device);

    bool open() override;
    bool close() override;
    bool isOpen() const override;

    std::optional<std::uint8_t> read() override;
    bool eos() const override;

private:
    QIODevice* m_device {};
};

#endif