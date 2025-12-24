#pragma once
#include <iterator>
#include <type_traits>
#include <cassert>
#include <initializer_list>
#include <memory>
#include "VectorIterator.hpp"

template <typename T, typename Alloc = std::allocator<T>>
class Vector
{
public:
    /*!
     * @brief Тип хранимых значений
     */
    typedef T value_type;
    /*!
     * @brief Ссылка на тип хранимых значений
     */
    typedef T &reference;
    /*!
     * @brief Константная ссылка на тип хранимых значений
     */
    typedef const T &const_reference;
    /*!
     * @brief Тип итератора
     */
    typedef VectorIterator<T, false> iterator;
    /*!
     * @brief Тип константного итератора
     */
    typedef VectorIterator<T, true> const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;

    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    /*!
     * @brief Тип, обозначающий дистанцию между двумя итераторами,
     *        т.е. результат std::distance(it1, it2)
     */
    typedef ptrdiff_t difference_type;
    /*!
     * @brief Тип, используемый для преставления размера контейнера
     */
    typedef size_t size_type;
    /*!
     * @brief Тип используемого аллокатора
     */
    typedef Alloc allocator_type;

    //---------------------------------------------------Constructors/Destructor/Operator=----------------------------------------------
    /*!
     * @brief Пустой конструктор, создаёт пустой список
     * @param alloc Аллокатор
     */
    Vector(const allocator_type &alloc = allocator_type()) noexcept(std::is_nothrow_default_constructible_v<Alloc>)
        : begin_(nullptr), end_(nullptr), cap_(nullptr), allocator(alloc) {}

    /*!
     * @brief Копирующий конструктор
     * @param o Копируемый список
     */
    Vector(const Vector &o)
        // Работает только если есть T() и T(a)
        requires std::copy_constructible<T>
        : allocator(o.allocator)
    {
        if (o.size() == 0)
        {
            begin_ = end_ = cap_ = nullptr;
            return;
        }

        begin_ = allocate(o.size());
        end_ = begin_;
        cap_ = begin_ + o.size();

        for (size_type i = 0; i < o.size(); ++i)
        {
            construct_at(end_++, o[i]);
        }
    }

    /*!
     * @brief Копирующий конструктор
     * @param o Копируемый список
     * @param alloc Аллокатор
     */
    Vector(const Vector &o, const allocator_type &alloc)
        // Работает только если есть T() и T(a)
        requires std::copy_constructible<T>
        : allocator(alloc)
    {
        size_type n = o.size();
        if (n == 0)
        {
            begin_ = end_ = cap_ = nullptr;
            return;
        }

        begin_ = allocate(n);
        end_ = begin_;
        cap_ = begin_ + n;

        for (size_type i = 0; i < n; ++i)
        {
            construct_at(end_++, o[i]);
        }
    }

    /*!
     * @brief Перемещающий конструктор
     * @param o Перемещаемый список
     */
    Vector(Vector &&o) noexcept : begin_(o.begin_), end_(o.end_), cap_(o.cap_), allocator(std::move(o.allocator))
    {
        o.begin_ = o.end_ = o.cap_ = nullptr;
    }

    /*!
     * @brief Перемещающий конструктор
     * @param alloc Аллокатор
     * @param o Перемещаемый список
     */
    Vector(Vector &&o, const allocator_type &alloc) : allocator(alloc)
    {
        if (alloc == o.allocator)
        {
            begin_ = o.begin_;
            end_ = o.end_;
            cap_ = o.cap_;
            o.begin_ = o.end_ = o.cap_ = nullptr;
        }
        else
        {
            size_type n = o.size();
            begin_ = allocate(n);
            end_ = begin_;
            cap_ = begin_ + n;
            for (size_type i = 0; i < n; ++i)
                std::construct_at(end_++, std::move(o[i]));
        }
    }

