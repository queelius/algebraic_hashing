#pragma once

#include "fnv_hash.hpp"

namespace alex::hash
{
    /**
     * HashIndex models an indexed family of hash functions X -> hash_value_type
     * where X depends on Hash.
     * 
     * For some type F to model an indexed family of hash functions, there must be an
     * overload of the function call method that maps indexes to hash functions,
     *     F : index_type -> Hash.
     * 
     * An instance of Hash is a hash function X -> hash_value_type that can be
     * constructed from an index_type.
     */
    template <typename Hash = FnvHash<unsigned int>>
    class HashIndex
    {
    public:
        using hash_type = Hash;
        using hash_value_type = typename Hash::hash_value_type;
        using index_type = unsigned int;

        // TODO: create an iterator such that every hash function in the index
        // may be iterated over.

        hash_type operator()(index_type index) const
        {
            return Hash(index);
        };
    };
}
