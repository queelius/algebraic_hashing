#pragma once
#include <vector>
#include <cstdint>
#include <string_view>

namespace alex::hash
{
    template <typename T>
    struct DefaultHash
    {
        uint32_t operator()(uint32_t d, std::basic_string_view<T> s)
        {
            static const uint32_t FNV_PRIME = 16777619;
            if (d == 0)
                d = FNV_PRIME;

            for (auto c : s)
                d = (d * FNV_PRIME) ^ c;

            return d;
        };
    };

    // parameterized by RandomAccessContainer,
    // which may be std::vector, but may be
    // something else also, like a packed container of
    // unsigned integers of k bits each. all of the
    // burden will be, i guess, with a ChdBuilder type,
    // or maybe just the container.
    template
    <
        class H = DefaultHash<char>,
        class C = std::vector<uint32_t>
    >
    class Chd
    {
    public:
        // [int] * [string<T>] -> [int]
        using hash_function_type = H;
        using container_type = C;

        int operator()(std::string_view key) const
        {
            return _h(_sigma[_h(0, key) % _m], key) % _N;
        };

    private:
        H _h;
        uint32_t _m, _N;
        C _sigma;
    };
}