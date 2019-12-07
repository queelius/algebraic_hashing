#pragma once

#include <unordered_set>
#include <cmath>

namespace alex::hash
{
    /**
     * random_perfect_hash<H> models a perfect hash function of type
     * H::domain -> H::codomain where H is a hash function of type
     *     H::domain -> H::codomain,
     * where H::domain is a regular type that overloads (^=) and (%=).
     * 
     * A perfect hash function f must overload the unary function call operator
     *     f : X -> Y.
     */

    template <typename H>
    class random_perfect_hash_fn
    {
    public:
        using hash_fn = H;
        using codomain = typename H::codomain;
        using domain = typename H::domain;

        constexpr double min_load_factor() const { return 1e-5; }
        constexpr double max_load_factor() const { return 1; }
        constexpr double default_load_factor() const { return .5; }

        random_perfect_hash_fn(random_perfect_hash_fn const &) = default;
        random_perfect_hash_fn(random_perfect_hash_fn &&) = default;

        // I models a forward iterator
        template <typename I>
        random_perfect_hash_fn(
            I begin,
            I end,
            H h,
            double r = default_load_factor()) :
                _h(h),
                r(std::max(min_load_factor(), std::min(max_load_factor(), r))),
                _N(static_cast<codomain>(std::ceil(std::distance(begin,end)/r)))
        {
            std::unordered_set<uint32_t> K;
            for (_l = 0; ; ++l)
            {
                for (auto x = begin; x != end; ++x)
                {
                    auto hash = _h(*x);
                    hash ^= _l;
                    hash %= _N;

                    if (K.count(hash) != 0)
                    {
                        K.clear();
                        break;
                    }
                    K.insert(hash);
                }
                if (!K.empty())
                    break;
            }
        }

        codomain operator()(domain const & x)
        {
            auto hash = _h(x);
            hash ^= _l;
            hash %= _N;
            return hash;
        };

        codomain max() const { return _N-1; };
        constexpr codomain min() const { return 0; };

    private:
        codomain const _N;
        H _h;
        domain _l;
    };
}