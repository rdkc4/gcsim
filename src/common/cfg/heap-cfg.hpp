#ifndef HEAP_CFG_HPP
#define HEAP_CFG_HPP

#include <cstddef>
#include <cstdint>

/**
 * @namespace cfg::heap
 * @brief module defining constants related to heap.
*/
namespace cfg::heap {
    /// size of the single segment.
    constexpr uint32_t SEGMENT_SIZE = 16 * 1024 * 1024; //<16MB

    /// number of small object segments.
    constexpr size_t SMALL_OBJECT_SEGMENTS = 4;

    /// number of medium object segments.
    constexpr size_t MEDIUM_OBJECT_SEGMENTS = 2;

    /// number of large object segments.
    constexpr size_t LARGE_OBJECT_SEGMENTS = 2;

    /// total number of segments.
    constexpr size_t TOTAL_SEGMENTS = SMALL_OBJECT_SEGMENTS + MEDIUM_OBJECT_SEGMENTS + LARGE_OBJECT_SEGMENTS;
    
};

#endif