#ifndef ALLOCATOR_DEFS_HPP
#define ALLOCATOR_DEFS_HPP


/**
 * @enum simulation_mode
 * @brief defines the type of the simulation.
*/
enum class simulation_mode { 
    stress,     //< predefined work per thread.
    relaxed     //< shared work across threads.
};

/**
 * @enum simulation_operation
 * @brief operation performed by simulator.
*/
enum class simulation_operation { 
    tls_alloc = 0,           //< allocation of a thread local object.
    global_alloc = 1,        //< allocation of a global object.
    global_realloc = 2,      //< reallocation of a global object.
    register_realloc = 3     //< reallocation of a register object.
};

/**
 * @enum garbage_collector_type
 * @brief type of garbage collector used in simulation.
*/
enum class garbage_collector_type {
    mark_sweep,     //< mark & sweep garbage collection.
    mark_compact    //< mark & compact garbage collection.
};

#endif