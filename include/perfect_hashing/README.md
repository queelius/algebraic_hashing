# Perfect hashing

[Perfect hash functions](@ref phf) may be used for memory efficient storage and
fast retrieval of items from static sets, among other uses. The CMPH library may
be used to efficiently generate perfect hash functions. It is efficient in two
ways:
1. Perfect hash function generators obtain linear time complexity.
2. The in-place storage requirements are compact and near-optimal.

## Description

We implement several data structures that implement the abstract data type of 
perfect hash functions. For three of these perfect hash functions, we just
provide a wrapper around the C Minimal Perfect Hashing (CMPH) library.

## Supported Algorithms

The following perfect hash function generators are available.
    - CHD
    - CHD_PH
    - FCH

