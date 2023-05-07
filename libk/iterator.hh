#pragma once

template<typename T>
struct iterator {
    public : 
        constexpr iterator(T* ptr) : ptr(ptr) {};
        auto constexpr operator * () const -> T& {
            return *ptr;
        }

        auto constexpr operator -> () const -> T* {
            return ptr;
        }

        auto constexpr operator ++ () -> iterator& {
            ++ptr;
            return *this;
        }

        auto constexpr operator ++ (signed) -> iterator {
            iterator t = *this;
            ++(*this);
            return t;
        }

        auto constexpr friend operator == (const iterator& u, const iterator& v) -> bool {
            return u.ptr == v.ptr;
        };

        auto constexpr friend operator != (const iterator& u, const iterator& v) -> bool {
            return u.ptr != v.ptr;
        };

    private:
        T* ptr;
};