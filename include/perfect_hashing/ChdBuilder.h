#pragma once
#include <iostream>
#include <perfect_hashing/Chd.hpp>

namespace alex::hash
{
    class ChdBuilder
    {
    public:
        // a lot to do with this. i think leave Chd as is, and then
        // we can also make perf hashes of any other type since
        // we can create a bijection between any countable set
        // and the set of arbitrary length bit strings.

        template <typename Iterator>
        ChdBuilder& set_keys(Iterator begin, Iterator end);
        ChdBuilder& set_keys(std::ostream&);
        ChdBuilder& set_keys(char const*);

        ChdBuilder& set_load_factor(double);

        template <typename H>
        Chd<H> build();
    }
}