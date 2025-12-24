#pragma once
#include <vector>
#include <memory>
#include "decoder.hpp"
#include "device/device.hpp"
#include "device/alu.hpp"
#include "generalRegisters.hpp"
#include "programMemory.hpp"
#include "device/bus.hpp"
#include "controleUnit.hpp"

#include <map>
#include <iostream>
#include <iomanip>


/*!
*@brief Сам процессор. Процессор состоит из:
•устройства управления
•исполнительных устройств, выполняющих определенные наборы операторов
•блока регистров общего назначения'
•памяти программ
•памяти данных
*/


class Processor
{
private:
    std::unique_ptr<Bus> bus ;
    std::unique_ptr<ProgramMemory> programMemory;
    std::unique_ptr<ControlUnit> controlUnit;

    std::vector<std::unique_ptr<ReadableDevice>> devices;


    uint32_t running;
    uint64_t currentTime;

public:
    explicit Processor(uint64_t busAddressSpace, size_t numRegisters);
   

    ~Processor(){
         try {
            if (controlUnit) {
                controlUnit->safeShutdown(); 
            }
        } catch (...) {
            std::cerr << "[Processor] Exception in destructor, ignoring..." << std::endl;
        }
    }
    Processor(const Processor &) = delete;
    Processor &operator=(const Processor &) = delete;
    Processor(Processor &&other) noexcept = default;
    Processor &operator=(Processor &&other) noexcept = default;

    void attachDevice(const std::string &name, uint64_t base, uint64_t size, std::unique_ptr<ReadableDevice> dev)
    {
        if (name == "ProgramMemory")
        {
            throw std::runtime_error("Use setProgramMemory() for ProgramMemory, not attachDevice()");
        }
        ReadableDevice *raw = dev.get();

        bus->attachDevice(name, base, size, raw);
        devices.push_back(std::move(dev));
    }


    /*!
    *@brief Установить ProgramMemory
    */
    void setProgramMemory(std::unique_ptr<ProgramMemory> newProgMem)
    {
        programMemory = std::move(newProgMem);

        if (controlUnit)
        {
            controlUnit->setProgramMemory(programMemory.get());
        }
    }


    /*!
    *@brief Загрузить команду
    */
     
    void loadProgram(const std::vector<uint8_t> &program)
    {
        if (programMemory)
        {
            programMemory->loadProgram(program);
        }
        if (controlUnit)
        {
            controlUnit->setPC(0);
        }
    }

    void setThreadCount(uint64_t thC){
        controlUnit->setThreadCount(thC);
    }

    void run();


    bool isValid() const
    {
        return bus && programMemory && controlUnit;
    }

    Bus *getBus() { return bus.get(); }
    ProgramMemory *getProgramMemory() { return programMemory.get(); }
    ControlUnit *getControlUnit() { return controlUnit.get(); }

    const Bus *getBus() const { return bus.get(); }
    const ProgramMemory *getProgramMemory() const { return programMemory.get(); }
    const ControlUnit *getControlUnit() const { return controlUnit.get(); }

    uint64_t getCurrentTime() const { return currentTime; }
    bool isRunning() const { return running; }
    size_t getDeviceCount() const { return devices.size(); }



    /*!
    *@brief Очистка процессора
    */
     
    void reset() {
        running = false;
        
        currentTime = 0;
        
        if (controlUnit) {
            controlUnit->setPC(0);
        }
        clearDataMemory();
        clearRegisters();
        if (programMemory) {
            programMemory->clear();
        }
    }

    /*!
    *@brief Очистка DataMemory
    */
     
      void clearDataMemory() {
        auto* bus = getBus();
        if (!bus) return;
        
        uint64_t dataMemBase = bus->getDeviceAddress("DataMemory");
        uint64_t dataMemSize = bus->getDeviceSize("DataMemory");
        
        for (uint64_t i = 0; i < dataMemSize; ++i) {
            bus->write(dataMemBase + i, 0);
        }
    }
    


    /*!
    *@brief Очистка регистров GPR
    */
     
    void clearRegisters() {
        auto* bus = getBus();
        if (!bus) return;
        uint64_t gprBase = bus->getDeviceAddress("GPR");
        uint64_t gprSize = bus->getDeviceSize("GPR");
        
        for (uint64_t i = 0; i < gprSize; ++i) {
            bus->write(gprBase + i, 0);
            
        }
    }

private:
    uint64_t getProgramMemorySize() const
    {
        return programMemory ? programMemory->getSize() : 0;
    }

    uint64_t getBusAddressSpaceSize() const
    {
        return bus ? bus->getAddressSpaceSize() : 0;
    }

    
};