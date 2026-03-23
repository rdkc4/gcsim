#ifndef GARBAGE_COLLECTOR_HPP
#define GARBAGE_COLLECTOR_HPP

#include "../common/gc/gc-visitor.hpp"
#include "../common/thread-pool/thread-pool.hpp"
#include "../root-set-table/root-set-table.hpp"
#include "../segment-free-memory-table/segment-free-memory-table.hpp"
#include "../heap/heap.hpp"

/**
 * @class garbage_collector
 * @brief base class for garbage collectors.
 * inherits from gc_visitor.
*/
class garbage_collector : public gc_visitor {
protected:
    /// thread pool for concurrent gc phases.
    thread_pool gc_thread_pool;

public:
    /**
     * @brief creates the instance of the garbage collector.
    */
    garbage_collector(size_t thread_count = 1) : gc_thread_pool{ thread_count } {}

    /**
     * @brief deletes the instance of the garbage collector.
    */
    virtual ~garbage_collector() = default;

    /**
     * @brief collects the garbage from the heap.
     * @param root_set - reference to a root-set-table.
     * @param heap_memory - reference to a heap.
    */
    virtual void collect(root_set_table& root_set, heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept = 0;
};

#endif