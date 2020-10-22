#include <algorithm>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace alex::hash
{

template <>
class random_perfect_hash<std::string_view>
{
public:
    using size_type = size_t;
    using hash_type = size_t;
    using value_type = std::string_view;

    random_perfect_hash(random_perfect_hash const & copy)
        : _l(copy._l)
        , _N(copy._N) {};

    template <typename I>
    random_perfect_hash(double r, I begin, I end)
    {
        _N = std::distance(begin, end) / r;
        std::unordered_set<size_t> K;
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

    size_t operator()(value_type key) const { return _hash(_l, key) % _N; };

private:
    size_t _l, _N;

    size_t _hash(size_t d, std::string_view s)
    {
        static const size_t FNV_PRIME = 16777619;
        if (d == 0)
            d = FNV_PRIME;

        for (auto c : s)
            d = (d * FNV_PRIME) ^ c;

        return d;
    };
};
}