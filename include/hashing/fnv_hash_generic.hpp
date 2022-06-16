/**
 * template <typename H>
 * fnv_hash<H> models a non-cryptographic hash function of the type
 *
 *     Hashable -> H
 *
 * where H has overloads for
 *
 *     ^= : (H&,H) -> H
 * and
 *     *= : (H*,H) -> H.
 *
 * Only two operations are required, which means a barebones
 * concrete type that satisfies the concept may be constructed.
 * In the simplest case, we let H be a primitive type like uint64_t,
 * which is pretty effective since it has 2^64 possibilities.
 * However, if fnv_hash is shown to have poor distributional
 * properties, then a larger type, say mod<N> where the number
 * of possibilities are 2^N, may be preferrable, e.g., mod<1024>
 * has 2^1024 possibilities, and its bit length is 1024.
 *
 * The probability of a collision is now much lesser since
 * there are more hash values that can be mapped to.
 */

#pragma once

#include <limits.h>
#include <vector>
#include <cstring>
#include <variant>
#include <optional>
#include <tuple>
#include <string>
namespace hashing::generic
{
    template <typename H>
    struct fnv_params {};

    template <>
    struct fnv_params<uint64_t>
    {
        static uint64_t const prime = 1099511628211ul;
        static uint64_t const offset_basis = 14695981039346656037ul;
    };

    namespace details
    {
        template <typename H>
        auto fnv_hash(char x)
        {
            auto h = fnv_params<H>::offset_basis;
            h ^= x;
            h *= fnv_params<H>::prime;
            return h;
        }

        template <typename H, typename T>
        auto fnv_hash_helper(T x, H h)
        {
            for (size_t i = 0; i < sizeof(T); ++i)
            {
                h ^= static_cast<char>(x & 0xFF);
                h *= fnv_params<H>::prime;
                x >>= CHAR_BIT;
            }
            return h;
        }

        template <typename H>
        auto fnv_hash(char const v[])
        {
            static const char CSTRING_TAG = 7;

            auto h = fnv_params<H>::offset_basis;
            h ^= CSTRING_TAG;
            h *= fnv_params<H>::prime;

            for (size_t i = 0; i < strlen(v); ++i)
            {
                h ^= v[i];
                h *= fnv_params<H>::prime;
            }
            return h;
        }

        template <typename H, iterable X>
        auto fnv_hash(X const & xs)
        {
            static const char X_TAG = 111;
            auto h = fnv_params<H>::offset_basis;
            h ^= X_TAG;
            h *= fnv_params<H>::prime;

            for (auto x : xs)
            {
                h ^= x;
                h *= fnv_params<H>::prime;
            }
            return h;
        }

        template <typename H>
        auto fnv_hash(std::string const & xs)
        {
            static const char STRING_TAG = 101;

            auto h = fnv_params<H>::offset_basis;
            h ^= STRING_TAG;
            h *= fnv_params<H>::prime;

            for (auto x : xs)
            {
                h ^= x;
                h *= fnv_params<H>::prime;
            }
            return h;
        }

       template <typename H>
       auto fnv_hash(uint32_t x)
        {
            static const char UINT32_TAG = 5;

            auto h = fnv_params<H>::offset_basis;
            h ^= UINT32_TAG;
            h *= fnv_params<H>::prime;
            return fnv_hash_helper(x,h);
        }

        template <typename H>
        auto fnv_hash(int32_t x)
        {
            static const char INT32_TAG = 3;

            auto h = fnv_params<H>::offset_basis;
            h ^= INT32_TAG;
            h *= fnv_params<H>::prime;
            return fnv_hash_helper(x,h);
        }

        template <typename H>
        auto fnv_hash(int64_t x)
        {
            static const char INT64_TAG = 17;

            auto h = fnv_params<H>::offset_basis;
            h ^= INT64_TAG;
            h *= fnv_params<H>::prime;
            return fnv_hash_helper(x,h);
        }

        template <typename H>
        auto fnv_hash(uint64_t x)
        {
            static const char UINT64_TAG = 13;

            auto h = fnv_params<H>::offset_basis;
            h ^= UINT64_TAG;
            h *= fnv_params<H>::prime;
            return fnv_hash_helper(x,h);
        }

        template <typename H, typename T>
        auto fnv_hash(std::vector<T> const & xs)
        {
            const static char VECTOR_TAG = 29;

            auto h = fnv_params<H>::offset_basis;
            h ^= VECTOR_TAG;
            h *= fnv_params<H>::prime;

            for (auto const & x : xs)
            {
                h ^= fnv_hash<H>(x);
                h *= fnv_params<H>::prime;
            }

            return h;
        }

        template <typename H, size_t I = 0, typename... V>
        auto fnv_hash(std::tuple<V ...> const & v)
        {
            static const char TUPLE_TAG = 31;

            auto h = fnv_params<H>::offset_basis;
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
        }

        template <typename H, typename... V>
        auto fnv_hash(std::variant<V ...> const & v)
        {
            static const char VARIANT_TAG = 119;

            auto h = fnv_params<H>::offset_basis;
            h ^= VARIANT_TAG;
            h *= fnv_params<H>::prime;

            std::visit([&h](const auto& x)
            {
                h ^= fnv_hash<H>(x);
                h *= fnv_params<H>::prime;
            }, v);

            return h;
        }

        template <typename H, typename V>
        auto fnv_hash(std::optional<V> x)
        {
            const static size_t OPTIONAL_TAG = 29;

            auto h = fnv_params<H>::offset_basis;
            h ^= fnv_hash<H>(OPTIONAL_TAG);
            h *= fnv_params<H>::prime;

            if (x.has_value())
            {
                h ^= fnv_hash<H>(*x);
                h *= fnv_params<H>::prime;
            }
            return h;
        }
    }

    template <typename H>
    struct fnv_hash
    {
        using hash_type = size_t;

        template <typename X>
        auto operator()(X const & x) const
        {
            return details::fnv_hash<H>(x);
        }
    };
}
