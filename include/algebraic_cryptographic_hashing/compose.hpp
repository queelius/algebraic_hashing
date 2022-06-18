#pragma once

namespace hashing
{
  namespace
  {
    struct identity_fn
    {
      template <typename X>
      auto operator()(X const & x) const { return x; }
    };
  }

  template <typename PH, typename F = identity_fn>
  struct perfect_hash_fn_compose
  {
    F f;
    PH ph;

    auto operator()(auto x) const
    {
      return f(ph(x));
    }
  };
}