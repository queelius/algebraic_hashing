#pragma once

#include <stdio.h>
#include <cmph/cmph.h>
#include <perfect_hashing/Mph.h>
#include <istream>

namespace alex::hash
{
    class MphBuilder
    {
    public:
        MphBuilder& set_key_source(FILE* keyfile, size_t nkeys = 0);
        //MphBuilder& set_key_source(std::istream& keystream, size_t nkeys = 0);
        MphBuilder& set_key_source(unsigned char** keys, size_t nkeys = 0);

        //template <typename Iterator>
        //MphBuilder& set_key_source(Iterator begin, Iterator end);

        MphBuilder& set_verbose(bool on = true);
        MphBuilder& set_load_factor(double load);
        MphBuilder& set_algorithm(CMPH_ALGO algo);

        Mph build();

    private:
        CMPH_ALGO _algo;
        bool _verbose;
        double _load;
        cmph_io_adapter_t* _keysource;
    };
}