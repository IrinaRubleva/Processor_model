#pragma once
#include "device/bus.hpp"
#include "programMemory.hpp"
#include "decoder.hpp"
#include "vector/include/Vector.hpp"
#include <optional>
#include <string>
#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <memory>




/*!
*@brief Устройство управления:
•не имеет портов, вместо этого имеет доступ ко всей шине сразу, может читать и писать в любые адреса шины;
•заранее знает адреса всех необходимых для работы устройст (заданы константой);
•поочередно достаёт команды из памяти команд, интерпретирует их и исполняет.
*/


class ControlUnit;

struct ThreadContext
{
    uint64_t pc = 0;
    std::atomic<bool> active{true};
    std::atomic<bool> should_stop{false};
    std::unique_ptr<std::thread> thread;

    ThreadContext() = default;

    ThreadContext(ThreadContext &&other) noexcept
        : pc(other.pc)
          ,
          active(other.active.load())
          ,
          should_stop(other.should_stop.load())
          ,
          thread(std::move(other.thread))
    {
    }

    ThreadContext &operator=(ThreadContext &&other) noexcept
    {
        if (this != &other)
        {
            pc = other.pc;
            active = other.active.load();
            should_stop = other.should_stop.load();
            thread = std::move(other.thread);
        }
        return *this;
    }

    ThreadContext(const ThreadContext &) = delete;
    ThreadContext &operator=(const ThreadContext &) = delete;
};

class ControlUnit
{
public:
    std::vector<ThreadContext> threads;
    std::mutex threads_mutex;
    std::condition_variable cv;
    std::atomic<bool> program_finished{false};

    Decoder decoder;
    Bus *bus = nullptr;
    ProgramMemory *programMemory = nullptr;
    uint64_t PC = 0;
    Vector<uint64_t> registers;
    uint64_t period = 1;

    uint64_t ThreadCount = 1;

    ControlUnit() : period(1) {}
    ControlUnit(Bus *busPtr, ProgramMemory *progMemPtr)
        : bus(busPtr), programMemory(progMemPtr), PC(0), period(1) {}

    void setBus(Bus *b) { bus = b; }
    void setProgramMemory(ProgramMemory *p) { programMemory = p; }
    void setPC(uint64_t pc) { PC = pc; }
    uint64_t getPC() const { return PC; }

    Bus *getBus() const { return bus; }
    uint64_t getPeriod() const { return period; };
    void setPeriod(uint64_t p) { period = p; };
    Decoder *getDecoder() { return &decoder; }

    Decoder &getRefDecoder() { return decoder; }

    Bus &getRefBus()
    {
        if (!bus)
            throw std::runtime_error("ControlUnit: bus is nullptr");
        return *bus;
    }

    const Bus &getRefBus() const
    {
        if (!bus)
            throw std::runtime_error("ControlUnit: bus is nullptr");
        return *bus;
    }

    uint64_t getDeviceAddress(const std::string &name) const
    {
        return bus->getDeviceAddress(name);
    }

    Vector<uint64_t> &regs() { return registers; }
    const Vector<uint64_t> &regs() const { return registers; }

    
    uint64_t getReg(size_t idx) const { return registers[idx]; }
    void setReg(size_t idx, uint64_t v) { registers[idx] = v; }
    uint8_t read(port_size port) const { return 0; };

    void joinAllThreads()
    {
        std::lock_guard<std::mutex> lock(threads_mutex);
        for (auto &ctx : threads)
        {
            if (ctx.thread && ctx.thread->joinable())
            {
                ctx.thread->join();
            }
        }
    }

    void createNewThread(uint64_t startPC);
    void runProgram();
    bool isFinished() const { return program_finished.load(); }
    void threadLoop(uint64_t startPC, size_t threadId);
    uint32_t getThreadCount();
    void decrementThreadCount();
    void terminateThread(size_t threadId);
    void stopAllThreads();
    bool hasActiveThreads() const;

    void setThreadCount(uint64_t thrC)
    {
        ThreadCount = thrC;
    }
    uint64_t getThreadCount() const
    {
        return ThreadCount;
    }

    void safeShutdown()
    {
        stopAllThreads();
        joinAllThreads();
    }
};