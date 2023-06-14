#pragma once

#include <string>
#include <functional>
#include <memory>

namespace algebraic_hashing {

/**
 * A cryptographic hash value models a particular output from a cryptographic
 * hash function.
 */
class cryptographic_hash
{
public:
    cryptographic_hash(cryptographic_hash const & copy) :
        concept_(copy.concept_->clone()) {}

    template <typename H>
    cryptographic_hash(H const & h) :
        concept_(new hash_model<H>{h}) {}

    //auto hexdigest() const { return concept_->hexdigest(); }
    //auto begin() const { return concept_->begin(); }
    //auto end() const { return concept_->begin(); }
    auto size() const { return concept_->size(); }
    auto operator[](size_t index) const { return concept_->get(index); }
    auto fast_hash() const { return concept_->fast_hash(); }

private:
    struct hash_concept
    {
        virtual hash_concept * clone() const = 0;
        virtual size_t size() const = 0;
        virtual char get(size_t) const = 0;
        //virtual const_forward_iterator begin() const = 0;
        //virtual const_forward_iterator end() const = 0;
        virtual size_t fast_hash() const = 0;
    };

    std::unique_ptr<hash_concept> concept_;

    template <typename H>
    struct hash_model final : hash_concept
    {
        H h;

        model* clone() const  override { return new H(h); }
        //const_forward_iterator begin() const override { return const_forward_iterator(begin(h)); }        
        //const_forward_iterator end() const override  { return const_forward_iterator(end(h)); }
        char get(size_t index) const  override { return h[index]; }
        size_t size() const override { return size(h); }
        size_t fast_hash() const override { return std::hash<H>{}(h); }
    };
};

} // namespace algebraic_hashing

namespace std {
template <>
struct hash<algebraic_hashing::cryptographic_hash>
{
    size_t operator()(algebraic_hashing::cryptographic_hash const & h) const
    {
        return h.fast_hash();
    }
};

} // namespace std
