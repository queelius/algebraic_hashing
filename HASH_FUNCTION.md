# Concept: Hash Function {#phf}

A hash function

$$
f : X \mapsto \{0,1\}^k
$$

for some data type $X$ maps every $x \in X$ to some bit string of lengh $k > 0$.
In the context of hash functions, we may substitute *maps* with *hashes*, i.e.,
$x$ *hashes* to $f(x)$.

A collision occurs between two values $x,y \in X$, $x \neq y$, if $f(x) = f(y)$.
The elements of $X$ should be, a priori, appear uniformly distributed over the
codomain $\{0,1\}^k$ and thus, the probability that two random elements $x,y \in X$
collide is given by $|X|^{-1}$.
