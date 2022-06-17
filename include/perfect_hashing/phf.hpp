#pragma once

#include <cmath>

namespace perfect_hashing
{
    /**
     * @brief Models the concept of a perfect hash function.
     *
     * phf<H> models a perfect hash function of type
     *     Hashable(H) -> size_t
     * where H is a hash function of type
     *     Hashable(H) -> size_t
     * using only one level of hashing.
     * 
     * @tparam H hash function type
     */
    template <typename H>
    struct phf
    {
        using hash_fn_type = H;
        using hash_type = typename H::hash_type;

        phf(phf const &) = default;
        phf(phf &&) = default;
        phf(size_t N, H h, size_t l, double err) :
            N(N), h(h), l(l), err(err) {}

        /**
         * @brief retrieves the minimum hash value
         * @tparam X the element type, must be hashable by H.
         * @param x the element to retrieve the hash of.
         */

        template <typename X>
        auto operator()(X const & x)
        {
            //return (h(x) ^ l) % N;
            return h.mix(h(x),l) % N;
        }

        /**
         * @brief retrieves the maximum hash value
         */
        auto max() const { return N-1; }

        /**
         * @brief retrieves the minimum hash value
         */
        auto min() const { return (size_t)0; }

        /**
         * @brief retrieves the error rate
         * 
         * The error rate is defined by the ratio the number of elements in the
         * set that are perfectly hashed to the number of elements in the set.
         */
        auto error_rate() const { return err; }

        /**
         * @brief retrieves the hash function that is used by phf
         */
        auto hash_fn() const { return h; }

        size_t const N;
        H const h;
        size_t const l;
        double const err;
    };
}