    /*!
     * @brief Конструктор, заполняющий список n копиями t
     * @param n Количество элементов
     * @param е Значение элемента, которым будет заполнен список
     */
    Vector(size_type n, const T &t, const allocator_type &alloc = allocator_type())
        requires std::copy_constructible<T>
        : allocator(alloc)
    {
        if (n == 0)
        {
            begin_ = end_ = cap_ = nullptr;
            return;
        }

        begin_ = allocate(n);
        end_ = begin_;
        cap_ = begin_ + n;

        for (size_type i = 0; i < n; ++i)
            construct_at(end_++, t);
    }

    /*!
     * @brief Конструктор, заполняющий список элементами из диапазона [i, j)
     * @param i Начало диапазона для копирования
     * @param j Конец диапазона для копирования
     */
    template <std::input_iterator It>
    Vector(It i, It j, const allocator_type &alloc = allocator_type())
        requires std::constructible_from<T, std::iter_reference_t<It>>
        : allocator(alloc)
    {
        size_type size = std::distance(i, j);
        if (size == 0)
        {
            begin_ = end_ = cap_ = nullptr;
            return;
        }

        begin_ = allocate(size);
        end_ = begin_;
        cap_ = begin_ + size;

        for (It it = i; it != j; ++it)
        {
            construct_at(end_++, *it);
        }
    }

    /*!
     * @brief Конструктор, заполняющий список элементами из списка инициализации
     * @param il inisializer_Vector, из которого будут скопированы значения
     */
    Vector(std::initializer_list<T> il, const allocator_type &alloc = allocator_type())
        requires std::move_constructible<T>
        : allocator(alloc)
    {
        size_type size = il.size();
        if (size == 0)
        {
            begin_ = end_ = cap_ = nullptr;
            return;
        }

        begin_ = allocate(size);
        end_ = begin_;
        cap_ = begin_ + size;

        for (auto it = il.begin(); it != il.end(); ++it)
            construct_at(end_++, std::move(*it));
    }

    /*!
     * @brief Копирующий оператор присваивания
     * @param o Копируемый список
     */
    Vector &operator=(const Vector &o)
        // Работает только если есть T() и T(a)
        requires std::copy_constructible<T>
    {
        if (this != &o)
        {
            Vector tmp(o);
            swap_state(tmp);
        }
        return *this;
    }

    /*!
     * @brief Перемещающий оператор присваивания
     * @param o Перемещаемый список
     */
    Vector &operator=(Vector &&o) noexcept
    {
        if (this != &o)
        {
            swap_state(o);
        }
        return *this;
    }

    /*!
     * @brief Деструктор
     */
    ~Vector()
    {
        destroy_range(begin_, end_);
        deallocate(begin_, capacity());
        begin_ = end_ = cap_ = nullptr;
    }

