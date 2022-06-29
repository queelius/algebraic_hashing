#pragma once

#include "aligned_bit_span.h"
#include "lcg.hpp"
#include "random_bit_permuter.hpp"
#include <string_view>
#include <array>

using std::string_view;
using std::array;
using alex::span::aligned_bit_span;

namespace alex {

template <unsigned int R, // byte rate
    unsigned int       C, // byte capacity
    unsigned int       X, // seed for F
    typename F = random_bit_permuter<alex_lcg<31>>>
class sponge
{
public:
    template <size_t K>
    using hash_t = array<unsigned char,K>

    sponge()
        : state(S, R + C)
        , rate(S, R) {};

    sponge(F f)
        : f(f)
        , state(S, R + C)
        , rate(S, R) {};

    template <size_t N, typename I>
    hash_t<R*N> hash(I begin, I end)
    {
        return hash<N>(vector<unsigned char>(begin,end).data());
    }

    template <size_t N>
    hash_t<R*N> hash(string_view x)
    {
        return hash<N>(x.data(), x.size());
    }

    template <size_t N>
    hash_t<R*N> hash(void* data, size_t size)
    {
        // reset state of permutation function
        f.state() = X;

        auto vals = static_cast<unsigned char*>(data);

        // state S is initialized to zeros
        memset(S, 0, R + C);

        const auto full_blocks = size / R;
        for (size_t i = 0; i < full_blocks; ++i)
        {
            rate ^= aligned_bit_span(vals + i * R, R);
            f.permute(state.begin(), state.end());
        }

        const auto left = size % R;
        if (left != 0)
        {
            rate ^= aligned_bit_span(vals + full_blocks * R, left);
            f.permute(state.begin(), state.end());
        }

        std::array<unsigned char, R*N> out;
        for (size_t i = 0; i < N; ++i)
        {
            for (size_t j = 0; j < R; ++j)
                out[i * R + j] = S[j];
            f.permute(state.begin(), state.end());
        }

        return out;
    }

    constexpr auto byte_rate() const { return R; }

    constexpr auto byte_capacity() const { return C; }

private:
    // S[0,...,R-1] is the byte rate section
    // S[R,...,C-1] is the byte capacity section
    unsigned char    S[R + C];
    aligned_bit_span state;
    aligned_bit_span rate;

    // permutation function.
    F f;
};

}
