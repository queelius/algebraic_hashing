Packed containers {#mainpage}
=================
A packed container of type `X`, denoted by `P(X)`, *packs* value types of type
`X` into a container, where each value is an object with arbitrary bit alignment
such that no bits are wasted within the container.

The way the data values of type `X` are packed into the container is handled by
a codec, an object type that implements the concept of a [Codec](CODEC_CONCEPT.md).

Typically, the value type `X` has other "native" object types which are byte
aligned and thus can have a machine
, e.g., in C++ we have primitive object types like `int` and `double`,
and compound object types like a class that composes multiple f

A data structure `P(X)` implements a packed container of type `X` if the
following conditions are satisfied:

1. `P(X)` is a [RegularType](regular_type).
2. `P(X)` has an [Iterator](iterator) that permits element traversal.
3. `P(X)` has accessors that allows retrieving a *packed element* (i.e., it has
    not been decoded to an object of)



A fixed-width packed array of type `X` is a packed array of type `X` where all
the values of type `X` have codes that are the same width. A general approach
to allowing these elements to be indexable in constant time (O(1) random access)
is to make each index map to a row in a bit matrix of fixed width, where each
row stores the code for the value.

Index | Element | Encoding
----- | ------- | ---------
`0`   | `x_1`   | `{0,1}^k`
`1`   | `x_2`   | `{0,1}^k`
`2`   | `x_3`   | `{0,1}^k`

Read more about packed containers [here](packed_containers.pdf).

## Variable-length packed array

A variable-length packed array of type `X` is a packed array where elements
of type `X` have arbitirary-length encodings. A general approach to allowing
these elements to be indexable in constant time (O(1) random access) is to
make each index map to a row in a bit matrix of fixed width, where each row
is a pointer to a bit string.

If constant write times are not necessary, then the pointer may be into a
large contiguous range of bits such that no space is wasted on alignment issues.

Index | Element | Bit Address | Encoding
----- | ------- | ----------- | --------
0     | x1      | {0,1}^k     | {0,1}^n1
1     | x2      | {0,1}^k     | {0,1}^n2
2     | x3      | {0,1}^k     | {0,1}^n3

If the packed array is immutable, then the entire data underlying the packed
array can be one contiguous range of bits, where padding is only necessary
for the last byte.



