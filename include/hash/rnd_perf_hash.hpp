#pragma once

#include <unordered_set>
#include <cmath>

namespace alex::hash
{
    /**
     * RandomPerfectHash<HashIndex> models a perfect hash function
     * of type X -> hash_value_type where X is any type hashable by
     * hash functions in HashIndex and hash_value_type is the output of hash
     * functions in HashIndex.
     * 
     * A perfect hash f must overload the unary function call operator
     *     f : X -> hash_type.
     *       
     * HashIndex models an indexed family of hash functions X -> hash_value_type.
     * There must be a constructor HashIndex(HashIndex::index_type) to construct
     * hash functions in the family, e.g., HashIndex(k) constructs the k-th hash function. 
     */
    template <typename HashIndex>
    class RandomPerfectHash
    {
    public:
        using hash_index = HashIndex;
        using index_type = typename HashIndex::index_type;
        using hash_type = typename HashIndex::hash_type;
        using hash_value_type = typename HashIndex::hash_value_type;

        RandomPerfectHash(RandomPerfectHash const &) = default;
        RandomPerfectHash(RandomPerfectHash&&) = default;

        // I models a forward iterator
        template <typename I>
        RandomPerfectHash(I begin, I end, hash_index index, double r = .5)
        {
            if (r <= 0.) r = std::numeric_limits<double>::epsilon();
            if (r > 1.)  r = 1.;

            _N = static_cast<hash_value_type>(std::ceil(std::distance(begin,end) / r));
            std::unordered_set<uint32_t> K;
            for (index_type l = 0; ; ++l)
            {
                _h = index(l);
                for (auto x = begin; x != end; ++x)
                {
                    auto hash = _h(*x) % _N;
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

        template <typename X>
        hash_value_type operator()(X x)
        {
            return _h(x) % _N;
        };

    private:
        hash_value_type _N;
        hash_type _h;
    };
}