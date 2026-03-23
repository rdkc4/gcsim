#ifndef MC_GARBAGE_COLLECTOR_HPP
#define MC_GARBAGE_COLLECTOR_HPP

#include <cstddef>

#include "garbage-collector.hpp"
#include "../common/gc/gc-forwarder.hpp"
#include "../root-set-table/root-set-table.hpp"
#include "../root-set-table/thread-local-stack.hpp"
#include "../root-set-table/global-root.hpp"
#include "../root-set-table/register-root.hpp"
#include "../common/segment/segment-info.hpp"
#include "../common/segment/segment.hpp"
#include "../heap/heap.hpp"

/**
 * @class mc_garbage_collector
 * @brief implementation of the mark-compact garbage collector.
 * inherits from garbage_collector and gc_forwarder.
*/
class mc_garbage_collector final : public garbage_collector, public gc_forwarder {
private:
    /**
     * @brief marks all objects that are reachable from the root-set-table.
     * @param root_set - reference to a root-set-table
    */
    void mark(root_set_table& root_set) noexcept;

    /**
     * @brief computes and stores forwarding addresses for all marked objects in a segment.
     * @param seg - reference to a segment.
    */
    void compute_forwarding_addresses_segment(segment& seg) noexcept;

    /**
     * @brief computes forwarding addresses for all marked objects across all heap segments.
     * @param heap_memory - reference to a heap.
    */
    void compute_forwarding_addresses(heap& heap_memory) noexcept;

    /**
     * @brief updates roots to point to new addresses.
     * @param root_set - reference to the root set table.
    */
    void update_roots(root_set_table& root_set) noexcept;

    /**
     * @brief compacts a single segment by sliding all marked objects towards the start.
     * @param seg - reference to a segment.
     * @param seg_info - pointer to information about the segment.
    */
    void compact_segment(segment& seg, segment_info* seg_info) noexcept;

    /**
     * @brief compacts all heap segments, relocating marked objects and reclaiming free space.
     * @param heap_memory - reference to a heap.
     * @param free_memory_table - reference to a free memory table.
    */
    void compact(heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept;

public:
    /**
     * @brief creates the instance of the mark-and-sweep garbage collector.
     * @param thread_count - number of threads in a thread pool, defaults to 1.
    */
    mc_garbage_collector(size_t thread_count = 1);

    /**
     * @brief deletes the instance of the mark-and-sweep garbage collector.
    */
    ~mc_garbage_collector() override final = default;

    /**
     * @brief collects the garbage from the heap using mark-and-sweep algorithm.
     * @param root_set - reference to a root-set-table.
     * @param heap_memory - reference to a heap.
     * @param free_memory_table - reference to a free-memory-table.
    */
    void collect(root_set_table& root_set, heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept override;
  
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

    /**
     * @brief forwards the objects on the stack to new addresses.
     * @param stack - reference to a thread local stack.
    */
    virtual void forward(thread_local_stack& stack) override final;

    /**
     * @brief forwards the global root new addresses
     * @param global - reference to a global variable.
    */
    virtual void forward(global_root& global) override final;
    
    /**
     * @brief forwards the register root new addresses
     * @param reg - reference to a register variable.
    */
    virtual void forward(register_root& reg) override final;

};

#endif