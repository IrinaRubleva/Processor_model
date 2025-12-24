#include "../include/controleUnit.hpp"
#include <stdexcept>
#include <iostream>
#include "../include/commands/command.hpp"
#include "../include/device/alu.hpp"



void ControlUnit::runProgram() {
    
    
    createNewThread(0);
    std::unique_lock<std::mutex> lock(threads_mutex);
    
    cv.wait(lock, [this] { 
        bool allFinished = true;
        for (const auto& ctx : threads) {
            if (ctx.active.load()) {
                allFinished = false;
                break;
            }
        }
        return allFinished;
    });
    
    program_finished = true;
}

void ControlUnit::createNewThread(uint64_t startPC) {
    std::lock_guard<std::mutex> lock(threads_mutex);
    
    size_t threadId = threads.size();
    
    ThreadContext ctx;
    ctx.pc = startPC;
    ctx.active = true;
    ctx.should_stop = false;
    
    if (startPC >= programMemory->getSize()) {
        std::cerr << "[ControlUnit] ERROR: startPC=" << startPC 
                  << " is out of bounds (program size=" << programMemory->getSize() << ")" << std::endl;
        return;
    }
    
    ctx.thread = std::make_unique<std::thread>(
        &ControlUnit::threadLoop, 
        this, 
        startPC, 
        threadId
    );
    
    threads.push_back(std::move(ctx));
    
    
    uint32_t newCount = getThreadCount() + 1;
    uint64_t gprBase = bus->getDeviceAddress("GPR");
    for (int i = 0; i < 4; ++i) {
        bus->write(gprBase + i, (newCount >> (i * 8)) & 0xFF);
    }
    
   
}

void ControlUnit::threadLoop(uint64_t startPC, size_t threadId) {
    uint64_t pc = startPC;
    const uint64_t MAX_NO_COMMAND_COUNT = 100; 
    
    int noCommandCount = 0;
    
    try {
        while (!threads[threadId].should_stop.load()) {
            if (threadId < threads.size() && threads[threadId].should_stop.load()) {
            
                break;
            }
            
            uint64_t progSize = programMemory->getSize();
            if (pc >= progSize) {
                break;
            }
            
            uint64_t pc_ref = pc;
            auto cmd = decoder.decode(pc_ref, *programMemory);
            pc = pc_ref;
            
            if (!cmd) {
                noCommandCount++;
                if (noCommandCount >= MAX_NO_COMMAND_COUNT) {
                    break;
                }
                
                pc++;
                continue;
            }
            
            noCommandCount = 0;
            
            for (int i = 0; i < 10 && pc + i < programMemory->getSize(); i++) {
                //std::cout << std::hex << (int)programMemory->read(pc + i) << " ";
            }
          if (cmd->getName() == "THRE") {
           
                threads[threadId].should_stop = true;
                cmd->compute(*this);
                break;
            }
            
            if (auto aluOpt = bus->getDevice<ALUDevice_Synchron>("ALU")) {
                (*aluOpt)->setCurrentCommand(cmd.get());
            }
            
            cmd->compute(*this);
           
            static const int MAX_COMMANDS_PER_THREAD = 50;
            static thread_local int commandCount = 0;
            commandCount++;
            if (commandCount >= MAX_COMMANDS_PER_THREAD) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Thread #" << threadId << "] Error: " << e.what() << std::endl;
    }
    
    {
        std::lock_guard<std::mutex> lock(threads_mutex);
        if (threadId < threads.size()) {
            threads[threadId].active = false;
            threads[threadId].should_stop = true;
            threads[threadId].pc = pc;
        }
    }
    
    
    decrementThreadCount();
    cv.notify_all();
}





uint32_t ControlUnit::getThreadCount() {
    uint64_t gprBase = bus->getDeviceAddress("GPR");
    uint32_t count = 0;
    for (int i = 0; i < 4; ++i) {
        count |= (uint32_t)bus->read(gprBase + i) << (i * 8);
    }
    return count;
}

void ControlUnit::decrementThreadCount() {
      uint64_t gprBase = bus->getDeviceAddress("GPR");
    uint32_t threadCount = getThreadCount();
    
    if (threadCount > 0) {
        uint32_t newCount = threadCount - 1;
        for (int i = 0; i < 4; ++i) {
            bus->write(gprBase + i, (newCount >> (i * 8)) & 0xFF);
        }
    }
}

void ControlUnit::terminateThread(size_t threadId) {
    std::lock_guard<std::mutex> lock(threads_mutex);
    if (threadId < threads.size()) {
        threads[threadId].should_stop = true;
        threads[threadId].active = false;
    }
}

void ControlUnit::stopAllThreads() {
    {
        std::lock_guard<std::mutex> lock(threads_mutex);
        for (auto& ctx : threads) {
            ctx.should_stop = true;
            ctx.active = false;
        }
        program_finished = true;
    }
    cv.notify_all();
    joinAllThreads();
}

bool ControlUnit::hasActiveThreads() const {
    for (const auto& ctx : threads) {
        if (ctx.active.load()) {
            return true;
        }
    }
    return false;
}