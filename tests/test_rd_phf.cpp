#include <perfect_hashing/rd_phf_builder.hpp>
#include <hashing/fnv_hash.hpp>
#include <perfect_hashing/rd_phf.hpp>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include "utils.hpp"

void test_phf(size_t n = 10000);

int main()
{
    test_phf();
}

void test_phf(size_t n)
{
    auto xs = random_strings(n,30,21001);
    auto start = std::chrono::system_clock::now();

    auto ph = perfect_hashing::rd_phf_builder<hashing::fnv_hash>().
        load_factor(.15).
        timeout(std::chrono::seconds(300)).
        index(0ul,10000000000000ul)(xs);

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);

    std::cout << (double)elapsed.count() / 1000 << " seconds\n";
    std::cout << "error rate = " << ph.error_rate() << "\n";

    size_t coll = 0;
    size_t max_hash = 0;
    size_t min_hash = 100000000;
    std::unordered_set<size_t> hashes;
    for (auto const x : xs)
    {
        auto hash = ph(x);
        max_hash = hash > max_hash ? hash : max_hash;
        min_hash = hash < min_hash ? hash : min_hash;

        if (hashes.count(hash) != 0)
            ++coll;
        hashes.insert(hash);
    }
    std::cout << "min hash: " << min_hash << "\n";
    std::cout << "max hash: " << max_hash << "\n";
    std::cout << "error rate: " << (double)coll / n << "\n";
}
