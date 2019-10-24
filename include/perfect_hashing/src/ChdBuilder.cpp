#include <perfect_hashing/ChdBuilder.h>

template <typename Iterator>
alex::hash::ChdBuilder& alex::hash::ChdBuilder::set_keys(
    Iterator begin, Iterator end)
{
}

/** CHD build algorihtm:
 *  (1) Split S into buckets B[i] = g^(-1)({i}) and S, 0 <= i < r
 *  (2) Order buckets by decreasing size, B[i] < B[i+1] if |B[i]| < |B[i+1]|
 *  (3) Initialize array T[0 . . .m-1] with zeros
 *  (4) for all i in {0,...,r-1}, in the order from (2), do
 *          for L = 1, 2, ...
 *              let K[i] = {sigma[L](x) | x in B[i]}
 *              if |K[i]| == |B[i]| and {j | T[j] = 1} = empty;
                    sigma <- sigma[L];
    *          T[j] <- 1 for all j in K[i];
    *   (5) optional: transform (sigma(i)) 0<= i < r into compressed form,
    *       retaining O(1) access.
    */
template <typename H>
alex::hash::Chd<H> alex::hash::ChdBuilder::build()
{
    _m = m;
    typedef std::vector<std::string> Bucket;
    std::vector<std::pair<uint32_t, Bucket>> B(m);
    _N = std::distance(begin, end) / r;
    _sigma.resize(m, 0);
    std::unordered_set<uint32_t> T;
    std::unordered_set<uint32_t> K;

    for (uint32_t i = 0; i < m; ++i)
        B[i].first = i;

    for (auto key = begin; key != end; ++key)
        B[_h(0, *key) % m].second.push_back(*key);

    std::sort(B.begin(), B.end(),
        [](const std::pair<uint32_t, Bucket>& b1,
            const std::pair<uint32_t, Bucket>& b2) {
            return b1.second.size() > b2.second.size();
        });

    for (uint32_t i = 0; i < m; ++i)
    {
        uint32_t l = 1;
        uint32_t j = 0;

        K.clear();
        while (j < B[i].second.size())
        {
            auto slot = _h(l, B[i].second[j]) % N;
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
}
