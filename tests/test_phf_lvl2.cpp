#include <hashing/fnv_hash.hpp>
#include <perfect_hashing/phf_lvl2.hpp>
#include <perfect_hashing/phf_lvl2_builder.hpp>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include "utils.hpp"

void test_phf_lvl2();

int main()
{
    test_phf_lvl2();
}

void test_phf_lvl2()
{
    size_t n = 400;
    size_t seed = 1001;
    auto xs = random_strings(n,30,seed);

    auto start = std::chrono::system_clock::now();
    auto ph = perfect_hashing::phf_lvl2_builder<hashing::fnv_hash>().
        load_factor(.7).timeout(std::chrono::seconds(10))(xs);
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

    std::cout << "elapsed time: " << (double)elapsed.count() / 1000 << '\n';

    size_t collisions = 0;
    std::unordered_set<size_t> hashes;
    for (auto x : xs)
    {
        auto h = ph(x);
        if (hashes.count(h) != 0)
            ++collisions;
        hashes.insert(h);
    }

    std::cout << "collisions = " << collisions << "\n";
    std::cout << "collision rate = " << (double)collisions / n << "\n";
}
