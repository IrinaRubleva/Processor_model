#pragma once
#include "device.hpp"
#include <map>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <optional>

#include <mutex>




/*!
*@brief Шина
обладает своим адресным пространством (uint64_t);
содержит перечень подключённых устройств;
•
каждое устройство подключается к некоторому диапазону адресов (размер диапазона зависит от свойств устройства);
•
шина позволяет выполнить операцию чтения по некоторому адресу:
если по этому адресу подключен порт устройства - возвращает данные с этого
» » если
порта;
по этому адресу не подключено устройства - возвращает случайный шум;
•
шина позволяет выполнить операцию записи по некоторому адресу:
»
если по этому адресу подключен порт устройства - передаёт данные
в этот порт;
»
если по этому адресу не подключено устройства - ничего не делает.
*/




/*!
*@brief Карта Шины. Определяет имя, тип, размер устройства
*/
struct DeviceMapping
{
    std::string name;
    uint64_t baseAddress;
    uint64_t size;
    ReadableDevice *readDev;


    /*!
    *@brief Метод для просмотра, принадлежит ли устройство данному диапазону
    */
    bool range(uint64_t addr) const
    {
        return addr >= baseAddress && addr < baseAddress + size;
    }
};





class Bus : public WriteableDevice, public ReadableDevice
{
private:
    uint64_t addressSpaceSize;
    std::map<uint64_t, DeviceMapping> attachedDevices; // key = baseAddress
    

public:
    ~Bus() noexcept override = default;
    explicit Bus(uint64_t size = 0x1000) : addressSpaceSize(size) {}

    uint64_t getAddressSpaceSize() const { return addressSpaceSize; }
    void setAddressSpaceSize(uint64_t base) noexcept { addressSpaceSize = base; }

    /*!
    *@brief Прикрепить к шине то или иное устройство. Используется в loader.cpp
    */
    void attachDevice(const std::string &name, uint64_t baseAddress, uint64_t size,
                      ReadableDevice *readDev)
    {
        attachedDevices.emplace(baseAddress,
                                DeviceMapping{name, baseAddress, size, readDev});
    }

    uint64_t getDeviceAddress(const std::string &name) const{
        for (const auto &[base, m] : attachedDevices)
            if (m.name == name)
                return m.baseAddress;

        throw std::runtime_error("Bus: unknown device name: " + name);
    }


    uint64_t getDeviceSize(const std::string &name) const{
        for (const auto &[base, m] : attachedDevices){
            if (m.name == name)
                return m.size;
        }
        return 0;
    }


    template<typename T>
    std::optional<T*> getDeviceByName(const std::string& name) const {
        return getDevice<T>(name);
    }

    template <typename T>
    std::optional<T*> getDevice(const std::string& name) const
    {
        for (const auto& [base, mapping] : attachedDevices) {
            if (mapping.name == name) {
                // Используйте dynamic_cast для безопасного приведения
                if (auto* device = dynamic_cast<T*>(mapping.readDev)) {
                    return device;
                }
                break;
            }
        }
        return std::nullopt;
    }

        
    /*!
    *@brief Важный метод!!! Считаваем знаечние с портов устройства
    */
    uint8_t read(uint64_t address) const override;
    /*!
    *@brief Важный метод!!! Записываем знаечние в порты устройства
    */
    void write(uint64_t address, uint8_t value) override;
};
