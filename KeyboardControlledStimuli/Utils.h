#pragma once

#include <iostream>
#include <vector>

#include <ultraleap/haptics/vector3.hpp>

namespace RandomWalk::Utils {

    template <typename IntType>
    std::vector<IntType> linspace(IntType start, IntType end, int num_in) {

        std::vector<IntType> linspaced;

        double num = static_cast<double>(num_in);

        if (num == 0) { return linspaced; }
        if (num == 1)
        {
            linspaced.push_back(start);
            return linspaced;
        }

        double delta = (end - start) / (num - 1);

        for (int i = 0; i < num - 1; ++i)
        {
            linspaced.push_back(start + delta * i);
        }
        linspaced.push_back(end); // I want to ensure that start and end
                                  // are exactly the same as the input
        return linspaced;
    };
    
    template <typename IntType>
    void print_vector(std::vector<IntType> vec) {
        std::cout << "size: " << vec.size() << std::endl;
        for (double d : vec)
            std::cout << d << " ";
        std::cout << std::endl;
    };
    
    constexpr unsigned int hash(const char* s, int off = 0) {
        return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
    };


    template <typename IntType>
    std::vector<IntType> range(IntType start, IntType stop, IntType step)
    {
        if (step == IntType(0))
        {
            throw std::invalid_argument("step for range must be non-zero");
        }

        std::vector<IntType> result;
        IntType i = start;
        while ((step > 0) ? (i < stop) : (i > stop))
        {
            result.push_back(i);
            i += step;
        }

        return result;
    }

    template <typename IntType>
    std::vector<IntType> range(IntType start, IntType stop)
    {
        return range(start, stop, IntType(1));
    }

    template <typename IntType>
    std::vector<IntType> range(IntType stop)
    {
        return range(IntType(0), stop, IntType(1));
    }
}