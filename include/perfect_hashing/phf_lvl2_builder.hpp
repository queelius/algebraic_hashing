#pragma once

#include "phf_lvl2.hpp"
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_set>
#include <chrono>

namespace perfect_hashing
{
  template <typename H>
  struct phf_lvl2_builder
  {
    static auto max_index()
    {
      return std::numeric_limits<size_t>::max();
    }

    static auto min_index()
    {
      return (size_t)0;
    }

    static auto default_load_factor()
    {
      return 0.8;
    }

    static auto max_load_factor()
    {
      return 1.0;
    }

    static auto min_load_factor()
    {
      return std::nextafter(0.0,1.0);
    }

    static auto max_timeout()
    {
      return std::chrono::milliseconds::max().count();
    }

    double r;
    H h;
    size_t lower_index;
    size_t upper_index;
    size_t m;
    std::chrono::milliseconds duration;

    phf_lvl2_builder() :
      r(default_load_factor()),
      lower_index(min_index()),
      upper_index(max_index()),
      m(0),
      duration(max_timeout()) {}

    /**
     * @brief Set the time-out, the length of time before setttling for the
     *        best solution found.     * 
     * 
     * @param ms the time-out in milliseconds
     */
    auto & timeout(std::chrono::milliseconds ms)
    {
      duration = ms;
      return *this;
    }

    auto & num_buckets(size_t m = 0)
    {
      this->m = m;
      return *this;
    }

    /**
     * @brief Set the target load factor, a value in the interval (0,1].
     * @param r the target load factor of the perfect hash function.
     *          if a perfect hash function cannot be constructed, then the load
     *          factor for the perfectly hashed subset has r as an upper-bound.
     */
    auto & load_factor(double r)
    {
      this->r = std::min(max_load_factor(), std::max(r, min_load_factor()));
      return *this;
    }

    /**
     * @brief Set the hash function object.
     * @param h hash function
     */
    auto & hash_fn(H h)
    {
      this->h = h;
      return *this;
    }

    /**
     * @brief Sets the index range [lower,upper], the space over which to search
     *        for a perfect hash function.
     *
     * @param lower start of search space
     * @param upper end of the search space
     */
    auto & index(size_t lower = min_index(),
                 size_t upper = max_index())
    {
      lower_index = lower;
      upper_index = upper;
      return *this;
    }

    /**
     * @brief Constructs a random perfect hash function for the range of
     *        elements in [begin,end).
     *
     * @tparam I models a forward iterator
     * @param begin start of range
     * @param end end of range
     */
    template <typename I>
    auto operator()(I begin, I end)
    {
      using X = typename std::iterator_traits<I>::value_type;
      struct entry
      {
          size_t index;
          std::vector<X> xs;
      };

      std::sort(begin,end);
      end = std::unique(begin,end);
      auto sz = std::distance(begin,end);
      auto N = (size_t)std::ceil(sz/r);

      if (m == 0)
        m = (size_t)std::ceil(sqrt((double)sz));
      
      std::vector<entry> B(m);            
      std::unordered_set<size_t> T;
      std::unordered_set<size_t> K;
      std::vector<size_t> sigma(m,1);

      for (size_t i = 0; i < m; ++i)
        B[i].index = i;

      for (auto x = begin; x != end; ++x)
        B[h(*x) % m].xs.push_back(*x);

      std::sort(B.begin(), B.end(),
        [](auto const & b1, auto const & b2)
        { return b1.xs.size() > b2.xs.size(); });

      auto const start_time = std::chrono::system_clock::now();
      for (size_t i = 0; i < m; ++i)
      {
        auto const cur_time = std::chrono::system_clock::now();
        auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
          cur_time - start_time);
        if (elapsed > duration)
          break;

        size_t l = 1;
        size_t j = 0;

        K.clear();
        while (j < B[i].xs.size())
        {
          auto const hash = h.mix(h(B[i].xs[j]),l) % N;
          if (T.count(hash) != 0 || K.count(hash) != 0)
            { ++l; j = 0; K.clear(); }
          else
            { K.insert(hash); ++j; }
        }

        sigma[B[i].index] = l;
        for (auto j : K)
          T.insert(j);
      }
      return phf_lvl2(N,m,h,sigma);
    }

    template <typename X>
    auto operator()(X xs)
    {
      return operator()(std::begin(xs), std::end(xs));
    }
  };
}