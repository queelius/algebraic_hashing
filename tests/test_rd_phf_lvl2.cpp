#include <hashing/fnv_hash.hpp>
#include <perfect_hashing/rd_phf_lvl2.hpp>
#include <perfect_hashing/rd_phf_lvl2_builder.hpp>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include "utils.hpp"

void test_phf_lvl2(size_t n = 5000, size_t seed = 311102001);

int main()
{
    test_phf_lvl2();
}

void test_phf_lvl2(size_t n, size_t seed)
{
    auto xs = random_strings(n,30,seed);

    auto start = std::chrono::system_clock::now();
    auto ph = perfect_hashing::rd_phf_lvl2_builder<hashing::fnv_hash>().
        load_factor(.7).
        index(1,250).
        level0(0).
        //debugging(true).
        timeout(std::chrono::seconds(480))(xs);

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

    std::cout << "elapsed time: " << (double)elapsed.count() / 1000 << '\n';
    std::cout << "error rate: " << ph.error_rate() << "\n";
    std::cout << ph.l0 << "\n";

    size_t colls = 0;
    std::vector<bool> hashes(ph.max_hash()+1,false);
    for (auto x : xs)
    {
        auto hash = ph(x);
        if (hashes[hash])
            ++colls;
        hashes[hash] = true;
    }
    std::cout << "error rate = " << (double)colls / n << "\n";
}
