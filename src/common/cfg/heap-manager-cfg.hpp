#ifndef HEAP_MANAGER_CFG_HPP
#define HEAP_MANAGER_CFG_HPP

#include <chrono>
#include <cstdint>

#include "heap-cfg.hpp"

/**
 * @namespace cfg::heap_manager
 * @brief module defining constants related to heap manager.
*/
namespace cfg::heap_manager {
    /// max size of the small object.
    constexpr uint32_t SMALL_OBJECT_THRESHOLD = 256; //<256B

    /// max size of the medium object.
    constexpr uint32_t MEDIUM_OBJECT_THRESHOLD = 2 * 1024; //<2kB

    /// max size of the large object.
    constexpr uint32_t LARGE_OBJECT_THRESHOLD = 256 * 1024; //<256kB

    /// min interval between two gc calls.
    constexpr int64_t MIN_INTERVAL = 100; //<100ms

    /// interval between two automatic gc calls.
    constexpr int64_t PERIODIC_INTERVAL = 1000; //<1000ms

    /// trigger for gc when free memory on the heap is low.
    constexpr uint32_t GC_LOW_MEMORY_THRESHOLD = heap::TOTAL_SEGMENTS * heap::SEGMENT_SIZE / 10;

    /// min interval between gc calls.
    constexpr std::chrono::milliseconds MIN_GC_INTERVAL{MIN_INTERVAL};

    /// periodic interval of the gc calls.
    constexpr std::chrono::milliseconds PERIODIC_GC_INTERVAL{PERIODIC_INTERVAL};

};

// object threshold assertions.
static_assert(cfg::heap_manager::SMALL_OBJECT_THRESHOLD > 0, "Small object threshold must be positive");
static_assert(
    cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD > cfg::heap_manager::SMALL_OBJECT_THRESHOLD, 
    "Medium object treshold must be greater than small object threshold"
);
static_assert(
    cfg::heap_manager::LARGE_OBJECT_THRESHOLD > cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD, 
    "Large object treshold must be greater than medium object threshold"
);

#endif