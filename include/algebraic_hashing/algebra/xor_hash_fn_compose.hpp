#pragma once

namespace algebraic_hashing {
  
template <typename H1, typename H2>
struct xor_hash_fn_compose
{
  H1 h1;
  H2 h2;

  using hash_type = typename H1::hash_type;

  template <typename X>
  auto operator()(X const & x) const
  {
    return static_cast<hash_type>(h1(x) ^ h2(x));
  }
};

} // namespace algebraic_hashing