# Concept: Perfect Hash Function {#phf}

A hash function

$$
f : X \mapsto \{0,1\}^k
$$

for some data type $X$ maps every $x \in X$ to some bit string of lengh $k > 0$.
In the context of hash functions, we may substitute *maps* with *hashes*, i.e.,
$x$ *hashes* to $f(x)$.

A collision occurs between two values $x,y \in X$, $x \neq y$, if $f(x) = f(y)$.

A **perfect** hash function $f$ over some subset $X' \subseteq X$ (where $X$ may
be some data type, like a string) satisfies two conditions:

1. $f \colon X \mapsto Y$ is a hash function.
2. $f$ is one-to-one over $X'$, i.e., no hash collisions occur on any pair of
elements in $X'$.

A perfect hash function $f$ is a *minimal* perfect hash function if it has a
range of $[0,m-1]$ where $m$ is the cardinality of $X'$. (Note that we may
encode each integer in the range $[0,m-1]$ using a standard binary encoding so
that $f$ still has a codomain ${0,1}^k$.)
