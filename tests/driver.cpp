#include <iostream>
#include <string_view>
#include <string>
#include <vector>
#include <tuple>
#include <variant>
#include "hash/fnv_hash.hpp"
#include "hash/rnd_perf_hash.hpp"
#include "RingMod.hpp"
#include <cmath>
#include <tuple>

template <size_t N>
std::ostream& operator<<(std::ostream& out, alex::ringmod::RingMod<N> a)
{
    for (size_t i = N-1; i < N; --i)
        out << a.digits[i];
    return out;
};

int main()
{
    using namespace alex::ringmod;

    std::cout << alex::hash::fnv_hash::fnv_hash<int>(3) << std::endl;

    //alex::hash::FnvHash<unsigned int> h(13ul);
    //auto a = std::make_tuple<std::string,int>(
    //    std::string("now is the time for all the good men to come to the aid of their country"), 10);
    //std::variant<std::tuple<std::string,int>,std::string_view> x = a;
}