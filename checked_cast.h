/*
 * Safe type cast for function call decorator.
 * @maintainer xavier dot roche at algolia.com
 */

#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>

/*
 * Internal: throw a bad cast error.
 * Implementation note: mark function as not inlined to prevent local (inlined) hot code increase,
 * and mark function as cold to move code in far sections
 */
[[noreturn]] inline void __attribute((noinline, cold)) checked_cast_throw(const char* const pretty)
{
    throw std::runtime_error(std::string("checked_cast<>() overflowed: ") + std::string(pretty));
}

/**
 * Template numerical conversion.
 * @param v The source numerical value
 * @return The destination numerical value, or the original pointer for non-numerical types
 * @comment Inspired by Bjarne Stroustrup's in "The C++ Programming Language 4th Edition"
 **/
template<class Target, class Source>
auto inline constexpr checked_cast(Source v)
{
    if constexpr (std::is_same<Target, Source>::value) {
        return v;
    } else if constexpr (!std::is_integral<Source>::value) {
        return v;
    } else {
        static_assert(!std::is_reference<Source>::value);
        const auto r = static_cast<Target>(v);
        if (static_cast<Source>(r) != v)
            checked_cast_throw(__PRETTY_FUNCTION__);
        return r;
    }
}

/*
 * Container holding a type, and allowing to return a cast runtime-checked casted value.
 * @example
 *     const size_t my_integer = foo();
 *     const checked_cast_call_container c(my_integer);
 *     int a = static_cast<int>(c);
 */
template<typename T>
class checked_cast_call_container
{
public:
    inline constexpr checked_cast_call_container(T result)
      : _result(result)
    {}

    template<typename U>
    inline constexpr operator U() const
    {
        return checked_cast<U>(_result);
    }

    template<typename U>
    inline constexpr bool operator==(const U& other) const
    {
        // Note: we can test without case here
        return _result == other;
    }

    template<typename U>
    inline constexpr bool operator!=(const U& other) const
    {
        return !((*this) == other);
    }

private:
    const T _result;
};

/**
 * Wrapped call to a function, with runtime-checked casted input and output values.
 * @example checked_cast_call<&my_function>(str, 1, size, output)
 * @comment This version is using templated function pointer.
 */
template<auto fn, typename... Args>
auto constexpr checked_cast_call(Args... args)
{
    return checked_cast_call_container(fn(checked_cast_call_container(args)...));
}

/**
 * Wrapped call to a function, with runtime-checked casted input and output values.
 * @example checked_cast_call(my_function_ptr, str, 1, size, output)
 * @example checked_cast_call(my_function, str, 1, size, output)
 * @comment This version is using parametrized function pointer.
 */
template<typename Fun, typename... Args>
auto constexpr checked_cast_call(Fun fn, Args... args)
{
    return checked_cast_call_container(fn(checked_cast_call_container(args)...));
}
