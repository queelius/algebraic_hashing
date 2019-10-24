#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

namespace alex::hash
{
    struct Hash
    {
        uint32_t operator()(uint32_t d, const std::string& s)
        {
            static const uint32_t FNV_PRIME = 16777619;
            if (d == 0)
                d = FNV_PRIME;

            for (auto c : s)
                d = (d * FNV_PRIME) ^ c;

            return d;
        };
    };

    class PerfectHash
    {
    public:
        virtual int find(const std::string&) const = 0;
    };

    template <class H>
    class RPH: public PerfectHash
    {
    public:
        template <class Iter>
        void generate(Iter begin, Iter end, double r = .5)
        {
            _N = keys.size() / r;
            std::unordered_set<uint32_t> K;
            for (_l = 0; ; ++_l)
            {
                for (const auto& k : keys)
                {
                    auto h = _h(_l, k) % _N;
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
        }

        int find(const std::string& key) const
        {
            return hash(_l, key) % _N;
        };

    private:
        uint32_t _l, _N;
        H _h;
    };

    template <class H>
    class CHD: public PerfectHash
    {
    public:
		/*
			(1) Split S into buckets B[i] = g^(−1)({i}) and S, 0 <= i < r;
			(2) Sort buckets B[i] in falling order according to size |B[i]|
				(O(n) time, since numbers to be sorted are small);
			(3) Initialize array T[0 . . .m−1] with 0’s;
			(4) for all i in {0,...,r-1}, in the order from (2), do:
				(5) for L = 1, 2, ... repeat forming K[i] = {sigma[L](x) | x in B[i]}
				    until (|K[i]| == |B[i]|) and (K[i] and {j | T[j] = 1} = empty;
				(7) let sigma = the successful sigma[L];
				(8) for all j in K[i] let T[j] = 1;
			(9) optional: transform (sigma(i)) 0<= i < r into compressed form, retaining O(1) access.
		*/
        template <class Iter>
        void generate(Iter begin, Iter end, uint32_t m, double r = .99)
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
                   const std::pair<uint32_t, Bucket>& b2)
                    { return b1.second.size() > b2.second.size(); });

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

        int find(const std::string& key) const
        {
            return _h(_sigma[hash(0, key) % _m], key) % _N;
        };

    private:
        H _h;
        uint32_t _m, _N;
        std::vector<uint32_t> _sigma;
    };
}