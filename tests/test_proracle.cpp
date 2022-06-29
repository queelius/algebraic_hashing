#include <iostream>
#include <hashing/fnv_hash.hpp>
#include <algebraic_hashing/proracle.hpp>
#include <string>
#include <unordered_set>


template <size_t N=4>
void test_proracle_head_with_fnv(std::string x = "test abc");

void test_single_hash_proracle_with_fnv(std::string x = "test abc",
                                        size_t n = 10000000);

template <size_t N=100>
void test_proracle_collisions_with_fnv(std::string x = "test abc");

int main()
{
    test_proracle_head_with_fnv<8>();
}

template <size_t N=4>
void test_proracle_head_with_fnv(std::string x)
{
    algebraic_hashing::proracle<hashing::fnv_hash> o;
    auto hh = o(x);
    std::cout << "hash[j]: ";
    for (size_t j = 0; j < N; ++j)
    {
        std::cout << hh[j] << " ";
    }
    std::cout << "\n";

    std::cout << "head   : ";
    auto hhh = hh.head<N>();
    for (auto const & x : hhh)
    {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

void test_single_hash_proracle_with_fnv(std::string x, size_t n)
{
    algebraic_hashing::proracle<hashing::fnv_hash> o;
    auto hh = o(x);

    std::unordered_set<size_t> hs;
    for (size_t i = 0; i < n; ++i)
    {
        auto hash = hh[i];
        if (hs.count(hash) != 0)
        {
            std::cout << "Collision at i=" << i << " with hash=" << hash << "\n";
        }
        hs.insert(hash);
    }
}


template <size_t N>
void test_proracle_collisions_with_fnv(std::string x)
{
    algebraic_hashing::proracle<hashing::fnv_hash> o;
    auto hh = o(x);

    std::unordered_set<hashing::hash_value<char,N>> hs;
    for (size_t t = 0; t < 100000000; ++t)
    {
        auto hash = hh.head<N>();
    }
}
