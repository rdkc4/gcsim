#ifndef SHARED_REGISTER_SPACE_HPP
#define SHARED_REGISTER_SPACE_HPP

#include <mutex>

#include "../common/root-set/root-set-base.hpp"
#include "../common/header/header.hpp"
#include "../common/stack/indexed-stack.hpp"
#include "../heap-manager/heap-manager.hpp"

/**
 * @class shared_register_space
 * @brief space for registers accessible from all threads.
 * inherits from root_set_base.
*/
class shared_register_space final : public root_set_base {
private:
    /// mutex for concurrent access to registers.
    std::mutex register_space_mtx;

    /// id of the registers for root-set-table.
    const uint64_t register_id;

    /// reference to a heap manager.
    heap_manager& heap_manager_ref;

    /// indexed stack of registers.
    indexed_stack<header*> registers;

public:
    /**
     * @brief creates the instance of the shared register space.
     * @param heap_mngr - reference to a heap manager.
     * @param id - register id for root-set-table.
     * initializes all registers as nullptrs.
    */
    shared_register_space(heap_manager& heap_mngr, uint64_t id);

    /**
     * @brief deletes the instance of the shared register space.
    */
    ~shared_register_space();
    
    /**
     * @brief updates the value of the register at the random index.
     * @param heap_ptr - pointer to the new header on the heap.
    */
    void reallocate_random(header* heap_ptr);

    /**
     * @brief performs an operation on each register root.
     * @tparam Fn - type of the function.
     * @param fn - function that is performed on register roots.
    */
    template<typename Fn>
    void for_each(Fn&& fn){
        for (size_t i = 0; i < registers.get_size(); ++i){
            auto& entry{ registers[i] };

            if(entry){
                fn(entry);
            }
        }
    }

    /**
     * @brief accepts the gc visitor.
     * @param visitor - reference to a gc visitor.
     * calls marking on the gc visitor for register roots.
    */
    void accept(gc_visitor& visitor) noexcept override final;

    /**
     * @brief accepts the gc forwarder.
     * @param forwarder - reference to a gc forwarder.
     * calls forward on the gc forwarder for register roots.
    */
    void accept_forward(gc_forwarder& forwarder) noexcept override final;

};

#endif