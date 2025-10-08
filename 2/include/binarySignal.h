#pragma once
#include "signalState.h"
#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <cstddef> 


/**
 * @brief Класс, представляющий двоичный сигнал – определяется последовательностью состояний сигнала
 */
class BinarySignal{
public:
    /**
     * @brief  Конструктор по умолчанию
     */
    BinarySignal() {}
     /**
     * @brief Конструктор из одного состояния сигнала
     * @param sgn Состояние сигнала для инициализации
     */
    BinarySignal(const SignalState& sgn) { push_back(sgn);}
    /**
     * @brief Конструктор из строки символов '0' и '1'
     * @param str Строка, содержащая последовательность '0' и '1'
     * @throw std::invalid_argument Если строка содержит недопустимые символы
     */
    BinarySignal(const std::string& str);
    /**
     * @brief Конструктор сигнала с постоянным уровнем
     * @param lvl Уровень сигнала (true/false)
     * @param dur Длительность сигнала
     * @throw std::invalid_argument Если длительность равна 0
     */
    BinarySignal(bool lvl, std::size_t dur);

    /**
     * @brief Функция обмена содержимым двух сигналов
     * @param first Первый сигнал
     * @param second Второй сигнал
     */
    friend void swap(BinarySignal& first, BinarySignal& second) noexcept {
        std::swap(first.signal, second.signal);
        std::swap(first.size, second.size);
        std::swap(first.capacity, second.capacity);
    }

    /**
     * @brief Оператор копирующего присваивания
     * @param other Сигнал для копирования
     * @return Ссылка на текущий объект
     */
    BinarySignal&operator=(const BinarySignal& other);
    /**
     * @brief Оператор перемещающего присваивания
     * @param other Сигнал для перемещения
     * @return Ссылка на текущий объект
     */
    BinarySignal& operator=(BinarySignal&& other) noexcept;
    
    /**
     * @brief Дополнение сигнала другим сигналом
     * @param other Сигнал для добавления
     * @return Ссылка на текущий объект
     */
    BinarySignal& operator+=(const BinarySignal& other);
    /**
     * @brief Дублирование сигнала заданное число раз;
     * @param n Количество повторений
     * @return Новый сигнал, содержащий n копий текущего
     */
    BinarySignal operator*(std::size_t n) const;
    /**
     * @brief Дублирование сигнала заданное число раз в другоп порядке (для удобства);
     */
    friend BinarySignal operator*(std::size_t n, const BinarySignal& bs) {
        return bs * n;
    }


    /**
     * @brief Вывод сигнала в поток
     * @param os Выходной поток
     * @param signal Сигнал для вывода
     * @return Ссылка на поток
     */
    friend std::ostream& operator<<(std::ostream& os, const BinarySignal& signal);;

     /**
     * @brief  Ввод сигнала из потока
     * @param is Входной поток
     * @param signal Сигнал для заполнения
     * @return Ссылка на поток
     */
    friend std::istream & operator>>(std::istream & is, BinarySignal & signal);

    /**
     * @brief Получение уровня сигнала в заданный момент времени;
     * @param time Момент времени
     */
    bool operator[](std::size_t time) const;
     /**
     * @brief Инверсия уровней всех состояний сигнала
     * @return Ссылка на текущий объект
     */
    BinarySignal &inversionSignal();
    /**
     * @brief Получение общей длительности сигнала
     * @return Суммарная длительность всех состояний сигнала
     */
    std::size_t getTotalDuration() const;
    /**
     * @brief Получение количества состояний в сигнале
     * @return Количество состояний (не путать с общей длительностью)
     */
    size_t getSize() const {return size;}
    /**
     * @brief Вставка другого сигнала в заданный момент времени
     * @param startTime Момент времени для вставки
     * @param other Сигнал, который нужно вставить
     */
    void insertSignal(std::size_t startTime, const BinarySignal& other);
    /**
     * @brief Удаление части сигнала
     * @param startTime Начальный момент времени удаления
     * @param delDur Длительность удаляемого участка
     * @throw std::out_of_range Если startTime превышает длительность сигнала
     */
    void deleteSignal(std::size_t startTime, std::size_t delDur);

    /**
     * @brief форматирование сигнала в виде строки (например, для 1 символ “ ¯”, а для 0 – “_”, и на стыках 1 и 0 переход символами “/” и “\”, 
     * например “¯¯\_/¯¯¯\__”).
     */
    std::string formatSignal() const;
    /**
     * @brief Ввод сигнала из потока
     * @param is Входной поток
     */
    void inputSignal(std::istream & is);
    /**
     * @brief Вывод сигнала в поток
     * @param os Выходной поток
     */
    void outputSignal(std::ostream &os) const ; 

    /**
     * @brief Добавления элемента в конец
     */
    void push_back(const SignalState & state);

    /**
     * @brief Достать последний элемент
     */
    SignalState& back();
    /**
     * @brief Достать последний элемент
     */
    const SignalState& back() const;

    /**
     * @brief Копирующий конструктор
     */
    BinarySignal(const BinarySignal & other);
    /**
     * @brief Перемещающий конструктор
     */
    BinarySignal(BinarySignal &&other) noexcept;
    /**
     * @brief Деструктор
     */
    ~BinarySignal() { delete[] signal;}


private:
    /**
     * @brief Указатель на массив состояний сигнала
     */
    SignalState * signal = nullptr;
    /**
     * @brief Максимальное количество элементов
     */
    std::size_t capacity = 0;
    /**
     * @brief Текущее количество элементов
     */
    std::size_t size = 0;
    /**
     * @brief Увеличивает capacity
     */
    void expander(std::size_t new_capacity);
    /**
     * @brief Ищет время начала выполнения нового состояния и индекс его вхождения в сигнал
     */
    std::pair<std::size_t, std::size_t> findIndexAtTime(std::size_t time) const;
     /**
     * @brief Объединение последовательных состояний с одинаковым уровнем
     * @return Ссылка на текущий объект
     */
    BinarySignal & mergeStates();
};
