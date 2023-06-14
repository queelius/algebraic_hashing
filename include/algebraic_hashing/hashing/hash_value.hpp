#pragma once

#include <iterator>
#include <concepts>
#include <string>
#include <iomanip>
#include <sstream>

namespace algebraic_hashing {

auto to_hex(unsigned int x, int width=0, char fill='0')
{
    std::stringstream s;
    s << std::setw(width) << std::setfill(fill) << std::hex << x;
    return s.str();
}

/**
 * Models the concept of a cryptographic hash value.
 * 
 * It also models the concept of a ring
 *     (hash_value<N>, + = |, * = ^, 0 = hash_value<N>(), 1 = hash_value<N>::ones())
 * such that:
 *     hash_value<N>() ^ h = h
 *     hash_value<N>() & h = hash_value<N>()
 *     hash_value<N>::ones() | h = hash_value<N>::ones()
 *     hash_value<N>::ones() & h = h
 *     hash_value<N>::ones() ^ h = ~h
 *     ~h | h = hash_value<N>::ones()
 *     ~h & h = hash_value<N>()
 *     inv(h) ^ h = hash_value<N>()
 *     h ^ inv(h) = hash_value<N>()
 */

template <size_t N>
struct hash_value
{
    std::array<uint8_t, N> data;

    hash_value() { data.fill(0); }

    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
    constexpr auto size() const { return sizeof(T) * N; }
    auto operator[](size_t index) const { return data[index]; }
    auto & operator[](size_t index) { return data[index]; }

    auto & operator^=(hash_value<N> const & rhs)
    {
        for (size_t i = 0; i < N; ++i)
            data[i] ^= rhs[i];
        return *this;
    }

    // we implement the negation operator as the xor operator.
    // this is because the xor operator is its own inverse.
    auto operator~() const
    {
        hash_value<N> h;
        for (size_t i = 0; i < N; ++i)
            h.data[i] = ~data[i];
        return h;
    }

    // return the hash value as a hexadecimal string.
    auto as_hex() const
    {
        std::string hex;
        // we store hash values as an array of unsigned ints [0,255].
        // we want to return them as hexadecimal values [0,ff],
        // where we store each element as a string of length 2, so that
        // the hash value is a string of length 2*N. 
        for (auto const & b : data)
            hex += to_hex_str(b, 2);
        return hex;
    }
};

template <size_t N>
auto operator!=(hash_value<N> const & l, hash_value<N> const & r) { return l.data != r.data; }

template <size_t N>
auto operator==(hash_value<N> const & l, hash_value<N> const & r) { return l.data == r.data; }

// the inverse of a hash value is itself, since the xor operator is its own inverse.
// this is when we think of hash_value<N> as a group under the xor operator.
template <size_t N>
auto inv(hash_value<N> h) { return h; }

template <size_t N1, size_t N2>
auto concat(hash_value<N1> const & l,
            hash_value<N2> const & r)
{
    hash_value<N1+N2> lr;
    for (size_t i = 0; i < N1; ++i)
        lr[i] = l[i];
    for (size_t i = 0; i < N2; ++i)
        lr[N1+i] = r[i];
    return lr;
}

template <size_t N>
auto operator^(hash_value<N> lhs,
               hash_value<N> const & rhs)
{
    return lhs ^= rhs;
}

} // namespace algebraic_hashing

namespace std {

template <size_t N>
struct hash<algebraic_hashing::hash_value<N>>
{
    size_t operator()(algebraic_hashing::hash_value<N> const & x) const
    {
        uint8_t s = T(0);
        for (size_t i = 0; i < N; ++i)
            s ^= x[i] + T(0x9e3779b9) + (s << T(6)) + (s >> T(2));
        return s;
    }
};

} // namespace std