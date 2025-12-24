#pragma once
#include "../device/device.hpp"
#include "../controleUnit.hpp"

#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>




typedef uint64_t port_size;

class ALU {
public:
    virtual void submit(Command* cmd) = 0;
    virtual ~ALU() = default;
    virtual void onTick() = 0;
};

class ALUDevice_Synchron : public ReadableDevice, public WriteableDevice, public SynchronDevice, public ALU{
private:
    uint8_t opCode = 0;
    std::array<uint8_t, 8> operandA{};
    std::array<uint8_t, 8> operandB{};
    std::array<uint8_t, 8> result{};
    std::array<uint8_t, 8> flags{};

    Command* currentCommand = nullptr;
    ControlUnit* cu_ptr = nullptr;

    uint64_t period = 1;

    // -----многопоточка-----------------
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    mutable std::mutex state_mtx;

    bool tick_requested = false;
    bool stop = false;

public:
    explicit ALUDevice_Synchron(uint64_t period_)
        : period(period_)
    {
        worker = std::thread(&ALUDevice_Synchron::threadLoop, this);
    }

    ~ALUDevice_Synchron() {
        {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
        }
        cv.notify_one();
        if (worker.joinable())
            worker.join();
    }

    
    uint8_t read(port_size port) const override;
    void write(port_size port, uint8_t value) override;

    uint64_t getPeriod() const override { return period; }

    void attachControlUnit(ControlUnit* cu)
    {
        std::lock_guard<std::mutex> lock(mtx);
        cu_ptr = cu;
    }

    
    void submit(Command* cmd) override
    {
        std::lock_guard<std::mutex> lock(state_mtx);
        currentCommand = cmd;
    }

    
    void onTick()
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tick_requested = true;
        }
        cv.notify_one();
    }
    void setCurrentCommand(Command* cmd) {
        currentCommand = cmd;
    }

private:
   
    void threadLoop()
    {
        std::unique_lock<std::mutex> lock(mtx);

        while (!stop)
        {
            cv.wait(lock, [&] { return tick_requested || stop; });
            if (stop)
                break;

            tick_requested = false;

            if (currentCommand && cu_ptr)
            {
                Command* cmd;
                ControlUnit* cu;

                {
                    std::lock_guard<std::mutex> lock(state_mtx);
                    cmd = currentCommand;
                    cu  = cu_ptr;
                }
                
                lock.unlock();
                tick(*cu);
                lock.lock();
            }
        }
    }

    
    void tick(ControlUnit& cu) override;

    uint64_t unpack(const std::array<uint8_t,8>& a);

    void pack(std::array<uint8_t,8>& dst, uint64_t val);
};







class ALUDevice_Asynchron : public ReadableDevice, public WriteableDevice,  public AsynchronDevice, public ALU {
    uint8_t opCode;
    std::array<uint8_t, 8> operandA;
    std::array<uint8_t, 8> operandB;
    std::array<uint8_t, 8> result;
    std::array<uint8_t, 8> flags;
    bool is_Ready;
    Command* currentCommand = nullptr;

    std::thread worker;
    std::atomic<bool> stop{false};

public:
    void onTick() override{}


    ALUDevice_Asynchron() {
        worker = std::thread([this]() {
            while (!stop) {
                if (is_Ready) {
                    tick(*cu_ptr);
                }
                std::this_thread::yield();
            }
        });
    }

    ~ALUDevice_Asynchron() {
        stop = true;
        if (worker.joinable())
            worker.join();
    }

public:
    void attachControlUnit(ControlUnit* cu) {
        cu_ptr = cu;
    }
    uint8_t read(port_size port) const override;
    void write(port_size port, uint8_t value);
    bool ready() const override {return is_Ready;};
    void tick(ControlUnit& cu) override;
    void setCurrentCommand(Command* cmd) {
        currentCommand = cmd;
    }
    Command* getCurrentCommand() const {
        return currentCommand;
    }


    void submit(Command* cmd) override {
        std::cout << "=== [ALU_Async::submit] START ===" << std::endl;
        
        setCurrentCommand(cmd);
        std::cout << "[ALU_Async::submit] Command set: " 
                << (currentCommand ? "YES" : "NO") << std::endl;
        
        if (!cu_ptr) {
            std::cerr << "[ALU_Async::submit] ERROR: ControlUnit not attached!" << std::endl;
            return;
        }
        
        std::cout << "[ALU_Async::submit] Calling tick()..." << std::endl;
        tick(*cu_ptr);
        
        std::cout << "[ALU_Async::submit] DONE" << std::endl;
        std::cout << "=== [ALU_Async::submit] END ===" << std::endl;
    }



private:
    uint64_t unpack(const std::array<uint8_t,8>& a);
    void pack(std::array<uint8_t,8>& dst, uint64_t val);
    ControlUnit* cu_ptr = nullptr;
};