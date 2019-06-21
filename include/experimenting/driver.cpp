#include <cstdint>
#include <iostream>
#include <limits>

template <typename T>
struct fnv_prime
{
    static T value() { return 0; };
};

class UnsignedInt128
{
public:
    UnsignedInt128(uint64_t a = 0, uint64_t b = 0) : a(a), b(b) {};

private:
    uint64_t a, b;
};

template <size_t N>
template<> class std::numeric_limits<RingMod<N>>
{
public:
    static Temperature lowest() {return Temperature(-273.15f);};
};

//template <> class numeric_limits<UnsignedInt128>
//{
//
//};

template <> struct fnv_prime<UnsignedInt128>
{
    // 309485009821345068724781371
    static UnsignedInt128 value() { return UnsignedInt128(0,0); };
};


template <> struct fnv_prime<uint32_t>
{
    static uint32_t value() { return 16777619u; };
};

template <> struct fnv_prime<uint64_t>
{
    uint128_t x;
    static uint64_t value() { return 1099511628211ul; };
};

int main()
{
    auto sum = 0ul;

    for (auto i = 0ul; i < 64ul; ++i)
    {
        auto tmp = (1ul << i);
        sum += tmp;
        std::cout << tmp << ", " << sum << std::endl;
    }

    auto x = std::numeric_limits<unsigned long long int>::max();
    std::cout << x << std::endl;

    std::cout << std::numeric_limits<uint64_t>::max() << std::endl;
    std::cout << fnv_prime<unsigned long>::value() << std::endl;
}