#ifndef STRUCTS_CFG_HPP
#define STRUCTS_CFG_HPP

#include <cstddef>

/**
 * @namespace cfg::structs
 * @brief module defining constants related to data structures.
*/
namespace cfg::structs {

    /**
     * @namespace cfg::structs::hmap
     * @brief module defining constants related to hash map.
    */
    namespace hash_map {
        /// default number of buckets in a hash map.
        constexpr size_t DEFAULT_MAP_CAPACITY{8};

        /// max load factor for resizing of the hash map.
        constexpr double MAX_LOAD_FACTOR{0.75};
    };

    /**
     * @namespace cfg::structs::indexed_stack
     * @brief module defining constants related to hash map.
    */
    namespace indexed_stack {
        /// default capacity of the indexed stack.
        constexpr size_t DEFAULT_STACK_CAPACITY{8};
    };

    namespace fixed_stack {
        /// maximum capcity of the fixed stack.
        constexpr size_t MAX_STACK_CAPACITY{256};
    };

};

// data structure assertions.
static_assert(cfg::structs::hash_map::DEFAULT_MAP_CAPACITY > 0, "Default map capacity must be positive");
static_assert(cfg::structs::indexed_stack::DEFAULT_STACK_CAPACITY > 0, "Default stack capacity must be positive");
static_assert(cfg::structs::fixed_stack::MAX_STACK_CAPACITY > 0, "Max fixed stack capacity must be positive");

#endif