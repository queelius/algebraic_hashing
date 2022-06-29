#pragma once

#include "rd_phf_lvl2.hpp"
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>
#include <iostream>

namespace algebraic_hashing::perfect_hashing
{
  template <typename H>
  struct rd_phf_lvl2_builder
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
      return 0.5;
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

    bool debug;
    std::ostream & debug_out;
    double r;
    H h;
    size_t l0;
    size_t lower_index;
    size_t upper_index;
    size_t m;
    std::chrono::milliseconds duration;

    rd_phf_lvl2_builder() :
      debug(false),
      debug_out(std::cout),
      r(default_load_factor()),
      l0(0),
      lower_index(min_index()),
      upper_index(max_index()),
      m(0),
      duration(max_timeout()) {}

    /**
     * @brief Set debug mode to true or false.
     * @param mode if mode is true, then show debugging information.
     */
    auto & debugging(bool mode = true)
    {
      debug = mode;
      return *this;
    }    
    
    /**
     * @brief Set debug output.
     * @param out the debugging output stream.
     */
    auto & debug_output(std::ostream & out)
    {
      debug_out = out;
      return *this;
    }

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

    /**
     * @brief Set the number of level 2 buckets (bins).
     * 
     * @param m number of buckets (bins). if 0, then automatically sets it to
     *          some "good" value.
     */
    auto & num_buckets(size_t value = 0)
    {
      m = value;
      return *this;
    }

    /**
     * @brief Set the level 0 index.
     * 
     * @param l0 level 0 index.
     */
    auto & level0(size_t value = 0)
    {
      l0 = value;
      return *this;
    }


    /**
     * @brief Set the target load factor, a value in the interval (0,1].
     * @param r the target load factor of the perfect hash function.
     *          if a perfect hash function cannot be constructed, then the load
     *          factor for the perfectly hashed subset has r as an upper-bound.
     */
    auto & load_factor(double value)
    {
      r = std::min(max_load_factor(), std::max(value, min_load_factor()));
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
      std::sort(begin,end);
      end = std::unique(begin,end);

      auto res = build(begin,end,l0);
      auto min_error = res.error_rate();
      if (l0 == 0)
      {
        for (size_t l = 1; l < 3000; ++l)
        {
            auto trial = build(begin,end,l);
            if (trial.error_rate() < min_error)
            {
                min_error = trial.error_rate();
                res = trial;
            }
        }
      }
      return res;
    }

    template <typename I>
    auto build(I begin, I end, size_t l0)
    {
      using X = typename std::iterator_traits<I>::value_type;
      struct entry
      {
          size_t index;
          std::vector<X> xs;
      };

      auto sz = std::distance(begin,end);
      auto N = (size_t)std::ceil(sz/r);

      if (m == 0)
        m = (size_t)std::ceil(sqrt((double)sz));

      std::vector<entry> B(m);            
      std::vector<bool> T(N,false);
      std::vector<size_t> sigma(m,0);

      for (auto x = begin; x != end; ++x)
      {
        auto h0 = h.mix(l0,*x) % m;
        B[h0].xs.push_back(*x);
        B[h0].index = h0;
      }

      std::sort(B.begin(), B.end(),
        [](auto const & b1, auto const & b2)
        { return b1.xs.size() > b2.xs.size(); });

      if (debug)
      {
        debug_out << "l0: " << l0 << "\n";
        debug_out << "#(bins): " << m << "\n";
        for (auto const & b : B)
            debug_out << "size(bin #" << b.index << "): " << b.xs.size() << "\n";
      }

      size_t total_cols = 0;
      auto const start_time = std::chrono::system_clock::now();
      for (auto const & b : B)
      {
        auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now() - start_time);
        if (elapsed > duration)
            break;

        size_t l1;
        auto col1 = b.xs.size();
        std::vector<bool> K;
        for (size_t l = lower_index; l <= upper_index; ++l)
        {
            K = T;
            size_t col = 0;
            for (auto const x : b.xs)
            {
                auto const hash = h.mix(l,x) % N;
                if (K[hash])
                    if (++col >= col1) break;
                K[hash] = true;
            }
            if (col < col1)
            {
                col1 = col; l1 = l;
                if (col1 == 0)
                    break;
            }
        }

        total_cols += col1;
        sigma[b.index] = l1;
        std::swap(T,K);
        if (debug)
            debug_out << "bin " << b.index << ": l1=" << l1 << ", col1=" << col1 << "\n";
      }
      return rd_phf_lvl2(N,m,l0,(double)total_cols/sz,h,sigma);
    }


    template <typename X>
    auto operator()(X xs)
    {
      return operator()(std::begin(xs), std::end(xs));
    }
  };
}