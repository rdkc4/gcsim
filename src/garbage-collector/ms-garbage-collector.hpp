#ifndef MS_GARBAGE_COLLECTOR_HPP
#define MS_GARBAGE_COLLECTOR_HPP

#include <cstddef>

#include "garbage-collector.hpp"
#include "../common/segment/segment-info.hpp"
#include "../common/segment/segment.hpp"
#include "../root-set-table/root-set-table.hpp"
#include "../root-set-table/thread-local-stack.hpp"
#include "../root-set-table/global-root.hpp"
#include "../root-set-table/register-root.hpp"
#include "../segment-free-memory-table/segment-free-memory-table.hpp"
#include "../heap/heap.hpp"

/**
 * @class ms_garbage_collector
 * @brief implementation of the mark-sweep garbage collector.
*/
class ms_garbage_collector final : public garbage_collector {
private:
    /**
     * @brief marks all objects that are reachable from the root-set-table.
     * @param root_set - reference to a root-set-table
    */
    void mark(root_set_table& root_set) noexcept;

    /**
     * @brief sweeps objects from a segment.
     * @param seg - reference to a segment.
    */
    void sweep_segment(segment& seg) noexcept;

    /**
     * @brief sweeps the unmarked objects from heap.
     * @param heap_memory - reference to a heap.
    */
    void sweep(heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept;

    /**
     * @brief merges free blocks in the segment.
     * @param seg - reference to segment.
     * @param seg_info - pointer to the information about the segment.
    */
    void coalesce_segment(segment& seg, segment_info* seg_info);

public:
    /**
     * @brief creates the instance of the garbage collector.
     * @param thread_count - number of threads in a thread pool, defaults to 1.
    */
    ms_garbage_collector(size_t thread_count = 1);

    /**
     * @brief deletes the instance of the garbage collector.
    */
    ~ms_garbage_collector() override final = default;

    /**
     * @brief collects the garbage from the heap.
     * @param root_set - reference to a root-set-table.
     * @param heap_memory - reference to a heap.
     * @param free_memory_table -reference to a free memory table.
    */
    void collect(root_set_table& root_set, heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept override final;

    /**
     * @brief marks the objects on the stack.
     * @param stack - reference to a thread local stack.
    */
    void visit(thread_local_stack& stack) override final;

    /**
     * @brief marks the global object.
     * @param global - reference to a global root.
    */
    void visit(global_root& global) override final;

    /**
     * @brief marks the register object.
     * @param reg - reference to a register root.
    */
    void visit(register_root& reg) override final;

};

#endif