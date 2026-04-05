#ifndef ALLOCATORS_HPP
#define ALLOCATORS_HPP

#include <latch>
#include <utility>
#include <atomic>
#include <cstddef>

#include "../heap-manager/heap-manager.hpp"
#include "../common/thread-pool/thread-pool.hpp"
#include "../root-set-table/thread-local-stack.hpp"
#include "../root-set-table/shared-register-space.hpp"
#include "../root-set-table/shared-global-space.hpp"
#include "../common/cfg/sim-cfg.hpp"
#include "../diagnostics/diagnostics.hpp"

/**
 * @enum simulation_mode
 * @brief defines the type of the simulation.
*/
enum class simulation_mode { stress, relaxed };

/**
 * @enum simulation_operation
 * @brief operation performed by simulator.
*/
enum class simulation_operation { tls_alloc = 0, global_alloc = 1, global_realloc = 2, register_realloc = 3 };

/**
 * @class allocators
 * @brief simulates the allocations on the heap.
*/
class allocators {
private:
    /// reference to a heap manager used by allocators.
    heap_manager& heap_manager_ref;

    /// allocators thread pool.
    thread_pool alloc_thread_pool;

    /// shared space for registers.
    shared_register_space register_space;

    /// shared space for global variables.
    shared_global_space global_space;

    /// total number of allocations.
    std::atomic<size_t> total_allocs{};

    /// total number of unsucessful allocations.
    std::atomic<size_t> total_failed_allocs{};

    /// total number of a single thread's allocations.
    static thread_local size_t per_thread_allocs;

    /// total number of a single thread's unsuccessful allocations.
    static thread_local size_t per_thread_failed_allocs;

    /**
     * @brief simulates allocation of a thread, stress mode.
     * @param tls - pointer to a thread local stack.
     * @param scope_count - number of scopes.
     * @param operations_per_scope - number of operations per scope.
    */
    void simulate_thread_alloc(thread_local_stack* tls, size_t scope_count, size_t operations_per_scope);

    /**
     * @brief adds simulation to queue.
     * @tparam fn - type of the function.
     * @param simulate - simulation function.
     * @param completion_latch - synchronization for simulation.
    */
    template <typename fn>
    void enqueue_simulation(fn&& simulate, std::latch& completion_latch){
        alloc_thread_pool.enqueue([simulate = std::forward<fn>(simulate), &completion_latch]{
            simulate();
            completion_latch.count_down();
        });
    }

    /**
     * @brief getter for the name of the mode of simulation.
     * @param mode - simulation mode.
     * @returns name of the mode.
    */
    static constexpr const char* simulation_mode_name(simulation_mode mode) {
        switch (mode) {
            case simulation_mode::stress: return "stress";
            case simulation_mode::relaxed: return "relaxed";
        }
        std::unreachable();
    }

    /**
     * @brief getter for tls scope count.
     * @param mode - mode of the simulation.
     * @returns number of scopes for tls.
    */
    static constexpr size_t tls_scope_count(simulation_mode mode){
        switch(mode){
            case simulation_mode::stress: return cfg::sim::TLS_SCOPE_COUNT_STRESS;
            case simulation_mode::relaxed: return cfg::sim::TLS_SCOPE_COUNT_RELAXED;
        }
        std::unreachable();
    }

    /**
     * @brief getter for tls allocation count per scope.
     * @param mode - mode of the simulation.
     * @returns number of allocation for tls per scope.
    */
    static constexpr size_t tls_allocs_per_scope(simulation_mode mode){
        switch(mode){
            case simulation_mode::stress: return cfg::sim::TLS_ALLOC_STRESS_THRESHOLD_PER_SCOPE;
            case simulation_mode::relaxed: return cfg::sim::TLS_ALLOC_RELAXED_THRESHOLD_PER_SCOPE;
        }
        std::unreachable();
    }

    /**
     * @brief allocates an object on the heap.
     * @param tls - pointer to a stack of the thread that wants to allocate object.
     * @returns pointer to the header of the object.
    */
    header* allocate_object(thread_local_stack* tls);

public:
    /**
     * @brief creates the instance of the allocators.
     * @param heap_manager_ref - reference to a heap manager.
     * @param thread_count - number of thread in allocators thread pool.
    */
    allocators(heap_manager& heap_manager_ref, size_t thread_count);

    /**
     * @brief deletes the instance of the allocators.
    */
    ~allocators() = default;

    /**
     * @brief starts the allocation simulation, stress mode.
     * @param tls_count - number of threads that are allocating objects.
     * @param mode - mode of the simulation.
    */
    diagnostics simulate_alloc(size_t tls_count, simulation_mode mode);

};

#endif