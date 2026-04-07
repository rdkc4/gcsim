#ifndef THREADS_CFG_HPP
#define THREADS_CFG_HPP

#include <cstddef>

/**
 * @namespace cfg::threads
 * @brief module defining the constants related to threads.
*/
namespace cfg::threads {
    /// number of thread of the garbage collector's thread pool.
    constexpr size_t GC_THREAD_COUNT{8};

};

// thread assertions.
static_assert(cfg::threads::GC_THREAD_COUNT > 0, "GC thread count must be positive");

#endif