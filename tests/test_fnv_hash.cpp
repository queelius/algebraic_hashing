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
        std::cout << "======================================\n";
    }

    hashing::fnv_hash h2;
    std::cout << "(h == h2): " << (h == h2) << "\n";
    std::cout << "(h != h2): " << (h != h2) << "\n";
    std::cout << "(h < h2): " << (h < h2) << "\n";
    std::cout << "(h > h2): " << (h > h2) << "\n";
    std::cout << "(h <= h2): " << (h <= h2) << "\n";
    std::cout << "(h >= h2): " << (h >= h2) << "\n";
}



void test_fnv_collisions()
{
    std::unordered_set<size_t> hs;
    hashing::fnv_hash h;
    for (size_t i = 0; ; ++i)
    {
        auto hash = h(i);
        if (i % 1000000 == 0) std::cout << "i=" << i << " => hash=" << hash << "\n";
        if (hs.count(hash) != 0)
        {
            std::cout << "Collision at i=" << i << " with hash=" << hash << "\n";
        }
        hs.insert(hash);
    }
}
