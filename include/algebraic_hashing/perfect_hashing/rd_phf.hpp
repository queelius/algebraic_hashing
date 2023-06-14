#pragma once

#include <cmath>

namespace algebraic_hashing {
/**
 * @brief Models the concept of a rate-distorted perfect hash function.
 *
 * rd_phf<H> models a perfect hash function of type
 *     Hashable(H) -> size_t
 * where H is a hash function of type
 *     Hashable(H) -> size_t
 * using only one level of hashing.
 * 
 * @tparam H hash function type
 */
template <typename H>
struct rd_phf
{
    using hash_fn_type = H;
    using hash_type = typename H::hash_type;

    rd_phf(rd_phf const &) = default;
    rd_phf(rd_phf &&) = default;
    rd_phf(size_t N, size_t l0, double err, H h) :
        N(N), l0(l0), err(err), h(h) {}

    /**
     * @brief retrieves the minimum hash value
     * @tparam X the element type, must be hashable by H.
     * @param x the element to retrieve the hash of.
     */
    template <typename X>
    auto operator()(X const & x) { return h.mix(l0,x) % N; }

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

    auto operator<=>(rd_phf const &) const = default;

    size_t const N, l0;
    double const err;
    H const h;
};

template <typename H>
constexpr bool is_eq(rd_phf<H> const & lhs,
                        rd_phf<H> const & rhs)
{
    return lhs.N == rhs.N &&
            lhs.l == rhs.l &&
            lhs.err == rhs.err &&
            lhs.h == rhs.h;
}

} // namespace algebraic_hashing
