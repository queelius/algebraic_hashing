#include <iostream>
#include <hashing/fnv_hash.hpp>
#include <string>
#include "utils.hpp"

int main()
{
    hashing::fnv_hash h;

    auto xs = random_strings(1000);
    auto ys = random_elements<int>(1000);

    for (size_t i = 0; i < xs.size(); ++i)
    {
        std::cout << h(xs[i]) << "\n";
        std::cout << h(std::optional<std::string>{xs[i]}) << "\n";
        std::cout << h(std::variant<std::string,int>{xs[i]}) << "\n";
        std::cout << h(std::variant<std::string,int>{ys[i]}) << "\n";
    }
    
}