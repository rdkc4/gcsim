#ifndef THREADS_CFG_HPP
#define THREADS_CFG_HPP

#include <cstddef>

/**
 * @namespace cfg::threads
 * @brief module defining the constants related to threads.
*/
namespace cfg::threads {
    /// number of thread of the garbage collector's thread pool.
    constexpr size_t GC_THREAD_COUNT = 8;

    /// array of numbers of the allocator threads.
    constexpr size_t ALLOC_THREAD_COUNTS[] = {1, 2, 5, 10};

    /// length of the alloc thread counts array.
    constexpr size_t ALLOC_THREAD_COUNTS_LENGTH = sizeof(ALLOC_THREAD_COUNTS) / sizeof(size_t);

};

// thread assertions.
static_assert(cfg::threads::GC_THREAD_COUNT > 0, "GC thread count must be positive");

#endif