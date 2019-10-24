# Concept: RegularType

A `RegularType` is a concept that requires the following. Consider a data type
`X`. If `X` is a `RegularType`, then it implements assignment, default
construction, copy construction, and move constuction.

For example, the following syntax must be valid:
``` cpp
    X x;
    X y(x);
    y = x;
    X z(move(y));
```