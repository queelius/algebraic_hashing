#pragma once

namespace alex::hash
{
    /**
     * HashValueType is a RegularType that overloads binary
     * operations +, ^, <<, and >> in a way that is consistent with integers.
     */
    template <typename HashValueType = unsigned int>
    class JenkinsHash
    {
    public:
        static const HashValueType DEFAULT_SEED = 0;    
        using hash_value_type = HashValueType;

        JenkinsHash(HashValueType seed = DEFAULT_SEED) : _seed(seed == 0 ? DEFAULT_SEED : seed) {};

        hash_value_type seed() const { return _seed; };

        template <typename ValueType>
        HashValueType operator()(ValueType xs) const
        {
            HashValueType hash = _seed;
            for (auto const & x : xs)
            {
                hash = hash + static_cast<HashValueType>(x);
                hash = hash + (hash << 10);
                hash = hash ^ (hash >> 6);
            }
            hash = hash + (hash << 3);
            hash = hash ^ (hash >> 11);
            hash = hash + (hash << 15);
            return hash;
        };

    private:
        HashValueType _seed;
    };
}