

namespace alex::set
{
    template <typename X>
    struct Identity
    {
        X operator()(X x) { return x };
    };

    // Hash : Hashable -> HashValueType
    // PerfHash : Hashable -> [0,...,N-1]
    // Coder : ValueType -> Hashable
    // Default: Coder is identity for ValueType
    //          which will work if ValueType is a
    //          Hashable type.

    /**
     * If Coder is not injective, then two or more elements
     * of ValueType map to the same element in HashValueType.
     * These creates an equivelance relation, i.e., if
     * Coder(x) = Coder(y) then x equiv y. This could potentially
     * reduce the bit length of the PerfHash object since
     * given a set S to perfectly hash, it only needs...
     * 
     * If accidental, then increases fpr rate.
     * 
     * template parameter for fpr through N?
     */
    template <
        typename ValueType,
        typename PerfHash,
        typename Hash,
        typename Interval,
        typename Coder = Identity<ValueType>>
    class PerfectHashFilter
    {
    public:
        template <typename I>
        PerfectHashFilter(
            I begin,
            I end,
            Interval fpr,
            Interval fnr,
            double r)
        {

        };

        bool contains(ValueType x) const
        {
            auto code = Encoder(x);
            return _hashes[_ph(code)] == _h(code) % _N;
        };

    private:
        Interval fpr, fnr;
        PerfHash _ph;
        Hash _h;
        typename Hash::hash_type _N;

        // this should be a packed container type
        // that (assuming uniform distribution)
        // [0,N-1] can be compactly stored.
        std::vector<typename Hash::hash_type> _hashes;
    };
}