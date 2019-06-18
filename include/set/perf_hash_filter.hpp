

namespace alex::set
{
    template <
        typename ValueType,
        template <typename> typename PerfHash,
        typename <typename> typename Hash,
        typename Interval>
    class RandomApproximateSet
    {
    public:
        template <typename I>
        RandomApproximateSet(
            I begin,
            I end,
            Interval fpr,
            Interval fnr,
            double r)
        {

        };

    private:
        Interval fpr, fnr;
        PerfHash<ValueType> _ph;
        Hash<ValueType> _h;
    };
}