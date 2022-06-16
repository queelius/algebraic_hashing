
-   <a href="#c-library-algebraic_perfect_hash_functions"
    id="toc-c-library-algebraic_perfect_hash_functions">C++ library:
    <code>algebraic_perfect_hash_functions</code></a>

# C++ library: `algebraic_perfect_hash_functions`

Theorem (Fermat’s Little).  
If
![p](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;p "p")
is a prime number, then for any integer
![a](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a "a"),
the number

![a^p - a](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a%5Ep%20-%20a "a^p - a")

is an integer multiple of
![p](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;p "p").

See the [function
reference](https://queelius.github.io/perfect_hash_functions/index.html)

A *set* is an unordered collection of distinct elements. If we know the
elements in a set are given by
![a](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a "a"),
![b](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;b "b"),
and
![c](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;c "c"),
we may denote the set with the notation
![\\{a,c,b\\}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7Ba%2Cc%2Cb%5C%7D "\{a,c,b\}").

A *finite* set has a finite number of elements. For example,
![\\{ 1, 3, 5 \\}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7B%201%2C%203%2C%205%20%5C%7D "\{ 1, 3, 5 \}")
is a finite set with three elements. When sets
![\mathbb{A}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BA%7D "\mathbb{A}")
and
![\mathbb{B}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BB%7D "\mathbb{B}")
are *isomorphic*, denoted by
![\mathbb{A} \cong \mathbb{B}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BA%7D%20%5Ccong%20%5Cmathbb%7BB%7D "\mathbb{A} \cong \mathbb{B}"),
they can be put into a one-to-one correspondence (bijection), e.g.,
![\\{b,a,c\\} \cong \\{1,2,3\\}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7Bb%2Ca%2Cc%5C%7D%20%5Ccong%20%5C%7B1%2C2%2C3%5C%7D "\{b,a,c\} \cong \{1,2,3\}").
Since there exists at least one bijection between isomorphic sets, we
can losslessly convert one to the other and thus, isomorphic sets are in
some sense equivalent.

The cardinality of a finite set
![\mathbb{A}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BA%7D "\mathbb{A}")
is the number of elements in the set, denoted by
![\|\mathbb{A}\|](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%7C%5Cmathbb%7BA%7D%7C "|\mathbb{A}|"),
e.g.,
![\|\\{ 1, 3, 5\\}\| = 3](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%7C%5C%7B%201%2C%203%2C%205%5C%7D%7C%20%3D%203 "|\{ 1, 3, 5\}| = 3").
A *countably infinite* set is isomorphic to the set of natural numbers
![\mathbb{N} = \\{1,2,3,\ldots\\}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BN%7D%20%3D%20%5C%7B1%2C2%2C3%2C%5Cldots%5C%7D "\mathbb{N} = \{1,2,3,\ldots\}").

Given two elements
![a](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a "a")
and
![b](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;b "b"),
an ordered pair of
![a](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a "a")
then
![b](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;b "b")
is denoted by
![(a,b)](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%28a%2Cb%29 "(a,b)"),
where
![(a,b) = (c,d)](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%28a%2Cb%29%20%3D%20%28c%2Cd%29 "(a,b) = (c,d)")
iff
![a = c](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a%20%3D%20c "a = c")
and
![b = d](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;b%20%3D%20d "b = d").
Ordered pairs are non-commutative and non-associative, i.e.,
![(a,b) \neq (b,a)](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%28a%2Cb%29%20%5Cneq%20%28b%2Ca%29 "(a,b) \neq (b,a)")
if
![a \neq b](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;a%20%5Cneq%20b "a \neq b")
and
![\bigl(a,(b,c)\bigr) \neq \bigl((b,a),c\bigr)](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cbigl%28a%2C%28b%2Cc%29%5Cbigr%29%20%5Cneq%20%5Cbigl%28%28b%2Ca%29%2Cc%5Cbigr%29 "\bigl(a,(b,c)\bigr) \neq \bigl((b,a),c\bigr)").

Related to the ordered pair is the Cartesian product.
By the non-commutative and non-associative property of ordered pairs,
the Cartesian product is non-commutative and non-associative. However,
they are isomorphic, i.e.,
![\mathbb{X} \times \mathbb{Y} \cong \mathbb{Y} \times \mathbb{X}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%20%5Ctimes%20%5Cmathbb%7BY%7D%20%5Ccong%20%5Cmathbb%7BY%7D%20%5Ctimes%20%5Cmathbb%7BX%7D "\mathbb{X} \times \mathbb{Y} \cong \mathbb{Y} \times \mathbb{X}").

A is a generalization of order pairs which can consist of an arbitrary
number of elements, e.g.,
![(x_1,x_2,\ldots,x_n)](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%28x_1%2Cx_2%2C%5Cldots%2Cx_n%29 "(x_1,x_2,\ldots,x_n)").
Note that

![\mathbb{X_1} \times \mathbb{X_2} \times \mathbb{X_3} \cong
  \mathbb{X_1} \times \left(\mathbb{X_2} \times \mathbb{X_3}\right) \cong
  \left(\mathbb{X_1} \times \mathbb{X_2}\right) \times \mathbb{X_3},](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX_1%7D%20%5Ctimes%20%5Cmathbb%7BX_2%7D%20%5Ctimes%20%5Cmathbb%7BX_3%7D%20%5Ccong%0A%20%20%5Cmathbb%7BX_1%7D%20%5Ctimes%20%5Cleft%28%5Cmathbb%7BX_2%7D%20%5Ctimes%20%5Cmathbb%7BX_3%7D%5Cright%29%20%5Ccong%0A%20%20%5Cleft%28%5Cmathbb%7BX_1%7D%20%5Ctimes%20%5Cmathbb%7BX_2%7D%5Cright%29%20%5Ctimes%20%5Cmathbb%7BX_3%7D%2C "\mathbb{X_1} \times \mathbb{X_2} \times \mathbb{X_3} \cong
  \mathbb{X_1} \times \left(\mathbb{X_2} \times \mathbb{X_3}\right) \cong
  \left(\mathbb{X_1} \times \mathbb{X_2}\right) \times \mathbb{X_3},")

thus we may implicitly convert between them without ambiguity.

If each set in the
![n](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;n "n")-ary
Cartesian product is the same, the power notation may be used, e.g.,
![\mathbb{X}^3 \equiv \mathbb{X} \times \mathbb{X} \times \mathbb{X}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%5E3%20%5Cequiv%20%5Cmathbb%7BX%7D%20%5Ctimes%20%5Cmathbb%7BX%7D%20%5Ctimes%20%5Cmathbb%7BX%7D "\mathbb{X}^3 \equiv \mathbb{X} \times \mathbb{X} \times \mathbb{X}").
As special cases, the
![0](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;0 "0")-ary
(nullary) Cartesian product is defined to be
![\\{ \emptyset \\}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7B%20%5Cemptyset%20%5C%7D "\{ \emptyset \}")
and the
![1](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;1 "1")-ary
(unary) Cartesian product is the identity, e.g.,
![\mathbb{X}^1 = \mathbb{X}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%5E1%20%3D%20%5Cmathbb%7BX%7D "\mathbb{X}^1 = \mathbb{X}").

For example, a bijection between
![\\{0,1\\}^m](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7B0%2C1%5C%7D%5Em "\{0,1\}^m")
and
![\\{1,2,\ldots,2^m\\}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7B1%2C2%2C%5Cldots%2C2%5Em%5C%7D "\{1,2,\ldots,2^m\}")
is given by the equation

![b_m \bicond \sum\_{j=1}^{m} 2^{m - j + 1} b_j\\,,](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;b_m%20%5Cbicond%20%5Csum_%7Bj%3D1%7D%5E%7Bm%7D%202%5E%7Bm%20-%20j%20%2B%201%7D%20b_j%5C%2C%2C "b_m \bicond \sum_{j=1}^{m} 2^{m - j + 1} b_j\,,")

where
![b_m \in \\{0,1\\}^m](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;b_m%20%5Cin%20%5C%7B0%2C1%5C%7D%5Em "b_m \in \{0,1\}^m").
We denote the transformation in either direction by the unary operation
!['](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%27 "'"),
e.g.,
![2' = 0](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;2%27%20%3D%200 "2' = 0").
We show some of the mappings in the following table:

| ![\mathbb{N}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BN%7D "\mathbb{N}") | ![\\{0,1\\}^\*](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5C%7B0%2C1%5C%7D%5E%2A "\{0,1\}^*") |
|---------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------|
| 1                                                                                                                         | ![\emptyset](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cemptyset "\emptyset")                |
| 2                                                                                                                         | ![0_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;0_2 "0_2")                                    |
| 3                                                                                                                         | ![1_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;1_2 "1_2")                                    |
| 4                                                                                                                         | ![00_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;00_2 "00_2")                                 |
| 5                                                                                                                         | ![01_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;01_2 "01_2")                                 |
| 6                                                                                                                         | ![10_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;10_2 "10_2")                                 |
| 7                                                                                                                         | ![11_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;11_2 "11_2")                                 |
| 8                                                                                                                         | ![000_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;000_2 "000_2")                              |
| 9                                                                                                                         | ![001_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;001_2 "001_2")                              |
| 10                                                                                                                        | ![010_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;010_2 "010_2")                              |
| 11                                                                                                                        | ![011_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;011_2 "011_2")                              |
| 12                                                                                                                        | ![100_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;100_2 "100_2")                              |
| 13                                                                                                                        | ![101_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;101_2 "101_2")                              |
| 14                                                                                                                        | ![110_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;110_2 "110_2")                              |
| 15                                                                                                                        | ![111_2](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;111_2 "111_2")                              |
| ![\vdots](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cvdots "\vdots")                 | ![\vdots](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cvdots "\vdots")                         |

A *hash function* is given by the following definition.
For a given bit string
![x](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;x "x")
and hash function
![\operatorname{hash}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Coperatorname%7Bhash%7D "\operatorname{hash}"),
![y = \operatorname{hash}(x)](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;y%20%3D%20%5Coperatorname%7Bhash%7D%28x%29 "y = \operatorname{hash}(x)")
is denoted the of
![x](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;x "x").

We are particularly interested in *perfect* hash functions as given by
the following definition.
Perfect hash functions are generally *surjective* when not restricted to
the range of elements it perfectly hashes.

The *load factor* is given by the following definition.
A perfect hash function of type
![\mathbb{X} \mapsto \mathbb{Y}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%20%5Cmapsto%20%5Cmathbb%7BY%7D "\mathbb{X} \mapsto \mathbb{Y}")
over
![\mathbb{A}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BA%7D "\mathbb{A}")
with a load factor
![r](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;r "r")
may be denoted by
![\operatorname{hash}\_{\mathbb{A}}^{r}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Coperatorname%7Bhash%7D_%7B%5Cmathbb%7BA%7D%7D%5E%7Br%7D "\operatorname{hash}_{\mathbb{A}}^{r}").
If
![m = \|{\mathbb{A}\\}\|](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;m%20%3D%20%7C%7B%5Cmathbb%7BA%7D%5C%7D%7C "m = |{\mathbb{A}\}|")
and we are interested in drawing attention to the cardinality of the
perfect hash function, we may also denote it by
![\operatorname{hash}\_{m}^{r}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Coperatorname%7Bhash%7D_%7Bm%7D%5E%7Br%7D "\operatorname{hash}_{m}^{r}").

Every hash function in
![\mathbb{X} \mapsto \mathbb{Y}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%20%5Cmapsto%20%5Cmathbb%7BY%7D "\mathbb{X} \mapsto \mathbb{Y}")
is a perfect hash function over some subset of
![\mathbb{X}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D "\mathbb{X}"),
e.g., every hash function is trivially a perfect hash function of the
empty set and singleton sets.

Assuming
![\mathbb{X}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D "\mathbb{X}")
and
![\mathbb{Y}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BY%7D "\mathbb{Y}")
are finite, the set of hash functions of type
![\mathbb{X} \mapsto \mathbb{Y}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%20%5Cmapsto%20%5Cmathbb%7BY%7D "\mathbb{X} \mapsto \mathbb{Y}"),
which may also be denoted by
![\mathbb{Y}^{\mathbb{X}}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BY%7D%5E%7B%5Cmathbb%7BX%7D%7D "\mathbb{Y}^{\mathbb{X}}"),
has a cardinality

![\|{\mathbb{Y}}^{\|\mathbb{X}\|}\|.](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%7C%7B%5Cmathbb%7BY%7D%7D%5E%7B%7C%5Cmathbb%7BX%7D%7C%7D%7C. "|{\mathbb{Y}}^{|\mathbb{X}|}|.")

The set of perfect hash functions over
![\mathbb{A} \subseteq \mathbb{X}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BA%7D%20%5Csubseteq%20%5Cmathbb%7BX%7D "\mathbb{A} \subseteq \mathbb{X}")
is a subset of
![\mathbb{X} \mapsto \mathbb{Y}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BX%7D%20%5Cmapsto%20%5Cmathbb%7BY%7D "\mathbb{X} \mapsto \mathbb{Y}")
with the predicate that no collisions may occur on any pair of elements
in
![\mathbb{A}](https://latex.codecogs.com/png.image?%5Cdpi%7B110%7D&space;%5Cbg_white&space;%5Cmathbb%7BA%7D "\mathbb{A}").
