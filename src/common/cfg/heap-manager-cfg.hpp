#ifndef HEAP_MANAGER_CFG_HPP
#define HEAP_MANAGER_CFG_HPP

#include <chrono>
#include <cstdint>

/**
 * @namespace cfg::heap_manager
 * @brief module defining constants related to heap manager.
*/
namespace heap_manager {
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

    /// min interval between gc calls.
    constexpr std::chrono::milliseconds MIN_GC_INTERVAL{MIN_INTERVAL};

    /// periodic interval of the gc calls.
    constexpr std::chrono::milliseconds PERIODIC_GC_INTERVAL{PERIODIC_INTERVAL};
};

#endif