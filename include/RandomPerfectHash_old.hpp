#include <algorithm>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace alex::hash
{
class RandomPerfectHash
{
public:
    using size_type = size_t;
    using hash_type = uint32_t;
    using value_type = std::string_view;

    RandomPerfectHash(const RandomPerfectHash& copy)
        : _l(copy._l)
        , _N(copy._N){};

    template <typename Iterator>
    RandomPerfectHash(double r, Iterator begin, Iterator end)
    {
        _N = std::distance(begin, end) / r;
        std::unordered_set<uint32_t> K;
        for (_l = 0;; ++_l)
        {
            for (auto k = begin; k != end; ++k)
            {
                auto h = _hash(_l, *k) % _N;
                if (K.count(h) != 0)
                {
                    K.clear();
                    break;
                }
                K.insert(h);
            }
            if (!K.empty())
                break;
        }
    };

    hash_type operator()(value_type key) const { return _hash(_l, key) % _N; };

private:
    uint32_t _l, _N;

    hash_type _hash(uint32_t d, std::string_view s)
    {
        static const hash_type FNV_PRIME = 16777619;
        if (d == 0)
            d = FNV_PRIME;

        for (auto c : s)
            d = (d * FNV_PRIME) ^ c;

        return d;
    };
};
}
