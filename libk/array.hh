#pragma once
#include "int.hh"
#include "option.hh"
#include "iterator.hh"

template<typename T, usize S>
class Array {
    public :
        void constexpr fill(T const& element) {
            for (usize i = 0; i < S; ++i) {
                m_arr[i] = element;
            }
        }

        [[nodiscard]] auto constexpr len() -> usize { return S; }

        // account for both non-`const' and `const' objects
        [[nodiscard]] auto constexpr begin() -> iterator<T> { return iterator<T>(&m_arr[0]); }
        [[nodiscard]] auto constexpr begin() const -> iterator<T> { return iterator<T>(&m_arr[0]); }

        [[nodiscard]] auto constexpr end() -> iterator<T> { return iterator<T>(&m_arr[S]); }
        [[nodiscard]] auto constexpr end() const -> iterator<T> { return iterator<T>(&m_arr[S]); }

        [[nodiscard]] auto constexpr penultimate() -> iterator<T> { return iterator<T>(&m_arr[S - 1]); }
        [[nodiscard]] auto constexpr penultimate() const -> iterator<T> { return iterator<T>(&m_arr[S - 1]); }

        [[nodiscard]] auto constexpr first() -> T& { return m_arr[0]; }
        [[nodiscard]] auto constexpr first() const -> const T& { return m_arr[0]; }

        [[nodiscard]] auto constexpr last() -> T& { return m_arr[S - 1]; }
        [[nodiscard]] auto constexpr last() const -> const T& { return m_arr[S - 1]; }

        [[nodiscard]] auto constexpr data() -> T* { return &m_arr[0]; }
        [[nodiscard]] auto constexpr data() const -> const T* { return &m_arr[0]; }

        [[nodiscard]] auto constexpr at(usize i) const -> Option<T> {
            if (i < S) {
                return { m_arr[i] };
            }
            return { }; // check for out-of-bounds access
        }
        
        [[nodiscard]] auto constexpr get(usize i) const -> Option<T> { return at(i); }

        auto constexpr operator [] (usize i) -> T& { return m_arr[i]; };
        auto constexpr operator [] (usize i) const -> T const& { return m_arr[i]; };

        T m_arr[S];
};

template<typename T, typename ... Types>
Array(T, Types ...) -> Array<T, sizeof ... (Types) + 1>;