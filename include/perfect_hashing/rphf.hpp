#pragma once

#include <cmath>

namespace hashing::perfect_hashing
{
    /**
     * @brief Models the concept of a perfect hash function.
     *
     * rphf<H> models a perfect hash function of type
     *     Hashable(H) -> size_t
     * where H is a hash function of type
     *     Hashable(H) -> size_t.
     *
     * This is a maximum entropy representation of a
     * perfect hash function, since:
     *
     *   - It doesn't include any information about the number of
     *     elements in the perfectly hashed set, only that it is
     *     between min() and max().
     *
     *   - It doesn't contain information about which are the
     *     elements of S.
     *
     *
     *
     */
    template <typename H>
    class rphf
    {
    public:
        using hash_fn = H;
        using hash_type = typename H::hash_type;

        constexpr double min_load_factor() { return 1e-20; }
        constexpr double max_load_factor() { return 0.99; }

        rphf(random_phf const &) = default;
        rphf(random_phf &&) = default;

        /**
         * @brief Constructs a random perfect hash function.
         *
         * @tparam I models a forward iterator
         * @tparam H models a hash function
         * @param begin start of forward iterator
         * @param end end of forward iterator
         * @param lf load factor of the perfect hash function constructed
         *           if max_iter is reached, then the load factor for the
         *           perfectly hashed subset S of [begin,end) has lf as
         *           an upper-bound.
         */
        template <typename I>
        rphf(
            I begin,
            I end,
            H h,
            double lf = 0.5) : h_(h)
        {
            auto m = std::distance(begin,end);
            lf = std::max(min_load_factor(), std::min(max_load_factor(), lf));
            N_ = (size_t)std::ceil(m/lf);
            std::vector<bool> K(N_);
            bool collide;
            for (l_ = (size_t)0; ; ++l_)
            {
                collide = false;
                for (auto x = begin; x != end; ++x)
                {
                    auto h = (h_(*x) ^ l_) % N_;
                    if (K[h]) // collision
                    {
                        K.assign(N_,false); // reinit to all false
                        collide = true;
                        break;
                    }
                    K[h] = true;
                }

                if (!collide)
                    break;
                ++l_;
            }
        }

        template <typename X> // X is hashable by H
        auto operator()(X const & x) { return (h_(x) ^ l_) % N_; }

        auto max() const { return N_-1; }
        auto min() const { return (size_t)0; }

    private:
        hash_type N_;
        H h_;
        hash_type l_;
    };
}
