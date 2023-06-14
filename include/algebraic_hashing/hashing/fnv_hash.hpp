#pragma once

#include <climits>
#include <vector>
#include <cstring>
#include <variant>
#include <optional>
#include <tuple>
#include <string>
#include <compare>

namespace algebraic_hashing {

namespace details {
    struct fnv_params
    {
        static size_t const prime = 1099511628211ul;
        static size_t const offset_basis = 14695981039346656037ul;
    };

    auto fnv_hash(char x)
    {
        const size_t TAG =
            102948367061479ul;

        auto h = fnv_params::offset_basis;
        h ^= TAG;
        h *= fnv_params::prime;
        h ^= x;
        h *= fnv_params::prime;
        return h;
    }

    template <typename T>
    auto fnv_hash_helper(T x)
    {
        const size_t TAG =
            3715079492639262977ul;
        
        auto h = fnv_params::offset_basis;
        h ^= TAG;
        h *= fnv_params::prime;

        for (size_t i = 0; i < sizeof(T); ++i)
        {
            h ^= static_cast<char>(x & 0xFF);
            h *= fnv_params::prime;
            x >>= CHAR_BIT;
        }
        return h;
    }

    template <typename T>
    auto fnv_hash(T x)
    {
        return fnv_hash_helper(x);
    }

    auto fnv_hash(char const v[])
    {
        const size_t TAG =
            5187030066587545063ul;

        auto h = fnv_params::offset_basis;
        h ^= TAG;
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
        const size_t TAG =
            16178454387697795603ul;

        auto h = fnv_params::offset_basis;
        h ^= TAG;
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
        const size_t TAG =
            5499849890277433ul;

        auto h = fnv_params::offset_basis;        
        h ^= TAG;
        h *= fnv_params::prime;
        h ^= x;
        h *= fnv_params::prime;
        h ^= fnv_hash_helper(x);
        return h;
    }

    auto fnv_hash(int32_t x)
    {
        const size_t TAG =
            14900455398769288279ul;

        auto h = fnv_params::offset_basis; 
        h ^= TAG;
        h *= fnv_params::prime;
        h ^= x;
        h *= fnv_params::prime;
        h ^= fnv_hash_helper(x);
        return h;
    }

    auto fnv_hash(int64_t x)
    {
        const size_t TAG =
            13479134154285540563ul;

        auto h = fnv_params::offset_basis;        
        h ^= TAG;
        h *= fnv_params::prime;
        h ^= x;
        h *= fnv_params::prime;
        h ^= fnv_hash_helper(x);
        return h;
    }

    auto fnv_hash(uint64_t x)
    {
        const size_t TAG =
            15883342790653477757ul;  

        auto h = fnv_params::offset_basis;     
        h ^= TAG;
        h *= fnv_params::prime;
        h ^= x;
        h *= fnv_params::prime;
        h ^= fnv_hash_helper(x);
        return h;
    }

    template <typename T>
    auto fnv_hash(std::vector<T> const & xs)
    {
        const size_t TAG =
            10890309822534387151ul;
        
        auto h = fnv_params::offset_basis;
        h ^= TAG;
        h *= fnv_params::prime;

        for (auto const & x : xs)
        {
            h ^= fnv_hash(x);
            h *= fnv_params::prime;
        }

        return h;
    }
    
    template <size_t I = 0, typename... V>
    auto fnv_hash(std::tuple<V ...> const & v)
    {
        const size_t TAG =
            15924045725684695769ul;

        auto h = fnv_params::offset_basis;            
        h ^= TAG;
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
        const size_t TAG =
            85145676764564679ul;

        auto h = fnv_params::offset_basis;             
        h ^= TAG;
        h *= fnv_params::prime;

        std::visit([&h](const auto & x)
        {
            h ^= fnv_hash(x);
            h *= fnv_params::prime;
        }, v);

        return h;
    }

    template <typename V>
    auto fnv_hash(std::optional<V> x)
    {
        const size_t TAG =
            1784266876640463619ul;
        
        auto h = fnv_params::offset_basis;
        h ^= TAG;
        h *= fnv_params::prime;

        if (x.has_value())
        {
            h ^= fnv_hash(*x);
            h *= fnv_params::prime;
        }
        return h;
    }
} // namespace details

/**
 * fnv_hash models a non-cryptographic hash function of the type
 * 
 *     Hashable -> size_t.
 */
struct fnv_hash
{
    using hash_type = size_t;

    /**
     * @brief retrieves the maximum hash value
     */
    static auto max() { return std::numeric_limits<size_t>::max(); }

    /**
     * @brief retrieves the minimum hash value
     */
    static auto min() { return std::numeric_limits<size_t>::max(); }

    /**
     * @brief update a hash value with more values
     *
     * @tparam X value type to hash
     * @param h the existing hash
     * @param x the value type to update (mix) h with
     */
    template <typename X>
    static auto mix(size_t h, X x)
    {
        h ^= details::fnv_hash(x);
        h *= details::fnv_params::prime;
        return h;
    }

    template <typename X>
    auto operator()(X const & x) const
    {
        return details::fnv_hash(x);
    }

    auto operator<=>(fnv_hash const &) const = default;
};

constexpr bool is_eq(fnv_hash const &, fnv_hash const &) { return true; }

} // namespace algebraic_hashing
