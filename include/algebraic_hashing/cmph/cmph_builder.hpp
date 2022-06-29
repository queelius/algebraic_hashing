#pragma once

#include "cmph.hpp"
#include <vector>

namespace cmph
{
  struct cmph_builder
  {
    double load_factor;
    CMPH_ALGO algo;
    cmph_uint32 verbosity;

    cmph_builder() : verbosity(0), algo(CMPH_CHD_PH), load_factor(.81) {}

    auto & verbosity_level(cmph_uint32 level = 0) { verbosity = level; return *this; }
    auto & algo(CMPH_ALGO algo = CMPH_CHD_PH) { this->algo = algo; return *this; }
    auto & load_factor(double r) { load_factor = r; return *this; }

    template <typename I>
    void operator()(I begin, I end)
    {
        std::vector<char*> chars;
        chars.reserve(data.size());

        for (auto i = begin; i != end; ++i)
            chars.push_back(const_cast<char*>(i->c_str()));
        return operator()(&chars[0], std::distance(begin,end));
    }

    auto operator()(char** data, size_t m)
    {
        const auto source = cmph_io_vector_adapter(data, m);
        auto config = cmph_config_new(source);
        cmph_config_set_verbosity(config, verbosity);
        cmph_config_set_graphsize(config, load_factor);
        cmph_config_set_algo(config, algo);
        const auto hash = cmph_new(config);

        auto sz = cmph_packed_size(hash);
        auto buf = new unsigned char[sz];

        cmph_pack(hash, buf);
        cmph_config_destroy(config);
        auto N = cmph_size(hash);

        cmph_io_vector_adapter_destroy(source);
        cmph_destroy(hash);

        return cmph(sz,N,m,buf);
    }
  };
}