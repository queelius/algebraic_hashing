#pragma once

#include <iterator>
#include <concepts>
#include <string>

namespace algebraic_hashing::hashing
{
    template <std::integral T, size_t N>
    struct hash_value
    {
        std::array<T,N> data;

        auto begin() const { return data.begin(); }
        auto end() const { return data.end(); }
        constexpr auto size() const { return sizeof(T) * N; }
        auto operator[](size_t index) const { return data[index]; }
        auto & operator[](size_t index) { return data[index]; }

        auto & operator^=(hash_value<T,N> const & rhs)
        {
            for (size_t i = 0; i < N; ++i)
                data[i] ^= rhs[i];
            return *this;
        }

        operator std::string() const
        {
            return std::string(static_cast<char*>(data.data()));
        }
    };

    template<typename T, size_t N1, size_t N2>
    auto concat(hash_value<T,N1> const & l,
                hash_value<T,N2> const & r)
    {
        hash_value<T,N1+N2> lr;
        for (size_t i = 0; i < N1; ++i)
            lr[i] = l[i];
        for (size_t i = 0; i < N2; ++i)
            lr[N1+i] = r[i];
        return lr;
    }

    template <typename T, size_t N>
    auto operator^(hash_value<T,N> lhs,
                   hash_value<T,N> const & rhs)
    {
        return lhs ^= rhs;
    }
}

namespace std
{
    template <typename T, size_t N>
    struct hash<hashing::hash_value<T,N>>
    {
        size_t operator()(hashing::hash_value<T,N> const & x) const
        {
            T s = T(0);
            for (size_t i = 0; i < N; ++i)
                s ^= x[i] + T(0x9e3779b9) + (s << T(6)) + (s >> T(2));
            return s;
        }
    };
}