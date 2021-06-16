#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <iterator>

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
        for (IntType d : vec)
            std::cout << d << " ";
        std::cout << std::endl;
    };
    
    template <typename IntType>
    void print_vector(std::string_view comment, std::vector<IntType> vec) {
        std::cout << comment;
        print_vector(vec);
    }

    template <typename IntType>
    void print_map(const std::map<std::string, IntType>& m)
    {
        for (const auto& [key, value] : m) {
            std::cout << key << " = " << value << "; ";
        }
        std::cout << "\n";
    }

    template <typename IntType>
    void print_map(std::string_view comment, const std::map<std::string, IntType>& m) {
        std::cout << comment;
        print_map(m);
    }
    
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

    // https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }

    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
}