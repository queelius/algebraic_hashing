

template <typename T>
concept bool Set = requires(T x)
{
    { x.contains(typename T::value_type) } -> bool;
};

template <typename T>
concept bool Iterable() 
{
    return requires(T x)
    {
        { x.begin() };
        { x.end() };
    } || requires(T x)
    {
        { std::begin(x) };
        { std::end(x) };
    } || requires(T x)
    {
        { begin(x) };
        { end(x) };
    };

};

template <typename T>
concept bool RandomPosASet() 
{
    return requires(T x)
    {
        { x.fpr(); }
    } && Set<T>
};

template <typename T>
concept bool RandomNegASet() 
{
    return requires(T x)
    {
        { x.fnr(); }
    } && Set<T>
};

template <typename T>
concept bool RandomASet = RandomPosASet<T> || RandomNegASet<T>;


template <typename T> requires RandomASet<T> && Iterable<T>
auto make_union(T a, T b)
{
    std::vector<T::value_type> xs(a.begin(), a.end());
    xs.insert(b.begin(), b.end(), xs.end());
    return T(xs.begin(), xs.end(), fpr, fnr);
}