    //-------------------------------------------------Element access----------------------------------
    /*!
     * @brief Метод, присваивающий значения контейнеру
     * @param count - новый размер контейнера 
     * @param value - значение для инициализации элементов контейнера с помощью 
     * @param first,last — пара итераторов, определяющих исходный диапазон копируемых элементов. 
     * @param ilist — std::initializer_list для копирования значений из
     */
    void assign(size_type count, const T &value)
    {
        if (count > capacity())
        {
            T *new_begin = allocate(count);
            for (size_type i = 0; i < count; ++i)
            {
                std::construct_at(new_begin + i, value);
            }
            clear();
            deallocate(begin_, capacity());

            begin_ = new_begin;
            end_ = new_begin + count;
            cap_ = new_begin + count;
        }
        else
        {
            size_type old_size = size();

            size_type i = 0;
            for (; i < std::min(old_size, count); ++i)
                begin_[i] = value;
            for (; i < count; ++i)
                std::construct_at(begin_ + i, value);
            for (; i < old_size; ++i)
                std::destroy_at(begin_ + i);

            end_ = begin_ + count;
        }
    }

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last)
    requires std::constructible_from<T, std::iter_reference_t<InputIt>>
    {
        size_type n = std::distance(first, last);

        if (n > capacity())
        {
            T *new_begin = allocate(n);

            T *cur = new_begin;
            for (auto it = first; it != last; ++it, ++cur)
                std::construct_at(cur, *it);

            clear();
            deallocate(begin_, capacity());

            begin_ = new_begin;
            end_ = new_begin + n;
            cap_ = new_begin + n;
        }
        else
        {
            size_type old_size = size();

            size_type i = 0;
            auto it = first;

            for (; i < old_size && it != last; ++i, ++it)
                begin_[i] = *it;

            for (; it != last; ++it, ++i)
                std::construct_at(begin_ + i, *it);

            for (; i < old_size; ++i)
                std::destroy_at(begin_ + i);

            end_ = begin_ + n;
        }
    }

    void assign(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }


    /*!
    *@brief Геттер аллокатора
    */
    allocator_type get_allocator() const noexcept { return allocator; }

    /*!
    *@brief Доступ к указанному элементу с проверкой границ
    */
    reference at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range("Vector::at");
        return begin_[pos];
    }
    /*!
    *@brief Константный доступ к указанному элементу с проверкой границ
    */
    const_reference at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range("Vector::at");
        return begin_[pos];
    }

     /*!
    *@brief Доступ к указанному элементу без проверки границ
    */
    reference operator[](size_type pos)
    {
        return begin_[pos];
    }
    /*!
    *@brief Константный доступ к указанному элементу без проверки границ
    */
    const_reference operator[](size_type pos) const
    {
        return begin_[pos];
    }

    /*!
    *@brief Доступ к первому элементу
    */
    reference front()
    {
        assert(size() > 0);
        return begin_[0];
    }
    /*!
    *@brief Константный доступ к первому элементу
    */
    const_reference front() const
    {
        assert(size() > 0);
        return begin_[0];
    }

    /*!
    *@brief Доступ к последнему элементу
    */
    reference back()
    {
        assert(size() > 0);
        return *(end_ - 1);
    }
    /*!
    *@brief Константный доступ к последнему элементу
    */
    const_reference back() const
    {
        assert(size() > 0);
        return *(end_ - 1);
    }

    // ---------------------------------------------------Iterators------------------------------------------
    /*!
    *@brief Возвращает итератор в начало
    */
    iterator begin() noexcept { return iterator(begin_); }
    const_iterator begin() const noexcept { return const_iterator(begin_); }
    const_iterator cbegin() const noexcept { return const_iterator(begin_); }
    /*!
    *@brief Возвращает итератор в конец
    */
    iterator end() noexcept { return iterator(end_); }
    const_iterator end() const noexcept { return const_iterator(end_); }
    const_iterator cend() const noexcept { return const_iterator(end_); }
    /*!
    *@brief Возвращает обратный итератор в начало
    */
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    /*!
    *@brief Возвращает обратный итератор в конец
    */
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    //-------------------------------------------------------Capacity----------------------------------
   /*!
    *@brief Возвращает количество элементов
    */
    size_type size() const { return end_ - begin_; }
    /*!
    *@brief Проверяет, пуст ли контейнер
    */
    bool empty() const { return size() == 0; }
    /*!
    *@brief Возвращает максимально возможное количество элементов
    */
    size_type max_size() const { return std::allocator_traits<Alloc>::max_size(allocator); }

    /*!
    *@brief Возвращает количество элементов, которые могут храниться в выделенном на данный момент хранилище
    */
    size_type capacity() const { return cap_ - begin_; }
    /*!
    *@brief Хранилище резервов
    */
    void reserve(size_type new_cap)
    {
        if (new_cap <= capacity())
            return;

        T *new_data = allocate(new_cap);
        size_type sz = (*this).size();
        for (size_type i = 0; i < sz; ++i)
        {
            construct_at(new_data + i, std::move(begin_[i]));
        }

        destroy_range(begin_, end_);
        deallocate(begin_, capacity());

        begin_ = new_data;
        end_ = new_data + sz;
        cap_ = new_data + new_cap;
    }

    //------------------------------------------------------Modifiers-----------------------------------------------
    /*!
    *@brief Очищает содержимое
    */
    void clear()
    {
        destroy_range(begin_, end_);
        end_ = begin_;
    }

    /*!
     * @brief Вставка в список копии переданного значения
     * @param p Итератор, перед которым будет вставлен новый элемент
     * @param t Значения для вставки
     * @returns Итератор на вставленный элемент
     */
    iterator insert(const_iterator p, const T &t)
        requires std::copy_constructible<T>
    {
        size_type pos_index = p - cbegin();
        if (size() == capacity())
            reserve(capacity() == 0 ? 1 : capacity() * 2);

        T *pos_ptr = begin_ + pos_index;
        shift_right(pos_ptr, 1);
        construct_at(pos_ptr, t);
        return iterator(pos_ptr);
    }

    /*!
     * @brief Вставка в список переданного значения при помощи перемещения
     * @param p Итератор, перед которым будет вставлен новый элемент
     * @param t Значения для вставки
     * @returns Итератор на вставленный элемент
     */
    template <typename... Args>
    iterator insert(const_iterator p, T &&t)
        requires std::move_constructible<T>
    {
        size_type pos_index = p - cbegin();
        if (size() == capacity())
            reserve(capacity() == 0 ? 1 : capacity() * 2);

        T *pos_ptr = begin_ + pos_index;
        shift_right(pos_ptr, 1);
        construct_at(pos_ptr, std::move(t));
        return iterator(pos_ptr);
    }

    /*!
     * @brief Вставка в список n копий значения t
     * @param p Итератор, перед которым будут вставлены новые элементы
     * @param n Количество значений для вставки
     * @param t Значения для вставки
     * @returns Итератор на последний вставленный элемент
     */
    iterator insert(const_iterator p, size_type n, const T &t)
        requires std::copy_constructible<T>
    {
        if (n == 0)
            return iterator(begin_ + (p - cbegin()));

        size_type pos_index = p - cbegin();
        if (size() + n > capacity())
            reserve(std::max(capacity() * 2, size() + n));

        T *pos_ptr = begin_ + pos_index;
        shift_right(pos_ptr, n);
        for (size_type i = 0; i < n; ++i)
            construct_at(pos_ptr + i, t);

        return iterator(pos_ptr);
    }

    /*!
     * @brief Вставка в список элементов из диапазона [i, j)
     * @param p Итератор, перед которым будут вставлены новые элементы
     * @param i Начало диапазона
     * @param j Конец диапазона
     * @returns Итератор на последний вставленный элемент
     */
    template <std::input_iterator It>
    iterator insert(const_iterator p, It first, It last)
        requires std::copy_constructible<T>
    {
        size_type n = std::distance(first, last);
        if (n == 0)
            return iterator(begin_ + (p - cbegin()));

        size_type pos_index = p - cbegin();
        if (size() + n > capacity())
            reserve(std::max(capacity() * 2, size() + n));

        T *pos_ptr = begin_ + pos_index;
        shift_right(pos_ptr, n);

        T *cur = pos_ptr;
        for (It it = first; it != last; ++it, ++cur)
            construct_at(cur, *it);

        return iterator(pos_ptr);
    }

    /*!
     * @brief Вставка в список элементов из списка инициализации
     * @param p Итератор, перед которым будут вставлены новые элементы
     * @param il Список инициализации для вставки
     * @returns Итератор на последний вставленный элемент
     */
    iterator insert(const_iterator p, std::initializer_list<T> il)
        requires std::move_constructible<T>
    {
        return insert(p, il.begin(), il.end());
    }
    /*!
    *@brief Конструирует элемент на месте
    *@param p - итератор, перед которым будет создан новый элемент
    *@param args - аргументы для пересылки конструктору элемента
    */
    template <typename... Args>
    iterator emplace(const_iterator p, Args &&...args)
        requires std::constructible_from<T, Args...>
    {
        size_type pos_index = p - cbegin();

        if (size() == capacity())
            reserve(capacity() == 0 ? 1 : capacity() * 2);

        T *pos_ptr = begin_ + pos_index;
        shift_right(pos_ptr, 1);
        construct_at(pos_ptr, std::forward<Args>(args)...);

        return iterator(pos_ptr);
    }
    /*!
    *@brief Стирает элементы
    */
    iterator erase(const_iterator pos)
    {
        size_type idx = pos.ptr_ - begin_;

        std::destroy_at(begin_ + idx);
        std::move(begin_ + idx + 1, end_, begin_ + idx);
        --end_;

        return iterator(begin_ + idx);
    }
    /*!
    *@brief Стирает элементы
    */
    iterator erase(const_iterator first, const_iterator last)
    {
        size_type idx = first.ptr_ - begin_;
        size_type idx2 = last.ptr_ - begin_;
        size_type count = idx2 - idx;

        for (size_type i = idx; i < idx2; ++i)
            std::destroy_at(begin_ + i);
        std::move(begin_ + idx2, end_, begin_ + idx);
        end_ -= count;

        return iterator(begin_ + idx);
    }
    /*!
    *@brief Добавляет элемент в конец
    */
    void push_back(const T &value)
    {
        if (size() == capacity())
        {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }

        construct_at(end_, value);
        ++end_;
    }
    /*!
    *@brief Добавляет элемент в конец
    */
    void push_back(T &&value)
    {
        if (size() == capacity())
        {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        construct_at(end_, std::move(value));
        ++end_;
    }
    /*!
    *@brief Удаляет последний элемент
    */
    void pop_back()
    {
        if (end_ == begin_)
            return;
        --end_;
        destroy_at(end_);
    }

    /*!
    *@brief  Изменяет количество хранимых элементов
    *@param count — новый размер контейнера
    *@param value - значение, чтобы инициализировать новые элементы с помощью
    *@exception 
    */
    void resize(size_type count, const value_type &value)
    {
        size_type cur = size();
        if (count < cur)
        {
            while (size() > count)
            {
                --end_;
                destroy_at(end_);
            }
        }
        else if (count > cur)
        {
            if (count > capacity())
                reserve(count > capacity() * 2 ? count : capacity() * 2);

            while (size() < count)
            {
                construct_at(end_, value);
                ++end_;
            }
        }
    }

    /*!
    *@brief Изменяет количество хранимых элементов
    *@param count — новый размер контейнера
    */
    void resize(size_type count)
    {
        resize(count, T{});
    }

private:
    T *begin_ = nullptr; // начало массива
    T *end_ = nullptr;   // первый элемент за концом
    T *cap_ = nullptr;   // конец выделенной памяти

    allocator_type allocator;
    T *allocate(size_t n)
    {
        return std::allocator_traits<Alloc>::allocate(allocator, n);
    }
    // высвобождает объекты в памяти
    void deallocate(T *p, size_t n) noexcept
    {
        if (p)
            std::allocator_traits<Alloc>::deallocate(allocator, p, n);
    }

    template <typename... Args>
    void construct_at(T *p, Args &&...args)
    {
        std::allocator_traits<Alloc>::construct(allocator, p, std::forward<Args>(args)...);
    }
    // уничтожает объекты в памяти
    void destroy_at(T *p) noexcept
    {
        std::allocator_traits<Alloc>::destroy(allocator, p);
    }

    void destroy_range(T *first, T *last) noexcept
    {
        for (; first != last; ++first)
        {
            destroy_at(first);
        }
    }

    void shift_right(T *pos, size_type count)
    {
        T *end_pos_ = end_;
        end_ += count;
        for (T *p = end_ - 1; p >= pos + count; --p)
        {
            construct_at(p, std::move(*(p - count)));
            destroy_at(p - count);
        }
    }

    void swap_state(Vector &other) noexcept
    {
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(cap_, other.cap_);
        std::swap(allocator, other.allocator);
    }
};
