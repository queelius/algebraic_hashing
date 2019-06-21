#pragma once

#include <variant>
#include <tuple>
#include <optional>

namespace alex::hash::fnv_hash
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
    concept bool HashType = Xorable<T> && Multipliable<T> && fnv_prime<T>::value;
    
    template <typename T, HashType H>
    concept Hashable = requires(T x)
    {
        { fnv_hash<T>{}(x) } -> H;
    };

    template <typename T>
    struct fnv_prime {};

    template <> struct fnv_prime<uint32_t>
    {
         uint32_t const value = 16777619u;
    };

    template <> struct fnv_prime<uint64_t>
    {
         uint64_t const value = 1099511628211;
    };

    template <Iterable I, FnvHashValueType H>
    H fnv_hash(I const & xs) const
    {
        
        static const T ITERABLE_HASH_TAG = 29;

        std::cout << "[iterable]\n";

        h = h * fnv_prime<H>::value;
        h = fnv_hash(ITERABLE_HASH_TAG);
        for (auto const & x : v)
        {
            h = h * fnv_prime<H>::value;
            h = h * fnv_hash(x);
        }
        return h;
    };



    template <typename T>
    T fnv_hash(char const v[])
    {
        std::cout << "[char const[]]\n";
        auto h = fnv_basis<H>::value;
        for (size_t = 0; i < sizeof(v); ++i)
        {
            auto h = fnv_prime<H>::value;
            return h * FNV_PRIME ^ v;
        }
    };


}