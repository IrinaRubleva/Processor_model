#pragma once
#include <cstdint>
#include <memory>

typedef uint64_t port_size;
class ControlUnit ;



/*!
*@brief Любые устройства
* •имеют набор входных и выходных портов, каждый размером в 1 байт;
• •поддерживает операции записи и чтения некоторого порта (по его номеру);
при чтении входного порта или записи в выходной - неопределённое поведение (на ваше усмотрение).
•устройства делятся на:
»асинхронные - работают в соответствии с тактами процессора, получают управление раз в N тактов (N задаётся аттрибутом устройства);
 синхронные - получают управление сразу после обновлении любого из их портов.
*/


struct ReadableDevice{
    virtual uint8_t read(port_size port) const = 0; 
    virtual ~ReadableDevice()= default;
};

struct WriteableDevice
{
    virtual void write(uint64_t port, uint8_t value) = 0;
    virtual ~WriteableDevice() = default;
};
//new WritableDevice->write(1,1);

struct SynchronDevice
{
    virtual uint64_t getPeriod() const = 0;
    virtual void tick(ControlUnit& cu) = 0;
    virtual ~SynchronDevice() = default;
};

struct AsynchronDevice
{
    virtual bool ready() const = 0;
    virtual void tick(ControlUnit & cu) = 0;
    virtual ~AsynchronDevice() = default;
};
