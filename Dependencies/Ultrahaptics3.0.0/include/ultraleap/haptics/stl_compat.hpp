#pragma once

#include "ultraleap/haptics/common.hpp"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iterator>
#include <istream>
#include <sstream>

/** \cond EXCLUDE_FROM_DOCS */

namespace uh
{

ULH_API void* allocate(size_t size);
ULH_API void* alloczero(size_t size);
ULH_API void deallocate(void* memory);

template <class T>
class vector_wrapper
{
    size_t size = 0;
    T* d = nullptr;

public:
    vector_wrapper(const vector_wrapper<T>&) = delete;
    vector_wrapper& operator=(const vector_wrapper<T>&) = delete;
    vector_wrapper& operator=(vector_wrapper<T>&&) = delete;

    vector_wrapper(vector_wrapper<T>&& o)
    {
        size = o.size;
        d = o.d;
        o.d = nullptr;
    }

    template <class U>
    operator std::vector<U>() const
    {
        std::vector<U> v;
        v.reserve(size);
        for (size_t i = 0; i < size; ++i)
            v.push_back(std::move(d[i]));
        return v;
    }

    template <class U>
    vector_wrapper(std::vector<U>&& us)
        : size(us.size())
    {
        if (size == 0)
            return;
        d = static_cast<T*>(::uh::allocate(size * sizeof(T)));
        for (size_t i = 0; i < size; ++i)
            new (d + i) T(std::move(us[i]));
    }

    ~vector_wrapper()
    {
        if (d == nullptr)
            return;
        for (size_t i = 1; i <= size; ++i)
            d[size - i].~T();
        ::uh::deallocate(d);
    }
};

class string_wrapper
{
    size_t size = 0;
    char* d = nullptr;

public:
    string_wrapper& operator=(const string_wrapper&) = delete;
    string_wrapper& operator=(string_wrapper&&) = delete;

    string_wrapper(string_wrapper&& o)
    {
        size = o.size;
        d = o.d;
        o.d = nullptr;
    }

    operator std::string() const
    {
        return std::string(d, size);
    }

    string_wrapper(std::string&& s)
        : size(s.size())
    {
        if (size == 0)
            return;
        d = static_cast<char*>(::uh::allocate(size));
        s.copy(d, size);
    }

    ~string_wrapper()
    {
        ::uh::deallocate(d);
    }
};

template <char delimiter>
class word_delimited_by : public std::string
{};

template <char c>
std::istream& operator>>(std::istream& is, word_delimited_by<c>& output)
{
    std::getline(is, output, c);
    return is;
}

template <char delimiter>
std::vector<std::string> split(std::string input_str)
{
    std::istringstream iss(input_str);
    return { std::istream_iterator<word_delimited_by<delimiter>>(iss),
        std::istream_iterator<word_delimited_by<delimiter>>() };
}

} // namespace uh

/** \endcond */