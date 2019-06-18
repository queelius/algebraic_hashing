#include <vector>
#include <algorithm>
#include <unordered_set>

namespace alex::hash
{
    // ChdPerfectHashFn models a hash function in the family ValueType -> HashType
    // where HashType is some UnsignedInteger type, e.g., unsigned int, and
    //       and ValueType models some
    template <typename HashIndex>
    class ChdPerfectHashFn
    {
    public:
        using hash_index = HashIndex;
        using hash_type = typename HashIndex::HashType;
        using hash_fn_type = typename HashIndex::hash_fn_type;

        ChdPerfectHashFn(ChdPerfectHashFn const &) = default;
        ChdPerfectHashFn(ChdPerfectHashFn &&) = default;

        /**
         * I models forward iterator
         * r is the load factor
         * m is the number of buckets
         *
         * Algorithm:
         *  (1) Split S into buckets B[i] = g^(-1)({i}) and S, 0 <= i < r;
         *  (2) Sort buckets B[i] in falling order according to size |B[i]|
		 *      (O(n) time, since numbers to be sorted are small);
         *  (3) Initialize array T[0 . . .m-1] with 0�s;
         *  (4) for all i in {0,...,r-1}, in the order from (2), do:
         *  (5) for L = 1, 2, ... repeat forming K[i] = {sigma[L](x) | x in B[i]}
                until (|K[i]| == |B[i]|) and (K[i] and {j | T[j] = 1} = empty;
         *  (7) let sigma = the successful sigma[L];
         *  (8) for all j in K[i] let T[j] = 1;
         *  (9) optional: transform (sigma(i)) 0<= i < r into compressed form, retaining O(1) access.
		 */
        template <class I>
        ChdPerfectHashFn(I begin, I end, size_t m, double r = .99)
        {
            if (r <= 0. || r > 1.)
                throw std::invalid_argument("load factor 'r' must be a value in (0, 1]");
            if (m == 0)
                throw std::invalid_argument("parameter 'm' must be an integer greater than zero");

            _m = static_cast<hash_type>(m);
            _N = static_cast<hash_type>(std::ceil(std::distance(begin, end) / r));

            typedef std::vector<std::string> Bucket;
            std::vector<std::pair<uint32_t, Bucket>> B(m);            
            std::unordered_set<uint32_t> T;
            std::unordered_set<uint32_t> K;
            _sigma.resize(m, 0);

            for (uint32_t i = 0; i < m; ++i)
                B[i].first = i;

            for (auto key = begin; key != end; ++key)
                B[_index(0)(*key) % m].second.push_back(*key);

            std::sort(B.begin(), B.end(),
                [](const std::pair<uint32_t, Bucket>& b1,
                   const std::pair<uint32_t, Bucket>& b2)
                    { return b1.second.size() > b2.second.size(); });

            for (uint32_t i = 0; i < m; ++i)
            {
                uint32_t l = 1;
                uint32_t j = 0;
        
                K.clear();
                while (j < B[i].second.size())
                {
                    auto slot = _index(l)(B[i].second[j]) % N;
                    if (T.count(slot) != 0 || K.count(slot) != 0)
                    {
                        ++l;
                        j = 0;
                        K.clear();
                    }
                    else
                    {
                        K.insert(slot);
                        ++j;
                    }
                }

                _sigma[B[i].first] = l;
                for (auto j : K)
                    T.insert(j);
            }
        };

        template <typename X>
        hash_type operator()(X x) const
        {
            return _index(_sigma[_index(0)(x) % _m])(x) % _N;
        };

    private:
        hash_index _index;
        hash_type _m, _N;
        std::vector<index_type> _sigma;
    };
}