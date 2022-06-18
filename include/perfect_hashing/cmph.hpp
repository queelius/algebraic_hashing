/**
 * @file CMPH.hpp
 * @author Alex Towell
 * @date March 2014
 *
 * C++ interface for cmph, a C library for constructing a family of perfect hash functions.
 */

#pragma once

#include <cstdint>
#include <string>
#include "cmph/cmph.h"

namespace cmph
{
    class CMPH
    {
    public:
        CMPH() : _buffer(nullptr), _size(0) {};

        CMPH(CMPH const & copy) :
            _size(copy._size),
            _buffer(new unsigned char[copy._size])
        {
            ::std::memcpy(_buffer, copy._buffer, copy._size);
        }

        CMPH(CMPH&& src) :
            _size(src._size),
            _buffer(src._buffer)
        {
            src._size = 0;
            src._buffer = nullptr;
        }

        CMPH(char** data, size_t size, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = false) :
            _size(0),
            _buffer(nullptr)
        {
            build(data, size, load_factor, algo, verbose);
        }

        template <typename I>
        CMPH(I begin, I end, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = false) :
            _size(0),
            _buffer(nullptr)
        {
            build(begin,end,load_factor,algo,verbose);
        }

        CMPH & operator=(CMPH const & rhs)
        {
            if (this != &other)
            {
                clear();
                _size = other._size;
                _buffer = new unsigned char[_size];
                std::memcpy(_buffer, other._buffer, other._size);
            }

            return *this;
        }

        CMPH & operator=(CMPH && rhs)
        {
            if (this != &rhs)
            {
                clear();

                _size = rhs._size;
                _buffer = rhs._buffer;
                rhs._buffer = nullptr;
                rhs._size = 0;
            }

            return *this;
        }

        template <typename I>
        void build(I begin, I end, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = true)
        {
            std::vector<char*> chars;
            chars.reserve(data.size());

            for (auto i = begin; i != end; ++i)
                chars.push_back(const_cast<char*>(i->c_str()));
            return build(&chars[0], std::distance(begin,end), load_factor, algo, verbose);
        }

        void build(char** data, size_t size, double load_factor = 0.9, CMPH_ALGO algo = CMPH_CHD_PH, bool verbose = true)
        {
            const auto source = cmph_io_vector_adapter(data, size);
            auto config = cmph_config_new(source);
            cmph_config_set_verbosity(config, verbose);
            cmph_config_set_graphsize(config, load_factor);
            cmph_config_set_algo(config, algo);
            const auto hash = cmph_new(config);

            clear();
            _keys = size;
            _size = cmph_packed_size(hash);
            _buffer = new unsigned char[_size];

            cmph_pack(hash, _buffer);
            cmph_config_destroy(config);
            _max_hash = cmph_size(hash);

            cmph_io_vector_adapter_destroy(source);
            cmph_destroy(hash);
        }
        
        void clear()
        {
            if (_buffer != nullptr)
            {
                delete [] _buffer;
                _buffer = nullptr;
                _size = 0;
                _keys = 0;
            }        
        };


        auto find(const std::string& key) const
        {
            return cmph_search_packed(_buffer, key.c_str(), key.size());
        }

        auto find(const char* key, unsigned int len) const
        {
            return cmph_search_packed(_buffer, key, len);
        }

        auto max_hash() const { return _max_hash; }
        void* buffer() { return (void*)_buffer; }
        auto size() const { return _keys; }
        virtual ~CMPH() { clear(); }
        auto load_factor() const { return (double)_keys / _max_hash; }

    private:
        unsigned char* _buffer;
        uint32_t _size, _max_hash, _keys;
    };
}

