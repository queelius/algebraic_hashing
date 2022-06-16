Perfect hash functions
======================

Perfect hash functions may be used for memory efficient storage and
fast retrieval of items from static sets, among other uses. 

Description
-----------

We implement two data structures that model the concept of perfect hash
functions.
We denote them respectively by `phf` and `phf_lvl2`.
For small sets, `phf` is more memory efficient and provides faster queries.
However, construction can be very slow for larger sets.
`phf_lvl2` provides much faster constructions at the
cost of slightly slower queries and more in-place memory requirements.
However, it is still quite fast and memory-efficient.

Memory efficiency
-----------------

More gains in memory efficiency are readily possible by using in-place
compression, e.g., in `phf`, the `l` value can be easily compressed since
it should be geometric with a known `p` value. In this case, geometric
coding may be used. However, since we are only storing a single value,
there does not seem to be much point in doing so.

It makes more sense to pursue in-place compression for `phf_lvl2`, which stores
a vector of integers.

