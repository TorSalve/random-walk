#pragma once

#include <iostream>
#include <vector>

namespace RandomWalk::Utils {

    std::vector<double> linspace(double start, double end, int num_in)
    {

        std::vector<double> linspaced;

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
    }

    void print_vector(std::vector<double> vec)
    {
        std::cout << "size: " << vec.size() << std::endl;
        for (double d : vec)
            std::cout << d << " ";
        std::cout << std::endl;
    }
}