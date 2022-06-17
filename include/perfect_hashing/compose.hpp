


#pragma once

template <typename F, typename PH>
struct perfect_hash_fn_compose
{
  F f;
  PH ph;

  auto operator()(auto x) const
  {
    g(ph(x));
  }

  auto load_factor() const
  {
    return 0;
  }
};
