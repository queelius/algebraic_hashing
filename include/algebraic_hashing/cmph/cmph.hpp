/**
 * @file CMPH.hpp
 * @author Alex Towell
 * @date March 2014
 *
 * C++ interface for cmph, a C library for constructing a family of perfect hash
 * functions.
 */

#pragma once

#include <string_view>
#include <cstring>
#include "c_interface/cmph.h"

namespace cmph
{
    class cmph
    {
    public:
        cmph() : buf(nullptr), sz(0), m(0), N(0) {};

        cmph(int sz, int N, int m, unsigned char * buf) :
            sz(sz),
            N(N),
            m(m),
            buf(buf)
        {
        }

        cmph(cmph const & copy) :
            sz(copy.sz),
            N(copy.N),
            m(copy.m),
            buf(new unsigned char[copy.sz])
        {
            std::memcpy(buf,copy.buf,copy.sz);
        }

        cmph(cmph&& rhs) :
            sz(rhs.sz),
            N(rhs.N),
            m(rhs.m),
            buf(rhs.buf)
        {
            rhs.sz = rhs.N = rhs.m = 0;
            rhs.buf = nullptr;
        }

        cmph & operator=(cmph const & rhs)
        {
            if (this != &rhs)
            {
                clear();
                sz = rhs.sz;
                m = rhs.m;
                N = rhs.N;
                buf = new unsigned char[sz];
                std::memcpy(buf, rhs.buf, rhs.sz);
            }
            return *this;
        }

        cmph & operator=(cmph && rhs)
        {
            if (this != &rhs)
            {
                clear();
                sz = rhs.sz;
                buf = rhs.buf;
                N = rhs.N;
                m = rhs.m;
                rhs.buf = nullptr;
                rhs.sz = m = N = 0;
            }

            return *this;
        }
        
        void clear()
        {
            if (buf != nullptr)
            {
                delete [] buf;
                buf = nullptr;
                sz = m = N = 0;
            }        
        }

        auto find(std::string_view x) const
        {
            return cmph_search_packed(buf, x.data(), x.size());
        }

        auto max_hash() const { return N; }
        auto min_hash() const { return 0; }
        auto size() const { return m; }
        virtual ~cmph() { clear(); }
        auto load_factor() const { return (double)m / N; }

    private:
        unsigned char * buf;
        int sz, N, m;
    };
}

