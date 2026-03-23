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

    /**
     * @namespace rng::sim::shared_space
     * @brief module defining random number generation for shared spaces.
    */
    namespace shared_space {
        /**
         * @brief generates the random index of the shared_space.
         * @returns index of the shared space.
         * @warning n is greater than 0 (n > 0).
        */
        size_t generate_index(size_t n);
    };

};

// object distribution assertions.
static_assert(
    rng::sim::MAX_OBJECT_DISTRIBUTION > rng::sim::MIN_OBJECT_DISTRIBUTION, 
    "Max object distribution number must be greater than min object distribution number"
);

// small object assertions.
static_assert(rng::sim::MIN_SMALL_OBJECT_DISTRIBUTION > 0, "Min small object distribution number must be positive");
static_assert(
    rng::sim::MAX_SMALL_OBJECT_DISTRIBUTION > rng::sim::MIN_SMALL_OBJECT_DISTRIBUTION, 
    "Max small object distribution number must be greater than min small object distribution number"
);

// medium object assertions.
static_assert(
    rng::sim::MIN_MEDIUM_OBJECT_DISTRIBUTION > rng::sim::MAX_SMALL_OBJECT_DISTRIBUTION, 
    "Min medium object distribution number must be greater than max small object distribution number"
);
static_assert(
    rng::sim::MAX_MEDIUM_OBJECT_DISTRIBUTION > rng::sim::MIN_MEDIUM_OBJECT_DISTRIBUTION, 
    "Max medium object distribution number must be greater than min medium object distribution number"
);

// large object assertions.
static_assert(
    rng::sim::MIN_LARGE_OBJECT_DISTRIBUTION > rng::sim::MAX_MEDIUM_OBJECT_DISTRIBUTION, 
    "Min large object distribution number must be greater than max medium object distribution number"
);
static_assert(
    rng::sim::MAX_LARGE_OBJECT_DISTRIBUTION > rng::sim::MIN_LARGE_OBJECT_DISTRIBUTION, 
    "Max large object distribution number must be greater than min large object distribution number"
);

#endif