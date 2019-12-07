#pragma once

#include <array>
#include <string.h>
#include <cmath>

/**
 * RingMod<N> is integer modulo 2^N.
 * 
 * Most of the properties of integer modulo k
 * are not implemented, only enough to be useful
 * as a set of hash values of arbitrary length
 * with multiplication, addition, and xor.
 * 
 * String literals for constructing common modulo
 * equivalence classes are provided, e.g.,
 *     101_mod16 => RingMod<4>(5).
 * 
 * RingMod<N> is a RegularType that overloads the
 * operator set {+, *, ^, ==, <}.
 */

namespace alex::ringmod
{
    // ringmod<N> is integer modulo 2^N
    template <size_t N>
    struct RingMod
    {
        auto begin() const { return std::begin(digits); };
        auto end() const { return std::end(digits); };
        auto log_mod() { return N; };

        RingMod(unsigned int n = 0);
        RingMod(char const*);

        RingMod(std::array<bool, N> x = {}) : digits(x) {};

        operator unsigned int() const { return convert<unsigned int>(); };
        operator unsigned long long() const { return convert<unsigned long long>(); };
        operator unsigned long() const { return convert<unsigned long>(); };

        template <typename T>
        T convert() const
        {
            T result = T(0);
            for (size_t i = digits.size() - 1; i < digits.size(); --i)
                result = T(2) * result + digits[i];
            return result;
        };


        std::array<bool,N> digits;
    };

    template <size_t N>
    bool odd(RingMod<N> const &);

    template <size_t N1, size_t N2>
    bool operator==(RingMod<N1> const &, RingMod<N2> const &);

    template <size_t N>
    bool operator<(RingMod<N> const &, RingMod<N> const &);

    template <size_t N>
    RingMod<N> operator+(RingMod<N> const &, RingMod<N> const &);

    template <size_t N>
    RingMod<N> operator^(RingMod<N> const &, RingMod<N> const &);

    template <size_t N>
    RingMod<N> operator*(RingMod<N> const &, RingMod<N> const &);

    RingMod<2> operator "" _mod4(const char*);

    RingMod<4> operator "" _mod16(const char*);

    RingMod<10> operator "" _mod10b(const char*);

    RingMod<512> operator "" _mod512b(const char*);
}

// implementation
namespace alex::ringmod
{
    // implementation details. not part of the public interface.
    namespace detail
    {
        // preconditions: x is a binary sequence of 0's or 1's.
        template <size_t N>
        constexpr std::array<bool,N> from_binary(const char* x)
        {
            std::array<bool, N> a{};
            auto n = strlen(x);
            if (n <= N)
            {
                for (size_t i = 0; i < n; ++i)
                {
                    if (x[i] == '\0')
                        break;
                    // assert(x[i] == '0' || x[i] == '1');
                    a[n-i-1] = (bool)(x[i] - '0');
                }
            }
            else
            {
                for (size_t i = 0; i < N; ++i)
                {
                    if (x[i] == '\0')
                        break;
                    // assert(x[i] == '0' || x[i] == '1');
                    a[N-i-1] = (bool)(x[n-N+i] - '0');
                }               
            }
            
            return a;
        };

        template <size_t N>
        constexpr std::array<bool,N> from_int(unsigned long long n)
        {
            std::array<bool,N> a{};
            size_t i = 0;
            while (n != 0ul && i != N)
            {
                a[i] = n % 2;
                ++i;
                n /= 2;
            }
            return a;
        };

        template <size_t N>
        constexpr std::array<bool,N> xor_op(std::array<bool,N> const & a, std::array<bool,N> const & b)
        {
            std::array<bool,N> c{};
            for(size_t i = 0; i < N; ++i)
                c[i] = a[i] ^ b[i];
            return c;
        };

        template <size_t N>
        constexpr std::array<bool,N> add_op(std::array<bool,N> const & a, std::array<bool,N> const & b)
        {
            std::array<bool,N> c{};
            bool carry = false;
            for(size_t i = 0; i < N; ++i)
            {
                if (a[i])
                {
                    if (b[i])
                    {
                        c[i] = carry;
                        carry = true;
                    }
                    else
                        c[i] = !carry;
                }
                else
                {
                    if (b[i])
                        c[i] = !carry;
                    else
                    {
                        c[i] = carry;
                        carry = false;
                    }
                }
            }
            return c;
        };

        template <size_t N>
        constexpr std::array<bool,N> multiply_op(std::array<bool,N> const & a, std::array<bool,N> const & b)
        {
            std::array<bool,N> c{};
            for(size_t i = 0; i < N; ++i)
            {
                std::array<bool,N> k{};
                for(size_t j = 0; j < N-i; ++j)
                    k[j+i] = a[i] && b[j];
                c = std::move(add_op(c, k));
            }
            return c;
        };
    }

    template <size_t N>
    RingMod<N>::RingMod(unsigned int n) : digits(detail::from_int<N>(n))
    {
        // do nothing else
    }

    template <size_t N>
    RingMod<N>::RingMod(char const* x) : digits(detail::from_binary<N>(x))
    {
        // do nothing else
    }

    template <size_t N>
    bool odd(RingMod<N> const & a)
    {
        return a[0];
    }

    template <size_t N1, size_t N2>
    bool operator==(RingMod<N1> const & a, RingMod<N2> const & b)
    {
        return a.digits == b.digits;
    }

    template <size_t N>
    bool operator<(RingMod<N> const & a, RingMod<N> const & b)
    {
        for (size_t i=N-1; i < N; --i)
        {
            if (a.digits[i] != b.digits[i])
                return b.digits[i];
        }
        return false;
    }

    template <size_t N>
    RingMod<N> operator+(RingMod<N> const & a, RingMod<N> const & b)
    {
        return RingMod<N>(detail::add_op(a.digits, b.digits));
    }

    template <size_t N>
    RingMod<N> operator^(RingMod<N> const & a, RingMod<N> const & b)
    {
        return RingMod<N>(detail::xor_op(a.digits, b.digits));
    }

    template <size_t N>
    RingMod<N> operator*(RingMod<N> const & a, RingMod<N> const & b)
    {
        return RingMod<N>(detail::multiply_op(a.digits, b.digits));
    }

    RingMod<2> operator "" _mod4(const char* x)
    {
        return RingMod<2>(x);
    }

    RingMod<4> operator "" _mod16(const char* x)
    {
        return RingMod<4>(x);
    }

    RingMod<10> operator "" _mod10b(const char* x)
    {
        return RingMod<10>(x);
    }

    RingMod<512> operator "" _mod512b(const char* x)
    {
        return RingMod<512>(x);
    }
}
