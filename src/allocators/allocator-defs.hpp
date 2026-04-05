#ifndef ALLOCATOR_DEFS_HPP
#define ALLOCATOR_DEFS_HPP


/**
 * @enum simulation_mode
 * @brief defines the type of the simulation.
*/
enum class simulation_mode { 
    stress, 
    relaxed 
};

/**
 * @enum simulation_operation
 * @brief operation performed by simulator.
*/
enum class simulation_operation { 
    tls_alloc = 0, 
    global_alloc = 1, 
    global_realloc = 2, 
    register_realloc = 3 
};

/**
 * @enum garbage_collector_type
 * @brief type of garbage collector used in simulation.
*/
enum class garbage_collector_type {
    mark_sweep,
    mark_compact
};

#endif