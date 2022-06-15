/**
 * fnv_hash models a non-cryptographic hash function of the type
 * 
 *     Hashable -> size_t.
 */

#pragma once

#include <variant>
#include <tuple>
#include <optional>
#include <limits.h>
#include <vector>

namespace hashing
{
    struct fnv_params
    {
         size_t const prime = 1099511628211;
         size_t const offset_basis = 14695981039346656037;
    };

    struct fnv_hash
    {
        auto operator()(T const & x) const
        {
            details::fnv_hash(x);
        }
    };

    namespace details
    {
        template <typename T>
        auto fnv_hash(T x)
        {
            auto h = fnv_params::offset_basis;
            h ^= fnv_params::prime;
            
            for (size_t i = 0; i < sizeof(T); ++i)
            {
                h ^= static_cast<char>(x & 0xFF);
                h *= fnv_params::prime;
                x >>= CHAR_BIT;
            }

            return h;
        }

        auto fnv_hash(char const v[])
        {
            static const char CSTRING_TAG = 7;

            H h = fnv_params::offset_basis;
            h ^= CSTRING_TAG;
            h *= fnv_params::prime;

            for (size_t = 0; i < sizeof(v); ++i)
            {
                h ^= v[i];
                h *= fnv_params::prime;
            }

            return h;
        }

        auto fnv_hash(uint32_t x)
        {
            static const char UINT32_TAG = 5;

            auto h = fnv_params::offset_basis;        
            h ^= UINT32_TAG;
            h *= fnv_params::prime;

            return h ^ fnv_hash(x);
        }

        auto fnv_hash(int32_t x)
        {
            static const char INT32_TAG = 3;

            auto h = fnv_params::offset_basis;        
            h ^= INT32_TAG;
            h *= fnv_params<H>::prime;

            return h ^ fnv_hash(x);
        }

        auto fnv_hash(int64_t x)
        {
            static const char UINT64_TAG = 17;

            auto h = fnv_params::offset_basis;        
            h ^= INT64_TAG;
            h *= fnv_params::prime;

            return h ^ fnv_hash(x);
        }

        auto fnv_hash(uint64_t x)
        {
            static const char UINT64_TAG = 13;

            auto h = fnv_params::offset_basis;        
            h ^= UINT64_TAG;
            h *= fnv_params::prime;

            return h ^ fnv_hash(x);
        }
        
        auto fnv_hash(char x)
        {
            static const char CHAR_TAG = 37;

            auto h = fnv_params::offset_basis;
            h ^= CHAR_TAG;        
            h *= fnv_params::prime;
            h ^= x;
            h *= fnv_params::prime;

            return h;
        };

        template <size_t I = 0, typename... V>
        auto fnv_hash(std::tuple<V ...> const & v)
        {
            static const char TUPLE_TAG = 31;

            auto h = fnv_params::offset_basis;            
            h ^= TUPLE_TAG;
            h *= fnv_params::prime;

            h ^= fnv_hash(std::get<I>(v));
            h *= fnv_params::prime;

            if constexpr(I+1 != sizeof...(V))
            {
                h ^= fnv_hash<I+1>(v);
                h *= fnv_params::prime;
            }

            return h;
        }

        template <typename... V>
        auto fnv_hash(std::variant<V ...> const & v)
        {
            static const char VARIANT_TAG = 119;

            auto h = fnv_params::offset_basis;             
            h ^= fnv_hash<H>(VARIANT_TAG);
            h *= fnv_params::prime;

            std::visit([&h](const auto& x)
            {
                h ^= fnv_hash(x);
                h *= fnv_params::prime;
            }, v);

            return h;
        }

        template <typename V>
        auto fnv_hash(std::optional<V> x)
        {
            const static char OPTIONAL_TAG = 29;
            
            auto h = fnv_params::offset_basis;
            h ^= fnv_hash(OPTIONAL_TAG);
            h *= fnv_params::prime;

            if (x.has_value())
            {
                h ^= fnv_hash(*x);
                h *= fnv_params::prime;
            }

            return h;
        }

        template <typename T>
        auto fnv_hash(std::vector<T> const & xs)
        {
            const static char VECTOR_TAG = 29;
            
            auto h = fnv_params::offset_basis;
            h ^= fnv_hash(VECTOR_TAG);
            h *= fnv_params::prime;

            for (auto const & x : xs)
            {
                h ^= x;
                h *= fnv_params::prime;
            }

            return h;
        }
    }
}