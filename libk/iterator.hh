#pragma once

template<typename T>
struct iterator {
    public : 
        constexpr iterator(T* ptr) : _ptr(ptr) {};
    
        auto constexpr operator * () const -> T& { return *_ptr; }

        auto constexpr operator -> () const -> T* { return _ptr; }

        auto constexpr operator ++ () -> iterator& { ++_ptr; return *this; }

        auto constexpr operator ++ (signed) -> iterator { iterator t = *this; ++(*this); return t; }

        auto constexpr friend operator == (iterator const& u, iterator const& v) -> bool { return u._ptr == v._ptr; };

        auto constexpr friend operator != (iterator const& u, iterator const& v) -> bool { return u._ptr != v._ptr; };

    private:
        T* _ptr;
};