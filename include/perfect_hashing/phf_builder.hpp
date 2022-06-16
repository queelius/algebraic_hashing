#pragma once

#include "phf.hpp"
#include <vector>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <algorithm>
#include <limits>
#include <chrono>

namespace perfect_hashing
{
  template <typename H>
  struct phf_builder
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

    static auto default_threads()
    {
      return std::thread::hardware_concurrency();
    }

    static auto max_timeout()
    {
      return std::chrono::milliseconds::max().count();
    }

    double r;
    H h;
    size_t lower_index;
    size_t upper_index;
    size_t num_threads;
    std::chrono::milliseconds duration;

    phf_builder() :
      r(default_load_factor()),
      lower_index(min_index()),
      upper_index(max_index()),
      duration(max_timeout()),
      num_threads(default_threads()) {}

    auto & threads(size_t n = 0)
    {
      num_threads = (n == 0 ? std::thread::hardware_concurrency() : n);
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

    auto & timeout(std::chrono::milliseconds ms)
    {
      duration = ms;
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
      auto m = std::distance(begin,end);
      auto N = (size_t)std::ceil(m/r);

      size_t l_star;
      size_t succ_star = 0;
      size_t collides_star = 0;
      std::mutex lck;
      auto const start_time = std::chrono::system_clock::now();

      auto task = [&](size_t start, size_t stop)
      {
        std::vector<bool> K(N);
        for (auto l = start; l != stop; ++l)
        {
          auto const cur_time = std::chrono::system_clock::now();
          auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            cur_time - start_time);
          if (succ_star == m || elapsed > duration)
            break;
          size_t succ = 0;
          size_t collides = 0;
          for (auto x = begin; x != end; ++x)
          {
            auto hash = (h(*x) ^ l) % N;
            if (K[hash])
            {
              if (++collides > collides_star)
                break;
            }
            else
            {
              ++succ;
              K[hash] = true;
            }
          }

          if (succ > succ_star)
          {
            lck.lock();
            if (succ > succ_star)
            {
              l_star = l;
              succ_star = succ;
              collides_star = collides;
            }
            lck.unlock();
          }
          K.assign(N,false);
        }
      };

      size_t block_size = (upper_index - lower_index) / num_threads;
      std::vector<std::thread> threads;
      for (int i = 0; i < num_threads; ++i)
        threads.push_back(std::thread(task,lower_index+i*block_size,
                                      lower_index+(i+1)*block_size));
      for (auto & t : threads)
        t.join();
      
      return phf<H>(N,h,l_star,1.0-(double)(succ_star)/m);
    }

    template <typename X>
    auto operator()(X xs)
    {
      return operator()(std::begin(xs), std::end(xs));
    }
  };
}