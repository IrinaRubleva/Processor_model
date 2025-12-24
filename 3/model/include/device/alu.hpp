#pragma once
#include "../device/device.hpp"
#include "../controleUnit.hpp"

#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>


/*!
*@brief Арифметико-логическое устройство:
• позволяет выполнять перечень арифметических и логических операций;
• • •
синхронное или асинхронное
порт 0 - вход - код операции (сложение/вычитаение/…);
порты 1-8 - вход - первый операнд;
•
порты 9-16 - вход - второй операнд;
*Расширяемый класс - устройство. Необходимо предусмотреть возможность переопределения устройства в новом классе - наследнике (например, модуль операций с плавающей запятой).
•
порты 17-24 - выход - результат операции;
•
порты 25-32 - выход - переполнение, остаток от деления, и т.п. в зависимости от операции.
*/


typedef uint64_t port_size;


class ALUDevice_Synchron : public ReadableDevice, public WriteableDevice, public SynchronDevice{
private:
    uint8_t opCode = 0;
    std::array<uint8_t, 8> operandA{};
    std::array<uint8_t, 8> operandB{};
    std::array<uint8_t, 8> result{};
    std::array<uint8_t, 8> flags{};

    Command* currentCommand = nullptr;
    uint64_t period = 1;

public:
    explicit ALUDevice_Synchron(uint64_t period_)
        : period(period_)
    {
      
    }

    ~ALUDevice_Synchron() {
    }

    /*!
    *@brief Считать значение с порта
    */
    uint8_t read(port_size port) const override;
      /*!
    *@brief Записать значение с порта
    */
    void write(port_size port, uint8_t value) override;

    uint64_t getPeriod() const override { return period; }
    /*!
    *@brief Получить значение текущей команды
    */
    void setCurrentCommand(Command* cmd) {
        currentCommand = cmd;
    }

    /*!
    *@brief Выполнить команду на парту
    */
    void tick(ControlUnit& cu) override;

private:
   

    /*!
    *@brief Распаковка операндов с порта для формирования операндов команды
    */
    uint64_t unpack(const std::array<uint8_t,8>& a);
    /*!
    *@brief Пакуем результат с порта
    */
    void pack(std::array<uint8_t,8>& dst, uint64_t val);
};







class ALUDevice_Asynchron : public ReadableDevice, public WriteableDevice,  public AsynchronDevice{
    uint8_t opCode;
    std::array<uint8_t, 8> operandA;
    std::array<uint8_t, 8> operandB;
    std::array<uint8_t, 8> result;
    std::array<uint8_t, 8> flags;
    bool is_Ready;
    Command* currentCommand = nullptr;

public:


    ALUDevice_Asynchron() {
       
    }

    ~ALUDevice_Asynchron() {
    }

public:

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


    


private:
    uint64_t unpack(const std::array<uint8_t,8>& a);
    void pack(std::array<uint8_t,8>& dst, uint64_t val);
    ControlUnit* cu_ptr = nullptr;
};