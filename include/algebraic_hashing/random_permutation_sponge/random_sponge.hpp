#pragma once

#include <cstdint>
#include "lcg.hpp"
#include <string_view>
#include <array>

using std::array;

unsigned char hash(unsigned char x) { return x; }

namespace cryptographic_hashing
{
template <
    size_t      R = 128,    // bit rate
    size_t      C = 64,     // bit capacity
    size_t      X = 0,      // seed for PRNG
    size_t      M = 0,      // minimum number of permutations
                            // per round
    typename    PRNG = alex_lcg<31>>
class sponge_fn
{
public:
    bool operator==(random_sponge<R,C,X,M,PRNG> const& rhs) const
    {
        return S == rhs.S;
    }

    template <int K>
    array<bool,R> operator()(array<bool,K> x) const
    {
        return apply(begin(x),end(x));
    }

    template <int N, typename I>
    array<bool, R*N> apply(I b, I e) const
    {
        PRNG p(X);
        array<bool,R+C> S(false)

        auto size = distance(b,e);
        auto B = size / R;
        decltype(size) k = 0;
        for (decltype(size) i = 0; i < B; ++i)
        {
            for (decltype(size) j = 0; j < R; ++j)
                S[j] ^= *(b + i*R+j)

            decltype(size) P = p() % (R+C);
            for (decltype(size) j = 0; j < P; ++j)
                swap(S[p() % (R+C)],S[k++ % (R+C)]);
        }

        array<bool,R*N> out;
        for (decltype(size) i = 0; i < N; ++i)
        {
            for (decltype(size) j = 0; j < R; ++j)
            {
                out[i*R + j] = S[j];
            }

            decltype(size) P = M + p() % (R+C - M);
            for (decltype(size) j = 0; j < P; ++j)
                swap(S[p() % (R+C)],S[k++ % (R+C)]);

        }

        return out;
    }

    constexpr auto rate() const { return R; }

    constexpr auto capacity() const { return C; }

    constexpr auto size() const { return R+C; }

    // S[0,...,R-1] is the byte rate section
    // S[R,...,C-1] is the byte capacity section
};

}
