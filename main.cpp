/**
 * Safe type cast for function call decorator.
 * @maintainer xavier dot roche at algolia.com
 */

#include "checked_cast.h"
#include <string>
#include <cassert>
#include <cstdio>

// Demo of function taking small input size data
static char my_write(const void*, char size, FILE*)
{
    // Pretend we wrote everything
    return size;
}

// Demo of normal write function
static ssize_t my_write_ok(const void*, size_t size, FILE*)
{
    // Pretend we wrote everything
    return size;
}

inline std::string first_colon(const std::string& s)
{
    return s.substr(0, s.find(':'));
}

template<typename A, typename B>
inline void assertEqual(A a, B b)
{
    assert(a == b);
}

int main(int, char**)
{
    // Should pass
    {
        const char str[100] = "Hello!\n";
        const char b = checked_cast_call<my_write>(str, sizeof(str), stdout);
        assertEqual((size_t)b, sizeof(str));
    }

    // Should pass
    {
        const char str[100] = "Hello!\n";
        const char b = checked_cast_call<my_write_ok>(str, sizeof(str), stdout);
        assertEqual((size_t)b, sizeof(str));
    }

    {
        // Input overflow: input argument nmemb is larger than 127
        try {
            const char str[142] = "Hello!\n";
            const char b = checked_cast_call<my_write>(str, sizeof(str), stdout);
            (void)b;
            assert(!"call should have failed");
        } catch (const std::runtime_error& e) {
            assertEqual(first_colon(std::string(e.what())), std::string("checked_cast<>() overflowed"));
        }
    }

    {
        // Return overflow: return value 256 is larger than 127
        try {
            const char str[142] = "Hello!\n";
            const char c = checked_cast_call<my_write_ok>(str, sizeof(str), stdout);
            (void)c;
            assert(!"call should have failed");
        } catch (const std::runtime_error& e) {
            assertEqual(first_colon(std::string(e.what())), std::string("checked_cast<>() overflowed"));
        }
    }

    {
        // Input overflow: input argument nmemb is larger than 127
        try {
            const char str[256] = "Hello!\n";
            const char b = checked_cast_call<my_write>(str, sizeof(str), stdout);
            (void)b;
            assert(!"call should have failed");
        } catch (const std::runtime_error& e) {
            assertEqual(first_colon(std::string(e.what())), std::string("checked_cast<>() overflowed"));
        }
    }

    {
        // Return overflow: return value 256 is larger than 127
        try {
            const char str[256] = "Hello!\n";
            const char c = checked_cast_call<my_write_ok>(str, sizeof(str), stdout);
            (void)c;
            assert(!"call should have failed");
        } catch (const std::runtime_error& e) {
            assertEqual(first_colon(std::string(e.what())), std::string("checked_cast<>() overflowed"));
        }
    }

    return 0;
}
