/**
 * @file Mph.h
 * @author Alex Towell
 * @date March 2014
 *
 * C++ interface for cmph, a C library of (minimal) perfect hash functions.
 */

#pragma once
#include <string_view>
#include <iostream>

namespace alex::hash
{
    class Mph
    {
    public:
        using size_type = size_t;
        using hash_type = uint32_t;
        using value_type = std::string_view;

        friend class MphBuilder;
        friend std::ostream& operator<<(std::ostream&, Mph const &);
        friend std::istream& operator>>(std::istream&, Mph &);

        Mph();
        Mph(Mph const &);
        Mph(Mph &&);
        Mph(char* packed_mphf, size_type size);

        Mph& operator=(Mph const &);
        Mph& operator=(Mph &&);

        std::string_view data();

        size_type byte_size() const;

        hash_type max_hash() const;

        void erase();

        hash_type operator()(value_type key);
        hash_type operator()(char const * key, size_type size);

        ~Mph();

    private:
        char * _packed_mphf;
        size_type _size;
    };
}