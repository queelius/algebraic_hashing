#include <vector>
#include <algorithm>
#include <unordered_set>

namespace alex::hash
{
    /**
     * chd_perfect_hash_fn models a hash function in the family
     *     H::domain -> H::codomain
     * where H::codomain is some UnsignedIntegral type, e.g., unsigned int,
     * and H::domain is hashable by H.
     */
    template <typename H>
    class chd_perfect_hash_fn
    {
    public:
        using hash_fn = H;
        using codomain = typename H::codomain;
        using domain = typename H::domain;

        constexpr double min_load_factor() const { return 1e-5; }
        constexpr double max_load_factor() const { return 1; }
        constexpr double default_load_factor() const { return .5; }

        constexpr size_t default_buckets(size_t );

        chd_perfect_hash_fn(chd_perfect_hash_fn const &) = default;
        chd_perfect_hash_fn(chd_perfect_hash_fn &&) = default;

        template <class C> // C models some iterable Container type
        chd_perfect_hash_fn(
            C const & xs,
            size_t m = 0,
            double r = default_load_factor()) :
                chd_perfect_hash_fn(std::begin(xs), std::end(xs), m, r) {};

        /**
         * I models forward iterator
         * r is the load factor
         * m is the number of buckets
		 */
        template <class I>
        chd_perfect_hash_fn(
            I begin, I end,
            size_t m = 0,
            double r = default_load_factor())
        {
            _N = static_cast<hash_type>(std::ceil(std::distance(begin, end) / r));
        };

        codomain operator()(domain const & x) const
        {
            _h(x) % _N;

            codomain l2 = _h(x);
            l2 ^= 
            _sigma[]
        };

    private:
        domain _l;
        codomain _m, _N;
        hash_fn _h;
        std::vector<hash_fn> _sigma;
    };

    template <typename H>
    struct chd_perfect_hash_fn_builder
    {
        double r;
        size_t m;
        H h;

        chd_perfect_hash_fn_builder & hash_fn(H h)
        {
            this->h = h;
        };

        chd_perfect_hash_fn_builder & load_factor(double r)
        {
            this->r = r;
        };

        chd_perfect_hash_fn_builder & buckets(size_t m)
        {
            this->m = m;
        };

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
}