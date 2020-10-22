/**
 * hash<H> models a random (non-cryptographic) hash function of the type
 *     Hashable -> H
 * where Hashable is any type defined for fnv_hash and H is a regular type that
 * models a hash value type which supports xor assignment (^=),
 * multiplication assignment (*=), and can be constructed from a char type.
 */

#pragma once

#include <variant>
#include <tuple>
#include <optional>
#include <limits.h>
#include <vector>

namespace alex::hash::fnv_hash
{
    /**
     * hash_type fnv_hash(uint32_t d, std::string_view s)
     * {
     *     static const hash_type FNV_PRIME = 16777619;
     *     if (d == 0)
     *         d = FNV_PRIME;
     *
     *     for (auto c : s)
     *         d = (d * FNV_PRIME) ^ c;
     *
     *     return d;
     * }
     */

    template <typename T>
    struct fnv_params {};

    template <>
    struct fnv_params<uint32_t>
    {
         uint32_t const prime = 16777619u;
         uint32_t const offset_basis = 2166136261;
    };

    template <>
    struct fnv_params<uint64_t>
    {
         uint64_t const prime = 1099511628211;
         uint64_t const offset_basis = 14695981039346656037;
    };

    template <typename H, typename T>
    H fnv_hash_helper(T x, H h)
    {
        for (size_t i = 0; i < sizeof(T); ++i)
        {
            h ^= static_cast<char>(x & 0xFF);
            h *= fnv_params<H>::prime;
            x >>= CHAR_BIT;
        }

        return h;
    };

    template <typename H>
    H fnv_hash(char const v[])
    {
        static const char CSTRING_TAG = 7;

        H h = fnv_params<H>::offset_basis;
        h ^= CSTRING_TAG;
        h *= fnv_params<H>::prime;

        for (size_t = 0; i < sizeof(v); ++i)
        {
            h ^= v[i];
            h *= fnv_params<H>::prime;
        }

        return h;
    };

    template <typename H>
    H fnv_hash(uint32_t x)
    {
        static const char UINT32_TAG = 5;

        H h = fnv_params<H>::offset_basis;        
        h ^= UINT32_TAG;
        h *= fnv_params<H>::prime;

        return details::fnv_hash_helper(x, h);
    };

    template <typename H>
    H fnv_hash(int32_t x)
    {
        static const char INT32_TAG = 3;

        H h = fnv_params<H>::offset_basis;        
        h ^= INT32_TAG;
        h *= fnv_params<H>::prime;

        return details::fnv_hash_helper(x, h);
    };

    template <typename H>
    H fnv_hash(int64_t x)
    {
        static const char UINT64_TAG = 17;

        H h = fnv_params<H>::offset_basis;        
        h ^= INT64_TAG;
        h *= fnv_params<H>::prime;

        return details::fnv_hash_helper(x, h);
    };

    template <typename H>
    H fnv_hash(uint64_t x)
    {
        static const char UINT64_TAG = 13;

        H h = fnv_params<H>::offset_basis;        
        h ^= UINT64_TAG;
        h *= fnv_params<H>::prime;

        return details::fnv_hash_helper(x, h);
    };
    
    template <typename H>
    H fnv_hash(char x)
    {
        static const char CHAR_TAG = 37;

        H h = fnv_params<H>::offset_basis;
        h ^= CHAR_TAG;        
        h *= fnv_params<H>::prime;
        h ^= x;
        h *= fnv_params<H>::prime;

        return h;
    };

    template <typename H, size_t I = 0, typename... V>
    H fnv_hash(std::tuple<V ...> const & v)
    {
        static const char TUPLE_TAG = 31;

        H h = fnv_params<H>::offset_basis;            
        h ^= TUPLE_TAG;
        h *= fnv_params<H>::prime;

        h ^= fnv_hash<H>(std::get<I>(v));
        h *= fnv_params<H>::prime;

        if constexpr(I+1 != sizeof...(V))
        {
            h ^= fnv_hash<H,I+1>(v);
            h *= fnv_params<H>::prime;
        }

        return h;
    };

    template <typename H, typename... V>
    H fnv_hash(std::variant<V ...> const & v)
    {
        static const char VARIANT_TAG = 119;

        h = fnv_params<H>::offset_basis;             
        h ^= fnv_hash<H>(VARIANT_TAG);
        h *= fnv_params<H>::prime;

        std::visit([&h](const auto& x)
        {
            h ^= fnv_hash<H>(x);
            h *= fnv_params<H>::prime;
        }, v);

        return h;
    };

    template <typename H, typename V>
    H fnv_hash(std::optional<V> x)
    {
        const static char OPTIONAL_TAG = 29;
        
        h = fnv_params<H>::offset_basis;
        h ^= fnv_hash<H>(OPTIONAL_TAG);
        h *= fnv_params<H>::prime;

        if (x.has_value())
        {
            h ^= fnv_hash(*x);
            h *= fnv_params<H>::prime;
        }

        return h;
    };

    template <typename H, typename T>
    H fnv_hash(std::vector<T> const & xs)
    {
        const static char VECTOR_TAG = 29;
        
        h = fnv_params<H>::offset_basis;
        h ^= fnv_hash<H>(VECTOR_TAG);
        h *= fnv_params<H>::prime;

        for (auto const & x : xs)
        {
            h ^= x;
            h *= fnv_params<H>::prime;
        }

        return h;
    };

    template <typename X, typename Y>
    struct default_hash_fn
    {
        using codomain = Y;
        using domain = X;

        Y operator()(X const & x) const
        {
            return fnv_hash<Y>(x);
        };
    };
}