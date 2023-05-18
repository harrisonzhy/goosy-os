#pragma once
#include "int.hh"
#include "iterator.hh"

struct str {
    public :
        template<usize S>
        constexpr str(char const (&string)[S]) : t_str(string), t_size(S - 1) {}
        constexpr str(char const* string, usize size) : t_str(string), t_size(size - 1) {}

        auto constexpr len() const -> usize {
            return t_size;
        }

        auto constexpr begin() const -> iterator<const char> {
            return iterator<const char>(t_str);
        }

        auto constexpr end() const -> iterator<const char> {
            return iterator<const char>(t_str + t_size);
        }

        auto constexpr operator[](const usize i) const -> const char& {
            return t_str[i];
        }

    private:
        const char* t_str;
        usize t_size;
};