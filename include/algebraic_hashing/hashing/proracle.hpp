#pragma once

#include <compare>
#include <algebraic_hashing/hashing/hash_value.hpp>
#include <array>

namespace algebraic_hashing::hashing
{
    // H models a hash function
    // proracle<H> models a pseudo random oracle
    template <typename H>
    struct proracle
    {
        using hash_element_type = typename H::hash_type;

        struct hash
        {
            H h;
            hash_element_type h_x;
            auto operator[](size_t i) const
            {
                return h.mix(h_x,i);
            }

            friend auto operator<=>(hash const &, hash const &) = default;

            template <size_t N=8>
            auto head() const
            {
                std::array<hash_element_type,N> val;
                for (size_t i = 0; i < N; ++i)
                    val[i] = operator[](i);
                return hashing::hash_value<hash_element_type,N>{val};
            }
        };

        using hash_type = hash;

        template <typename X> // X is hashable by H
        auto operator()(X const & x) const
        {
            return hash{h,h(x)};
        }

        friend auto operator<=>(proracle const &, proracle const &) = default;        

        H h;
    };
}

