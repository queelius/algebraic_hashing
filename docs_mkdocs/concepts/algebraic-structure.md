# Hash Functions as Algebra

AlgebraicHashing treats hash functions as first-class mathematical objects. This page explores the algebraic structures that emerge from this perspective.

## Mathematical Foundation

### Hash Values Form an Abelian Group

Under XOR, hash values form an **abelian group** $(H, \oplus)$:

| Property | Definition | Example |
|----------|------------|---------|
| **Closure** | $a \oplus b \in H$ | `h1 ^ h2` is a hash value |
| **Associativity** | $(a \oplus b) \oplus c = a \oplus (b \oplus c)$ | `(h1 ^ h2) ^ h3 == h1 ^ (h2 ^ h3)` |
| **Identity** | $\exists 0: a \oplus 0 = a$ | `h ^ hash64::zero() == h` |
| **Inverse** | $a \oplus a = 0$ | `h ^ h == hash64::zero()` |
| **Commutativity** | $a \oplus b = b \oplus a$ | `h1 ^ h2 == h2 ^ h1` |

This structure is fundamental to the library's design:

```cpp
hash64 h1 = fnv64{}("hello");
hash64 h2 = fnv64{}("world");
hash64 zero = hash64::zero();

// Verify group properties
assert((h1 ^ h2) ^ h3 == h1 ^ (h2 ^ h3));  // Associative
assert(h1 ^ zero == h1);                     // Identity
assert(h1 ^ h1 == zero);                     // Self-inverse
assert(h1 ^ h2 == h2 ^ h1);                  // Commutative
```

### Hash Functions Form a Monoid

The set of hash functions with sequential composition forms a **monoid** $(F, \circ)$:

| Property | Definition |
|----------|------------|
| **Closure** | $f \circ g$ is a hash function |
| **Associativity** | $(f \circ g) \circ h = f \circ (g \circ h)$ |
| **Identity** | $\exists id: f \circ id = id \circ f = f$ |

```cpp
fnv64 f;
murmur3_64 g;
xxhash64 h;

// Sequential composition is associative
auto fg_h = (f * g) * h;
auto f_gh = f * (g * h);
// These produce the same composed function
```

### Combined Structure: Near-Ring

When we combine XOR composition (⊕) with sequential composition (∘), we get a structure resembling a **near-ring**:

$$
(H \to H, \oplus, \circ)
$$

With these properties:

1. $(H \to H, \oplus)$ is an abelian group
2. $(H \to H, \circ)$ is a monoid
3. Right distributivity: $(f \oplus g) \circ h = (f \circ h) \oplus (g \circ h)$

!!! note "Left Distributivity"
    Left distributivity does NOT hold: $h \circ (f \oplus g) \neq (h \circ f) \oplus (h \circ g)$

    This is because $h(f(x) \oplus g(x)) \neq h(f(x)) \oplus h(g(x))$ for most hash functions.

## Algebraic Properties in Practice

### Independence Through XOR

When two hash functions are independent (uncorrelated), their XOR composition can provide better properties:

$$
P(\text{collision in } f \oplus g) \approx P(\text{collision in } f) \times P(\text{collision in } g)
$$

```cpp
// Combining independent hash functions reduces collision probability
auto combined = fnv64{} ^ murmur3_64{};
// If each has 2^{-64} collision probability,
// combined has approximately 2^{-128} collision probability
```

### Entropy Bounds

For XOR composition, entropy is bounded by the minimum:

$$
H(f \oplus g) \leq \min(H(f), H(g))
$$

For independent functions, entropy approaches the minimum. The library provides entropy estimates:

```cpp
fnv64 f;
murmur3_64 g;

auto combined = f ^ g;
double entropy = combined.entropy();  // Conservative estimate
```

### Hash Stretching

Sequential composition can be used for hash stretching (similar to key derivation):

```cpp
// Apply hash repeatedly for stretching
auto stretched = repeat<1000>(fnv64{});
auto result = stretched("password");
```

!!! warning "Not Cryptographic"
    This is NOT a secure key derivation function. For security-critical applications, use established KDFs like PBKDF2, scrypt, or Argon2.

## The Category-Theoretic View

For the mathematically inclined, hash functions can be viewed through category theory:

- **Objects**: Types that can be hashed (Hashable types)
- **Morphisms**: Hash functions between types
- **Composition**: Sequential composition `*`
- **Identity**: The identity hash function

This forms a category **Hash** where:

```
ob(Hash) = Hashable types
hom(A, H) = {f : A → H | f is a hash function}
```

The XOR operation then becomes a **bifunctor** on this category, making composed hash functions natural transformations.

## Practical Applications

### Bloom Filters

Bloom filters traditionally need k independent hash functions. With algebraic composition:

```cpp
fnv64 base;
auto h1 = base;
auto h2 = base ^ murmur3_64{};
auto h3 = base ^ xxhash64{};
// Use h1, h2, h3 for Bloom filter
```

### Perfect Hashing

The algebraic structure enables constructing minimal perfect hash functions through composition:

```cpp
// Theoretical: find f such that f restricted to key set is bijective
auto phf = find_perfect_hash(keys);
```

### Consistent Hashing

For distributed systems, composition enables sophisticated consistent hashing schemes:

```cpp
// Combine node hash with key hash
auto slot = (node_hasher ^ key_hasher)(key);
```

## Further Reading

- [Core Concepts](core-concepts.md) - The type system
- [Design Philosophy](philosophy.md) - Why algebraic composition
- [Performance Guide](../tutorials/performance.md) - Practical considerations
