#ifndef THREAD_LOCAL_STACK_HPP
#define THREAD_LOCAL_STACK_HPP

#include "../common/cfg/structs-cfg.hpp"
#include "../common/stack/indexed-stack.hpp"
#include "../common/stack/fixed-stack.hpp"
#include "../common/header/header.hpp"
#include "../common/root-set/root-set-base.hpp"
#include "../common/root-set/thread-local-stack-entry.hpp"
#include "../heap-manager/heap-manager.hpp"
#include "../common/gc/gc-visitor.hpp"
#include "../common/gc/gc-forwarder.hpp"

/**
 * @class thread_local_stack
 * @brief local memory of a thread.
*/
class thread_local_stack final : public root_set_base {
private:
    /// id of the last pushed scope.
    size_t scope;

    /// tls id for root-set-table.
    const uint64_t tls_id;

    /// reference to a heap manager.
    heap_manager& heap_manager_ref;

    /// stack of initialized variables.
    indexed_stack<thread_local_stack_entry> tls;

    /// stack of temporary objects.
    fixed_stack<header*, cfg::structs::fixed_stack::MAX_STACK_CAPACITY> temporaries;

    /**
     * @brief performs an operation on each variable on the tls.
     * @tparam Fn - type of the function.
     * @param fn - function that is performed on elements.
    */
    template<typename Fn>
    void for_each_variable(Fn&& fn) {
        for(size_t i{0}; i < tls.get_size(); ++i){
            auto& entry{ tls[i] };

            if(entry.ref_to){
                fn(entry.ref_to);
            }
        }
    }

    /**
     * @brief performs an operation on each temporary variable.
     * @tparam Fn - type of the function.
     * @param fn - function that is performed on elements.
    */
    template<typename Fn>
    void for_each_temp_variable(Fn&& fn) {
        for(size_t i{0}; i < temporaries.get_size(); ++i){
            auto& temp{ temporaries[i] };

            if(temp){
                fn(temp);
            }
        }
    }

public:
    /**
     * @brief creates the instance of the thread_local_stack.
     * @param heap_mngr - reference to a heap manager.
     * @param id - id of the tls for root-set-table.
     * @details scope defaults to 1.
    */
    thread_local_stack(heap_manager& heap_mngr, uint64_t id);

    /**
     * @brief deletes the thread_local_stack.
     * @details frees final scope if not freed manually.
    */
    ~thread_local_stack();

    /// deleted copy constructor.
    thread_local_stack(const thread_local_stack&) = delete;

    /// deleted assignment operator.
    thread_local_stack& operator=(const thread_local_stack&) = delete;

    /// deleted move constructor.
    thread_local_stack(thread_local_stack&& other) = delete;

    /// deleted move assignment operator.
    thread_local_stack& operator=(thread_local_stack&& other) = delete;

    /**
     * @brief initializes new variable.
     * @param heap_ptr - pointer to the value of the variable on the heap.
    */
    void init(header* heap_ptr = nullptr);

    /**
     * @brief assigns new value to a variable.
     * @param index - index on the indexed stack.
     * @param new_ref_to - pointer to a new value on the heap.
    */
    void reassign_ref(size_t index, header* new_ref_to);

    /**
     * @brief removes the reference to a value on the heap.
     * @param index - index on the indexed stack.
    */
    void remove_ref(size_t index);

    /**
     * @brief pushes temporary root onto stack.
     * @param heap_ptr - pointer to an object on the heap.
    */
    header** push_temp(header* heap_ptr);

    /**
     * @brief pops the temporary root from the stack.
    */
    void pop_temp();

    /**
     * @brief simulates entering new scope.
     * @note simulation purposes.
    */
    void push_scope() noexcept;

    /**
     * @brief simulates exiting scope.
     * @param destr - flag if pop_scope is called by destructor, defaults to false.
     * @note simulation purposes.
    */
    void pop_scope(bool destr = false);

    /**
     * @brief performs an operation on all roots owned by tls.
     * @tparam Fn - type of the function.
     * @param fn - function that is performed on each root.
    */
    template<typename Fn>
    void for_each(Fn&& fn){
        for_each_variable(fn);
        for_each_temp_variable(fn);
    }

    /**
     * @brief accepts the gc visitor.
     * @param visitor - reference to a gc visitor.
     * Calls marking on the gc visitor for thread-local elements.
    */
    void accept(gc_visitor& visitor) noexcept override final;

    /**
     * @brief accepts the gc forwarder.
     * @param forwarder - reference to a gc forwarder.
     * Calls forward on the gc forwarder for thread-local elements.
    */
    void accept_forward(gc_forwarder& forwarder) noexcept override final;

};

#endif