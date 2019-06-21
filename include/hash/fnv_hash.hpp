#pragma once

#include <variant>
#include <tuple>
#include <optional>

namespace alex::hash
{
    template <typename T>
    concept bool Xorable = requires(T x)
    {
        { x ^ x } -> T;
    };

    template <typename T>
    concept bool Multipliable = requires(T x)
    {
        { x * x } -> T;
    };

    template <typename T1, typename T2>
    concept bool Convertible = requires(T1 x)
    {
        { static_cast<T1>(x) } -> T2;
    };

    template <typename T>
    concept bool FnvHashValue = Xorable<T> && Multipliable<T> && Convertible<T, unsigned int>;


    template <typename T>
    concept bool Iterable() 
    {
        return requires(T x)
        {
            { x.begin() };
            { x.end() };
        } || requires (T x)
        {
            { std::begin(x) };
            { std::end(x) };
        } || requires (T x)
        {
            { begin(x) };
            { end(x) };
        };

    };

    /*/template<typename T>
    concept Hashable = requires(T x)
    {
        { std::hash<T>{}(x) } -> std::size_t;
    };
    */

    template <typename T>
    concept bool FnvHashValueType = Xorable<T> && Multipliable<T> && Convertible<T, unsigned int>;

    /**
     * HashValueType is a RegularType that overloads binary
     * operations * and ^ in a way that is consistent with
     * integers. It can also be constructed from integers.
     */
    template <FnvHashValueType T = unsigned int>
    class FnvHash
    {
    public:
        static const T FNV_PRIME = 16777619u;
        static const T DEFAULT_SEED = FNV_PRIME;
        using hash_value_type = T;

        T seed() const { return _seed; };

        FnvHash(T seed = DEFAULT_SEED) : _seed(seed == 0 ? DEFAULT_SEED : seed) {};

        /**
         * ValueType is an a type that can be statically cast to HashValueType
         * or is an iterable collection of ValueType.
         */
        template <typename V>
        T operator()(V const & v) const
        {
            return _hash(v, _seed);
        };

    private:
        T _seed;

        template <size_t I = 0, typename... V>
        T _hash(std::tuple<V ...> const & v, T h) const
        {
            std::cout << "[tuple]\n";
            static const T TUPLE_HASH_TAG = ;

            h = _hash(TUPLE_HASH_TAG, h);
            h = _hash(std::get<I>(v), h);

            if constexpr(I+1 != sizeof...(V))
                h = _hash<I+1>(v, h);
            return h;
        };

        // tuple_cdr -> for projections on relations / cartesian products?
        // tuple_interlace -> re-arranging the order of tuples, maybe like rename in relational algebra?
        // tuple_cat - concat two tuples. good for "flattening" them.

        /*
        template <size_t I = 0, typename... V>
        T _hash(std::variant<V ...> const & v, T h) const
        {
            std::cout << "[variant]\n";
            static const T VARIANT_HASH_TAG = 1007;

            h = _hash(VARIANT_HASH_TAG, h);
            h = _hash(std::get<I>(v), h);
            if constexpr(I+1 != sizeof...(V))
                h = _hash<I+1>(v, h);
            return h;
        };
        */

        template <typename V>
        T _hash(std::optional<V> const & x, T h) const
        {
            if (x.has_value())
                return _hash(*x, h);
            else
                return _hash(NULL, h);
        };

        T _hash(Iterable const & v, T h) const
        {
            std::cout << "[iterable]\n";
            static const T ITERABLE_HASH_TAG = 29;

            h = _hash(ITERABLE_HASH_TAG, h);
            for (auto const & x : v)
            {
                h = _hash(x, h);
            }
            return h;
        };

        template <typename V> requires Convertible<V,T>
        T _hash(V const & v, T h) const
        {
            std::cout << "[non-iterable]\n";

            return h * FNV_PRIME ^ static_cast<T>(v);
        };

    };
}