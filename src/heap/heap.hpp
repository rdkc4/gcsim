#ifndef HEAP_HPP
#define HEAP_HPP

#include <cstddef>

#include "../common/segment/segment.hpp"
#include "../common/cfg/heap-cfg.hpp"

/**
 * @class heap
 * @brief implementation of the segmented heap.
*/
class heap {
private:
    /// segments for small object allocation.
    segment small_object_segments[cfg::heap::SMALL_OBJECT_SEGMENTS];
    
    /// segments for medium object allocation.
    segment medium_object_segments[cfg::heap::MEDIUM_OBJECT_SEGMENTS];
    
    /// segments for large object allocation.
    segment large_object_segments[cfg::heap::LARGE_OBJECT_SEGMENTS];

public:
    /**
     * @brief creates the instance of the heap.
     * @details initializes all segments.
    */
    heap() = default;

    /**
     * @brief deletes the heap object.
     * @details frees all segments.
    */
    ~heap() = default;

    /// deleted copy constructor.
    heap(const heap&) = delete;
    
    /// deleted assignment operator.
    heap& operator=(const heap&) = delete;

    /// deleted move constructor.
    heap(heap&&) = delete;

    /// deleted move assignment operator.
    heap& operator=(heap&&) = delete;

    /**
     * @brief getter for small object segments.
     * @param index - index of the small object segment.
     * @returns reference to a small object segment.
    */
    segment& get_small_object_segment(size_t index);

    /**
     * @brief getter for small object segments.
     * @param index - index of the small object segment.
     * @returns const reference to a small object segment.
    */
    const segment& get_small_object_segment(size_t index) const;

    /**
     * @brief getter for medium object segments.
     * @param index - index of the medium object segment.
     * @returns reference to a medium object segment.
    */
    segment& get_medium_object_segment(size_t index);

    /**
     * @brief getter for medium object segments.
     * @param index - index of the medium object segment.
     * @returns const reference to a medium object segment.
    */
    const segment& get_medium_object_segment(size_t index) const;

    /**
     * @brief getter for large object segments.
     * @param index - index of the large object segment.
     * @returns reference to a large object segment.
    */
    segment& get_large_object_segment(size_t index);

    /**
     * @brief getter for large object segments.
     * @param index - index of the large object segment.
     * @returns const reference to a large object segment.
    */
    const segment& get_large_object_segment(size_t index) const;

};

#endif