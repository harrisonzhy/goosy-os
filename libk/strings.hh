#pragma once
#include "int.hh"
#include "iterator.hh"

struct str {
    public :
        template<usize S>
        constexpr str(char const (&string)[S]) : _str(string), _size(S - 1) {}
        constexpr str(char const* string, usize size) : _str(string), _size(size - 1) {}

        auto constexpr len() const -> usize { return _size; }

        auto constexpr begin() const -> iterator<char const> { return iterator<char const>(_str); }

        auto constexpr end() const -> iterator<char const> { return iterator<char const>(_str + _size); }

        auto constexpr operator[](usize const i) const -> char const& { return _str[i]; }

    private:
        char const* _str;
        usize const _size;
};