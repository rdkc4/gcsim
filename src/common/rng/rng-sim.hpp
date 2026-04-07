#ifndef RNG_SIM_HPP
#define RNG_SIM_HPP

#include <cstdint>
#include <cstddef>

#include "../cfg/heap-manager-cfg.hpp"

/**
 * @namespace rng::sim
 * @brief module defining random number generation for simulation.
*/
namespace rng::sim {
    /// probability for object to be small.
    constexpr uint32_t SMALL_OBJECT_PROBABILITY{90};

    /// probability for object to be medium. 
    constexpr uint32_t MEDIUM_OBJECT_PROBABILITY{8};

    /// probability for object to be large.
    constexpr uint32_t LARGE_OBJECT_PROBABILITY{2};

    /**
     * @struct object_probability_t
     * @brief struct defining the thresholds for object size probability.
    */
    struct object_probability_t {
        /// small object threshold.
        const uint32_t small_obj_p{ SMALL_OBJECT_PROBABILITY };

        /// medium object threshold.
        const uint32_t medium_obj_p{ 
            SMALL_OBJECT_PROBABILITY + 
            MEDIUM_OBJECT_PROBABILITY 
        };

        /// large object threshold.
        const uint32_t large_obj_p{ 
            SMALL_OBJECT_PROBABILITY + 
            MEDIUM_OBJECT_PROBABILITY + 
            LARGE_OBJECT_PROBABILITY
        };
    };


    /// min generated number for small object distribution.
    constexpr uint32_t MIN_SMALL_OBJECT_DISTRIBUTION{1};

    /// max generated number for small object distribution.
    constexpr uint32_t MAX_SMALL_OBJECT_DISTRIBUTION{ 
        cfg::heap_manager::SMALL_OBJECT_THRESHOLD 
    };

    /// min generated number for medium object distribution.
    constexpr uint32_t MIN_MEDIUM_OBJECT_DISTRIBUTION{ 
        cfg::heap_manager::SMALL_OBJECT_THRESHOLD + 1
    };

    /// max generated number for medium object distribution.
    constexpr uint32_t MAX_MEDIUM_OBJECT_DISTRIBUTION{ 
        cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD 
    };

    /// min generated number for large object distribution.
    constexpr uint32_t MIN_LARGE_OBJECT_DISTRIBUTION{ 
        cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD + 1 
    };

    /// max generated number for large object distribution.
    constexpr uint32_t MAX_LARGE_OBJECT_DISTRIBUTION{ 
        cfg::heap_manager::LARGE_OBJECT_THRESHOLD
    };


    /// probability of object having no references.
    constexpr uint64_t NO_REF_PROBABILITY{80};

    /// probability of object having one reference.
    constexpr uint64_t ONE_REF_PROBABILITY{15};

    /// probability of object having two references.
    constexpr uint64_t TWO_REF_PROBABILITY{5};

    /**
     * @struct ref_probability_t
     * @brief struct defining the thresholds for ref count probability.
    */
    struct ref_probability_t {
        /// no ref threshold.
        const uint32_t no_ref_p{ NO_REF_PROBABILITY };

        /// one ref threshold.
        const uint32_t one_ref_p{ 
            NO_REF_PROBABILITY + 
            ONE_REF_PROBABILITY 
        };

        /// two refs threshold.
        const uint32_t two_ref_p{ 
            NO_REF_PROBABILITY + 
            ONE_REF_PROBABILITY + 
            TWO_REF_PROBABILITY
        };
    };

    /// probability that operation will be tls allocation.
    constexpr uint32_t TLS_ALLOC_PROBABILITY{90};

    /// probability that operation will be global allocation.
    constexpr uint32_t GLOBAL_ALLOC_PROBABILITY{4};

    /// probability that operation will be global reallocation.
    constexpr uint32_t GLOBAL_REALLOC_PROBABILITY{2};

    /// probability that operation will be register reallocation.
    constexpr uint32_t REGISTER_REALLOC_PROBABILITY{4};

    /**
     * @struct operation_probability_t
     * @brief struct defining the thresholds for operation probability.
    */
    struct operation_probability_t {
        /// tls allocation threshold.
        const uint32_t tls_alloc_p{ TLS_ALLOC_PROBABILITY };

        /// global allocation threshold.
        const uint32_t global_alloc_p{ 
            TLS_ALLOC_PROBABILITY + 
            GLOBAL_ALLOC_PROBABILITY 
        };

        /// global reallocation threshold.
        const uint32_t global_realloc_p{ 
            TLS_ALLOC_PROBABILITY + 
            GLOBAL_ALLOC_PROBABILITY + 
            GLOBAL_REALLOC_PROBABILITY
        };

        /// register reallocation threshold.
        const uint32_t register_realloc_p{
            TLS_ALLOC_PROBABILITY + 
            GLOBAL_ALLOC_PROBABILITY + 
            GLOBAL_REALLOC_PROBABILITY +
            REGISTER_REALLOC_PROBABILITY
        };
    };


    /** 
     * @brief generates the size of the object.
     * @returns amount of bytes object needs for allocation.
    */
    uint32_t generate_object_size();

    /**
     * @brief generates the number of references.
     * @returns number of references owned by object.
    */
    uint64_t generate_reference_count();

    /**
     * @brief generates the random operation number.
     * @return number that represents the simulation operation.
    */
    uint32_t generate_simulation_operation();

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