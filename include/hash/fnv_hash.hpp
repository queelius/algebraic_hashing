#pragma once

#include <variant>
#include <tuple>
#include <optional>
#include <string_view>
#include <string>
#include <typeinfo>

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

    template <typename T>
    concept bool FnvHashValueType = Xorable<T> && Multipliable<T>; // && Convertible<T, unsigned int>;

    /**
     * HashValueType is a RegularType that overloads binary
     * operations * and ^ in a way that is consistent with
     * integers. It can also be constructed from integers.
     */
    template <typename T>
    class FnvHash
    {
    public:
        using hash_value_type = T;

        T seed() const { return _seed; };

        FnvHash(T seed) : _seed(seed) {};

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
            static const T TUPLE_TAG = 31ul;
            
            h = _hash(TUPLE_TAG, std::move(h));
            h = _hash(std::get<I>(v), std::move(h));

            if constexpr(I+1 != sizeof...(V))
                h = _hash<I+1>(v, std::move(h));
            return h;
        };

        // tuple_cdr -> for projections on relations / cartesian products?
        // tuple_interlace -> re-arranging the order of tuples, maybe like rename in relational algebra?
        // tuple_cat - concat two tuples. good for "flattening" them.

        template <typename... V>
        T _hash(std::variant<V ...> v, T h) const
        {
            std::cout << "[variant]\n";
            static const T VARIANT_TAG = 1007;
            h = _hash(VARIANT_TAG, std::move(h));

            std::visit([&h, this](const auto& x) { h = this->_hash(x, std::move(h)); }, v);
            return h;
        };

        template <typename V>
        T _hash(std::optional<V> x, T h) const
        {
            std::cout << "[optional]\n";
            const static T OPTIONAL_TAG = T(29ul);
            h = _hash(OPTIONAL_TAG, std::move(h));
            if (x.has_value())
                return _hash(*x, std::move(h));
        };

        T _hash(std::string v, T h) const
        {
            std::cout << "[string]\n";
            const static T STRING_TAG = T(31ul);

            h = _hash(STRING_TAG, std::move(h));
            for (auto x : v)
            {
                h = _hash(x, std::move(h));
            }
            return h;
        };

        T _hash(int v, T h) const
        {
            std::cout << "[int]\n";
            const static T INT_TAG = T(129ul);

            union Tmp
            {
                int v;
                char c[4];
            };
            Tmp tmp;
            tmp.v = v;

            h = _hash(INT_TAG, std::move(h));
            h = _hash(tmp.c[0], std::move(h));
            h = _hash(tmp.c[1], std::move(h));
            h = _hash(tmp.c[2], std::move(h));
            h = _hash(tmp.c[3], std::move(h));
            return h;
        };


        T _hash(std::string_view v, T h) const
        {
            std::cout << "[string_view]\n";
            const static T STRING_VIEW_TAG = T(29ul);

            h = _hash(STRING_VIEW_TAG, std::move(h));
            for (auto x : v)
            {
                h = _hash(x, std::move(h));
            }
            return h;
        };

        /*
        template <typename V> // requires Convertible<V,T>
        T _hash(V const & v, T h) const
        {
            const static T FNV_PRIME = T(16777619ul);
            h = std::move(h) ^ static_cast<T>(v);
            h = std::move(h) * FNV_PRIME;
            return h;
        };
        */

        T _hash(char x, T h) const
        {
            const static T FNV_PRIME = T(16777619ul);
            h = std::move(h) ^ static_cast<T>(x);
            h = std::move(h) * FNV_PRIME;
            return h;
        };


    };
}