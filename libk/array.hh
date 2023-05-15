#pragma once
#include "int.hh"
#include "option.hh"
#include "iterator.hh"

template<typename T, usize S>
class Array {
    public :
        void constexpr fill(const T& element) {
            for (usize i = 0; i < S; ++i) {
                arr[i] = element;
            }
        }

        [[nodiscard]] auto constexpr len() -> usize { return S; }

        // account for both non-`const' and `const' objects
        [[nodiscard]] auto constexpr begin() -> iterator<T>       { return iterator<T>(&arr[0]); }
        [[nodiscard]] auto constexpr begin() const -> iterator<T> { return iterator<T>(&arr[0]); }

        [[nodiscard]] auto constexpr end() -> iterator<T>         { return iterator<T>(&arr[S]); }
        [[nodiscard]] auto constexpr end() const -> iterator<T>   { return iterator<T>(&arr[S]); }

        [[nodiscard]] auto constexpr first() -> T&                { return arr[0]; }
        [[nodiscard]] auto constexpr first() const -> const T&    { return arr[0]; }

        [[nodiscard]] auto constexpr last() -> T&                 { return arr[S - 1]; }
        [[nodiscard]] auto constexpr last() const -> const T&     { return arr[S - 1]; }

        [[nodiscard]] auto constexpr data() -> T*                 { return &arr[0]; }
        [[nodiscard]] auto constexpr data() const -> const T*     { return &arr[0]; }

        // checks for out-of-bounds accesses
        [[nodiscard]] auto constexpr at(usize i) const -> Option<T> {
            if (i < S) {
                return { arr[i] };
            }
            return { };
        }

        auto constexpr operator[](usize i) -> T& { return arr[i]; };
        auto constexpr operator[](usize i) const -> const T& { return arr[i]; };

        T arr[S];
};

template<typename T, typename ... Types>
Array(T, Types ...) -> Array<T, sizeof ... (Types) + 1>;