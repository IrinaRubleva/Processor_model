#pragma once
#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <cstddef>


/**
 * @brief Класс, представляющий собой состояние сигнала – 
 * определяется уровнем сигнала и временем, на протяжении которого сигнал находился на этом уровне
 */
class SignalState{
public:
    /**
     * @brief Конструктор по умолчанию, задаёт уровень сигнала и его длительность
     */
    SignalState(): level(false), duration(1) {}  
    /**
     * @brief Инициализирующий конструктор, принимающий уровень сигнала и его длительность
     * @param lvl уровень сигнала
     * @param dur длительность сигнала
     */
    SignalState(bool lvl, std::size_t dur): level(lvl), duration(dur) {validDuration(dur);}
    /**
     * @brief Инициализирующий конструктор, принимающий строку
     * @param str строка. Пример, str("1000") -> level = 1, duration = 1
     */
    SignalState(const std::string &str);
    /**
     * @brief Геттер уровня сигнала
     */
    bool getLevel() const;
    /**
     * @brief Геттер длительности сигнала
     */
    std::size_t getDuration() const;
    /**
     * @brief Сеттер для уровня
     * @param lvl булевой уровень сигнала
     */
    void setLevel(bool lvl);
    /**
     * @brief Сеттер для уровня
     * @param lvl символ уровеня сигнала
     */
    void setLevel(char lvl);
    /**
     * @brief Сеттер для уровня
     * @param lvl числовой уровень сигнала: false = 0, true = 1
     */
    void setLevelInt(int levelInt);
    /**
     * @brief Сеттер для длительности
     * @param newDur длительность - число
     */
    void setDuration(size_t newDur);
    /**
     * @brief Сеттер для сигнала
     * @param newLevel уровень сигнала
     * @param newDuration длительность сигнала
     */
    void setState(bool newLevel, size_t newDuration);
    /**
     * @brief функция для ввода в поток
     * @param is - поток
     */
    void inputState(std::istream & is);
     /**
     * @brief функция для вывода из потока
     * @param is - поток
     */
    void outputState(std::ostream &os) const ;
    /**
     * @brief инверсия уровня сигнала
     */
    SignalState & inversionState();
    /**
     * @brief удлинение сигнала на указанное число условных единиц времени
     * @param число на которое увеличиваем исходный duration
     */
    void lengtheningState(size_t time);
    /**
     * @brief укорачивание сигнала на указанное число условных единиц времени
     * @param число на которое укорачиваем исходный duration
     */
    void shorteningState(size_t time);
    /**
     * @brief форматирование сигнала в виде графика (например, для 1 использовать символ “ ¯”, а для 0 – “_”, продублированные нужное число раз).
     */
    std::string formatState() const;
    /**
     * @brief для ввода экземпляра сложного класса из входного потока;
     */
    friend std::ostream& operator<<(std::ostream& os, const SignalState & state);
    /**
     * @brief для вывода экземпляра сложного класса в выходной поток;
     */
    friend std::istream & operator>>(std::istream & is, SignalState & state);
    /**
     * @brief префиксный оператор для инверсии уровня сигнала
     */
    SignalState & operator~ ();

private: 
    bool level;
    std::size_t duration;
    /**
     * @brief проверка вилидности длительности
     */
    void validDuration(std::size_t dur);
};
