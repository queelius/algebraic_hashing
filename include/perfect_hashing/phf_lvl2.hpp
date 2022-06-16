#pragma once

#include <vector>
#include <cmath>

namespace perfect_hashing
{
    /**
     * chd_phf models a hash function in the family
     *     Hashable(H) -> size_t
     * where Hashable(H) is any value type that is hashable by H.
     */
    template <typename H>
    struct phf_lvl2
    {
        using hash_fn = H;

        phf_lvl2(phf_lvl2 const &) = default;
        phf_lvl2(phf_lvl2 &&) = default;

        phf_lvl2(size_t N, size_t m, H h, std::vector<size_t> sigma) :
            N(N), m(m), h(h), sigma(sigma) {}

        template <typename X> // X is hashable by H
        auto operator()(X const & x) const
        {
            return (h(x) ^ sigma[h(x) % m]) % N;
        }

        size_t const N;
        size_t const m;
        H const h;
        std::vector<size_t> const sigma;
    };
}