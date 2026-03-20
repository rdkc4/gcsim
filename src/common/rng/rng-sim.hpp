#ifndef RNG_SIM_HPP
#define RNG_SIM_HPP

#include <cstdint>

#include "../cfg/heap-manager-cfg.hpp"

/**
 * @namespace rng::sim
 * @brief module defining random number generation for simulation.
*/
namespace rng::sim {
    /// min generated number for object category distribution.
    constexpr size_t MIN_OBJECT_DISTRIBUTION = 1;

    /// max generated number for object category distribution.
    constexpr size_t MAX_OBJECT_DISTRIBUTION = 100;

    /// min generated number for small object distribution.
    constexpr size_t MIN_SMALL_OBJECT_DISTRIBUTION = 1;

    /// max generated number for small object distribution.
    constexpr size_t MAX_SMALL_OBJECT_DISTRIBUTION = cfg::heap_manager::SMALL_OBJECT_THRESHOLD;

    /// min generated number for medium object distribution.
    constexpr size_t MIN_MEDIUM_OBJECT_DISTRIBUTION = cfg::heap_manager::SMALL_OBJECT_THRESHOLD + 1;

    /// max generated number for medium object distribution.
    constexpr size_t MAX_MEDIUM_OBJECT_DISTRIBUTION = cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD;

    /// min generated number for large object distribution.
    constexpr size_t MIN_LARGE_OBJECT_DISTRIBUTION = cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD + 1;

    /// max generated number for large object distribution.
    constexpr size_t MAX_LARGE_OBJECT_DISTRIBUTION = cfg::heap_manager::LARGE_OBJECT_THRESHOLD;

    /** 
     * @brief generates the size of the object.
     * @returns amount of bytes object needs for allocation.
    */
    uint32_t generate_object_size();

};

#endif