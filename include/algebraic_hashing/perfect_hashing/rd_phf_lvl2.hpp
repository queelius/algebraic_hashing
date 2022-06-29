#pragma once

#include <vector>
#include <cmath>

namespace algebraic_hashing::perfect_hashing
{
    /**
     * phf_lvl2 models a hash function in the family
     *     Hashable(H) -> size_t
     * where Hashable(H) is any value type that is hashable by H.
     */
    template <typename H>
    struct rd_phf_lvl2
    {
        using hash_fn = H;

        rd_phf_lvl2(rd_phf_lvl2 const &) = default;
        rd_phf_lvl2(rd_phf_lvl2 &&) = default;

        auto & operator=(rd_phf_lvl2 const & rhs)
        {
            N = rhs.N;
            m = rhs.m;
            l0 = rhs.l0;
            err = rhs.err;
            sigma = rhs.sigma;
            return *this;
        }

        rd_phf_lvl2(size_t N, size_t m, size_t l0, double error,
                    H h, std::vector<size_t> sigma) :
            N(N), m(m), l0(l0), err(error), h(h), sigma(sigma) {}

        auto max_hash() const { return N-1; }

        template <typename X> // X is hashable by H
        auto operator()(X const & x) const
        {
            auto h0 = h.mix(l0,x) % m;
            return h.mix(sigma[h0],x) % N;
        }

        auto error_rate() const { return err; }

        size_t N, m, l0;
        double err;
        H h;
        std::vector<size_t> sigma;
    };
}