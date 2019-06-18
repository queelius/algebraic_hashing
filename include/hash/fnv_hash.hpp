#pragma once

namespace alex::hash
{
    /**
     * HashValueType is a RegularType that overloads binary
     * operations * and ^ in a way that is consistent with
     * integers. It can also be constructed from integers.
     */
    template <typename HashValueType = unsigned int>
    class FnvHash
    {
    public:
        static const HashValueType FNV_PRIME = 16777619;
        static const HashValueType DEFAULT_SEED = FNV_PRIME;
        using hash_value_type = HashValueType;

        hash_value_type seed() const { return _seed; };

        FnvHash(HashValueType seed = DEFAULT_SEED) : _seed(seed == 0 ? DEFAULT_SEED : seed) {};

        /**
         * ValueType is an iterable collection of type T
         * where T can be statically cast to HashValueType.
         */
        template <typename ValueType>
        HashValueType operator()(ValueType xs) const
        {
            HashValueType hash = _seed;
            for (auto const & x : xs)
            {
                hash = (hash * FNV_PRIME)
                    ^ static_cast<HashValueType>(x);
            }
            return hash;
        };

    private:
        HashValueType _seed;
    };
}