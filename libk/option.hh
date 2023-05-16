#pragma once
#include "int.hh"

static auto constexpr const None = false;
static auto constexpr const Some = true;

template<typename T>
class Option {
    public :
        constexpr Option(const T& value) : _val(value), _present(Some) {}
        constexpr Option() : _present(None) {}

        auto constexpr none() -> bool { return !_present; }
        auto constexpr some() -> bool { return _present; }

        auto constexpr unwrap() -> T& { return _val; }
        auto constexpr unwrap() const -> const T& { return _val; }
        void constexpr assign(const T& value) { _val = value; _present = Some; }
        
        auto constexpr operator ! () const -> bool { return !_present; }
        auto constexpr operator == (Option<T>& value) -> bool { return _val == value.val; }
        void constexpr operator = (const T& value) { _val = value; _present = Some; }

    private :
        T _val;
        bool _present;
};
template <typename T>
Option(T) -> Option<T>;

template<typename T>
class Option<T&> {
    public :
        constexpr Option(const T& value) : _val(&value) {}
        constexpr Option(T& value) : _val(&value) {}
        constexpr Option() : _val(nullptr) {}

        auto constexpr none() -> bool { return !_val; }
        auto constexpr some() -> bool { return _val;  }

        auto constexpr unwrap() -> T& { return *_val; }
        auto constexpr unwrap() const -> const T& { return *_val; }
        void constexpr assign(const T& value) { _val = &value; }
        
        auto constexpr operator ! () const -> bool { return !_val; }
        auto constexpr operator == (Option<T>& value) -> bool { return _val == value.val; }
        void constexpr operator = (const T& value) { _val = &value; }

    private :
        T* _val;
};

template <typename T>
Option(T&) -> Option<T&>;