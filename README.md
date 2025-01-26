Algebraic hash functions
========================

Hash Functions
--------------

A hash function
$$
f : X \mapsto \{0,1\}^k
$$
for some data type $X$ maps every $x \in X$ to some bit string of lengh $k$.
In the context of hash functions, we may substitute *maps* with *hashes*, i.e.,
$x$ *hashes* to $f(x)$.

A collision occurs between two values $x,y \in X$, $x \neq y$, if $f(x) = f(y)$.
The elements of $X$ should be, a priori, appear uniformly distributed over the
codomain $\{0,1\}^k$ and thus, the probability that two random elements $x,y \in X$
collide is given by $|X|^{-1}$.

We implement several hash functions, both cryptographic and non-cryptographic.

We then provide a sort of an algebra over these hash functions, e.g., composing
a hash function with certain types of functions results in other hash functions.

Perfect Hash Functions (PHF)
----------------------------

A **perfect** hash function $f$ over some subset $X' \subseteq X$ (where $X$ may
be some data type, like a string) satisfies two conditions:

1. $f : X \mapsto Y$ is a hash function.
2. $f$ is one-to-one over $X'$, i.e., no hash collisions occur on any pair of
elements in $X'$.

A perfect hash function $f$ is a *minimal* perfect hash function if it has a
range of $[0,m-1]$ where $m$ is the cardinality of $X'$. (Note that we may
encode each integer in the range $[0,m-1]$ using a standard binary encoding so
that $f$ still has a codomain $\{0,1\}^k$.)

PHFs may be used for memory efficient storage and
fast retrieval of items from static sets, among other uses. 


### Implementation of PHFs

We implement two data structures that model the concept of PHFs.
We denote them respectively by `phf` and `phf_two_lvl`.
The `phf` implementation is mainly of theoretical interest. It is a simple
implementation of a PHF that is not particularly memory efficient nor fast.
For instance, if $m = |X'|$, and the load factor $r = N/m$, then the
probability that no collisions occur for a random hash function for all
$x \in X'$ is

$$
p(m,N) = \frac{N}{N} \cdot \frac{N-1}{N} \cdots \frac{N-m+1}{N} = \frac{N!}{N^m(N-m)!}
$$

$$
p(m,r) = \Bigl(\frac{r}{m}\Bigr)^m \Bigl(\frac{m}{r}-1\Bigr) \cdots \Bigl(\frac{m}{r}-m+1\Bigr)
$$


is more memory efficient and provides faster queries.
However, construction can be very slow for larger sets.
`phf_lvl2` provides much faster constructions at the
cost of slightly slower queries and more in-place memory requirements.
However, it is still quite fast and memory-efficient.

More gains in memory efficiency are readily possible by using in-place
compression, e.g., in `phf`, the `l` value can be easily compressed since
it should be geometric with a known `p` value. In this case, geometric
coding may be used. However, since we are only storing a single value,
there does not seem to be much point in doing so.

It makes more sense to pursue in-place compression for `phf_lvl2`, which stores
a vector of integers.