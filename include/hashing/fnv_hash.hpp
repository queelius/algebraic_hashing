/**
 * fnv_hash models a non-cryptographic hash function of the type
 * 
 *     Hashable -> size_t.
 */

#pragma once

#include <limits.h>
#include <vector>
#include <cstring>
#include <variant>
#include <optional>
#include <tuple>
#include <string>
namespace hashing
{
    namespace details
    {
        struct fnv_params
        {
            static size_t const prime = 1099511628211ul;
            static size_t const offset_basis = 14695981039346656037ul;
        };

        auto fnv_hash(char x)
        {
            auto h = fnv_params::offset_basis;
            h ^= x;
            h *= fnv_params::prime;
            return h;
        }

        template <typename T>
        auto fnv_hash_helper(T x, size_t h)
        {
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

            auto h = fnv_params::offset_basis;
            h ^= CSTRING_TAG;
            h *= fnv_params::prime;

            for (size_t i = 0; i < strlen(v); ++i)
            {
                h ^= v[i];
                h *= fnv_params::prime;
            }
            return h;
        }

        auto fnv_hash(std::string const & x)
        {
            static const char STRING_TAG = 101;

            auto h = fnv_params::offset_basis;
            h ^= STRING_TAG;
            h *= fnv_params::prime;           

            for (size_t i = 0; i < x.size(); ++i)
            {
                h ^= x[i];
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
            return fnv_hash_helper(x,h);
        }

        auto fnv_hash(int32_t x)
        {
            static const char INT32_TAG = 3;

            auto h = fnv_params::offset_basis;        
            h ^= INT32_TAG;
            h *= fnv_params::prime;
            return fnv_hash_helper(x,h);
        }

        auto fnv_hash(int64_t x)
        {
            static const char INT64_TAG = 17;

            auto h = fnv_params::offset_basis;        
            h ^= INT64_TAG;
            h *= fnv_params::prime;
            return fnv_hash_helper(x,h);
        }

        auto fnv_hash(uint64_t x)
        {
            static const char UINT64_TAG = 13;

            auto h = fnv_params::offset_basis;        
            h ^= UINT64_TAG;
            h *= fnv_params::prime;
            return fnv_hash_helper(x,h);
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
            h ^= fnv_hash(VARIANT_TAG);
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

    }    
    
    struct fnv_hash
    {
        using hash_type = size_t;

        template <typename X>
        auto operator()(X const & x) const
        {
            return details::fnv_hash(x);
        }
    };
}