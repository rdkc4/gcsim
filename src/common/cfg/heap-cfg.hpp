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
    constexpr uint32_t SEGMENT_SIZE{ 16 * 1024 * 1024 }; //<16MB

    /// alignment in the segment of the heap.
    constexpr uint32_t SEGMENT_ALIGNMENT{16};

    /// number of small object segments.
    constexpr size_t SMALL_OBJECT_SEGMENTS{4};

    /// number of medium object segments.
    constexpr size_t MEDIUM_OBJECT_SEGMENTS{2};

    /// number of large object segments.
    constexpr size_t LARGE_OBJECT_SEGMENTS{2};

    /// total number of segments.
    constexpr size_t TOTAL_SEGMENTS{ 
        SMALL_OBJECT_SEGMENTS + 
        MEDIUM_OBJECT_SEGMENTS + 
        LARGE_OBJECT_SEGMENTS
    };
    
};

// segment assertions.
static_assert(cfg::heap::SEGMENT_ALIGNMENT > 0, "Segment alignment must be positive");
static_assert(cfg::heap::SEGMENT_SIZE > 0, "Size of the segment must be positive");
static_assert(cfg::heap::SMALL_OBJECT_SEGMENTS, "Number of small object segments must be positive");
static_assert(cfg::heap::MEDIUM_OBJECT_SEGMENTS, "Number of medium object segments must be positive");
static_assert(cfg::heap::LARGE_OBJECT_SEGMENTS, "Number of large object segments must be positive");
static_assert(cfg::heap::TOTAL_SEGMENTS, "Total number of segments must be positive");

#endif