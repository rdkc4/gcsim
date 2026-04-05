#ifndef SHARED_GLOBAL_SPACE_HPP
#define SHARED_GLOBAL_SPACE_HPP

#include <mutex>

#include "../common/root-set/root-set-base.hpp"
#include "../common/header/header.hpp"
#include "../common/stack/indexed-stack.hpp"
#include "../common/gc/gc-visitor.hpp"
#include "../common/gc/gc-forwarder.hpp"
#include "../heap-manager/heap-manager.hpp"

/**
 * @class shared_global_space
 * @brief space for global variables accessible from all threads.
 * inherits from root_set_base.
*/
class shared_global_space final : public root_set_base {
private:
    /// mutex for concurrent access to global variables.
    std::mutex global_space_mtx;

    /// id of the global space.
    uint64_t root_id;

    /// reference to a heap manager where global variables point to.
    heap_manager& heap_manager_ref;

    /// indexed stack of pointers to global variables.
    indexed_stack<header*> global_variables;

    /**
     * @brief allocates new global variable.
     * @param heap_ptr - pointer to the header on the heap.
    */
    void allocate_internal(header* heap_ptr);

public:
    /**
     * @brief creates the instance of the shared_global_space
     * @param heap_mngr - reference to a heap manager.
     * @param id - id of the global space for root-set-table.
    */
    shared_global_space(heap_manager& heap_mngr, uint64_t id);

    /**
     * @brief destroys the instance of the shared_global_space.
     * @details pops the variables from global_variables.
     * removes their identificator from heap manager's root_set_table.
    */
    ~shared_global_space();
    
    /**
     * @brief allocates new global variable.
     * @param heap_ptr - pointer to the header on the heap.
    */
    void allocate(header* heap_ptr);

    /**
     * @brief updates the value of the global variable at the random index.
     * @param heap_ptr - pointer to the new header on the heap.
    */
    void reallocate_random(header* heap_ptr);

    /**
     * @brief performs an operation on each global root.
     * @tparam Fn - type of the function.
     * @param fn - function that is performed on global roots.
    */
    template<typename Fn>
    void for_each(Fn&& fn){
        for(size_t i = 0; i < global_variables.get_size(); ++i){
            auto& entry{ global_variables[i] };

            if(entry){
                fn(entry);
            }
        }
    }

    /**
     * @brief accepts the gc visitor.
     * @param visitor - reference to a gc visitor.
     * calls marking on the gc visitor for global objects.
    */
    void accept(gc_visitor& visitor) noexcept override final;

    /**
     * @brief accepts the gc forwarder.
     * @param forwarder - reference to a gc forwarder.
     * calls forward on the gc forwarder for global objects.
    */
    void accept_forward(gc_forwarder& forwarder) noexcept override final;

};

#endif