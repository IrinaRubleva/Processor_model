#pragma once
#include <iterator>
#include <type_traits>


template <typename T, bool is_const>
class VectorIterator{
public:
    
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef std::conditional_t<is_const, const T, T>* pointer;
    typedef std::conditional_t<is_const, const T, T>& reference;
    typedef std::random_access_iterator_tag iterator_category;
    
    VectorIterator() noexcept: ptr_(nullptr) {}
    explicit VectorIterator(pointer p) noexcept : ptr_(p) {}

    template<bool other_const>
    requires (is_const || !other_const)
    VectorIterator(const VectorIterator<T, other_const>& o) noexcept : ptr_(o.ptr_) {}


    template<bool other_const>
    VectorIterator& operator = (const VectorIterator<T, other_const>& o) noexcept {
        ptr_ = o.ptr_;
        return *this;
    }

 
    reference operator * () const noexcept {return *ptr_;}
    pointer operator -> () const noexcept {return ptr_;}
    reference operator[](difference_type n) const noexcept { return *(ptr_ + n); }

    bool operator==(const VectorIterator& other) const noexcept { return ptr_ == other.ptr_; }
    bool operator!=(const VectorIterator& other) const noexcept { return ptr_ != other.ptr_; }
    bool operator<(const VectorIterator& other) const noexcept { return ptr_ < other.ptr_; }
    bool operator>(const VectorIterator& other) const noexcept { return ptr_ > other.ptr_; }
    bool operator<=(const VectorIterator& other) const noexcept { return ptr_ <= other.ptr_; }
    bool operator>=(const VectorIterator& other) const noexcept { return ptr_ >= other.ptr_; }


    VectorIterator& operator ++ () noexcept {++ptr_; return *this;}
    VectorIterator operator ++ (int) noexcept {VectorIterator tmp = *this; ++(*this); return tmp;}

    VectorIterator& operator -- () noexcept {--ptr_; return *this;}
    VectorIterator operator -- (int) noexcept {VectorIterator tmp = *this; --(*this); return tmp;}

    VectorIterator operator+(difference_type n) const noexcept { return VectorIterator(ptr_ + n); }
    friend VectorIterator operator+(difference_type n, const VectorIterator& it) noexcept {return it + n;}
    VectorIterator operator-(difference_type n) const noexcept { return VectorIterator(ptr_ - n); }
    VectorIterator& operator+=(difference_type n) noexcept { ptr_ += n; return *this; }
    VectorIterator& operator-=(difference_type n) noexcept { ptr_ -= n; return *this; }
    difference_type operator-(const VectorIterator& other) const noexcept { return ptr_ - other.ptr_; }

private:
    pointer ptr_;
    
    template<typename, typename> friend class Vector;

    
    friend class VectorIterator<T, !is_const>;
    template <typename, bool> friend class VectorIterator;

};


static_assert(std::random_access_iterator<VectorIterator<int, false>>);
static_assert(std::random_access_iterator<VectorIterator<int, true>>);