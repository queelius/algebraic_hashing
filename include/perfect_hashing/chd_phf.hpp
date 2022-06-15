#include <vector>
#include <algorithm>
#include <unordered_set>

namespace hashing::perfect_hashing
{
    /**
     * chd_phf models a hash function in the family
     *     Hashable(H) -> size_t
     * where Hashable(H) is any value type that is hashable by H.
     */
    template <typename H>
    class chd_phf
    {
    public:
        using hash_fn = H;

        constexpr double min_load_factor() const { return 1e-5; }
        constexpr double max_load_factor() const { return 0.99; }

        chd_phf(chd_phf const &) = default;
        chd_phf(chd_phf &&) = default;

        template <typename I> // C models some iterable Container type
        chd_phf(
            I begin,
            I end,            
            size_t m = 0,
            double lf = 0.5)
        {
            using X = typename iterator_traits<I>::value_type;
            struct entry
            {
                size_t index;
                std::vector<X> xs;
            };

            std::vector<entry> B(m);            
            std::unordered_set<size_t> T;
            std::unordered_set<size_t> K;
            sigma_.resize(m, 0);

            for (size_t i = 0; i < m; ++i)
                B[i].index = i;

            for (auto x = begin; x != end; ++x)
                B[h_(*x) % m].xs.push_back(*x);

            std::sort(B.begin(), B.end(),
                [](auto const & b1, auto const & b2)
                    { return b1.xs.size() > b2.xs.size(); });

            for (size_t i = 0; i < m; ++i)
            {
                size_t l = 1;
                size_t j = 0;
        
                K.clear();
                while (j < B[i].xs.size())
                {
                    auto h = (h_(B[i].xs[j]) ^ l) % N_;
                    if (T.count(h) != 0 || K.count(h) != 0)
                        { ++l; j = 0; K.clear(); }
                    else
                        { K.insert(h); ++j; }
                }

                sigma_[B[i].index] = l;
                for (auto j : K)
                    T.insert(j);
            }
        }

        template <typename X> // X is hashable by H
        auto operator()(X const & x) const
        {
            return (h_(x) ^ sigma_[h_(*x) % m_]) % N_;
        }

    private:
        size_t N_, m_;
        hash_fn h_;
        std::vector<size_t> sigma_;
    };
}