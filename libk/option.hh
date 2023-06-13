#pragma once
#include "int.hh"

auto constexpr const None = false;
auto constexpr const Some = true;

template<typename T>
class Option {
    public :
        constexpr Option(T const& value) : _val(value), _present(Some) {}
        constexpr Option() : _present(None) {}

        auto constexpr none() -> bool { return !_present; }
        auto constexpr some() -> bool { return _present; }

        auto constexpr unwrap() -> T& { return _val; }
        auto constexpr unwrap() const -> const T& { return _val; }
        void constexpr assign(T const& value) { _val = value; _present = Some; }
        
        auto constexpr operator ! () const -> bool { return !_present; }
        auto constexpr operator == (Option<T>& value) -> bool { return _val == value.val; }
        void constexpr operator = (T const& value) { _val = value; _present = Some; }

    private :
        T _val;
        bool _present;
};

template<typename T> Option(T) -> Option<T>;

auto constexpr const Nok = false;
auto constexpr const Ok  = true;

template<typename T>
class Result {
    public :
        constexpr Result(T const& value) : _ok(Ok) {}
        constexpr Result() : _ok(Nok) {}

        auto constexpr ok() -> bool { return _ok; }
        auto constexpr nok() -> bool { return !_ok; }

    private :
        bool _ok;
};

template<typename T> Result(T) -> Result<T>;

template<typename T>
class Option<T&> {
    public :
        constexpr Option(T& value) : _val(&value) {}
        constexpr Option(T const& value) : _val(&value) {}
        constexpr Option() : _val(nullptr) {}

        auto constexpr none() -> bool { return !_val; }
        auto constexpr some() -> bool { return _val;  }

        auto constexpr unwrap() -> T& { return *_val; }
        auto constexpr unwrap() const -> const T& { return *_val; }
        void constexpr assign(T const& value) { _val = &value; }
        
        auto constexpr operator ! () const -> bool { return !_val; }
        auto constexpr operator == (Option<T>& value) -> bool { return _val == value.val; }
        void constexpr operator = (T const& value) { _val = &value; }

    private :
        T* _val;
};

template <typename T> Option(T&) -> Option<T&>;