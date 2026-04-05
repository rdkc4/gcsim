#ifndef GC_FORWARDER_HPP
#define GC_FORWARDER_HPP

class thread_local_stack;
class shared_global_space;
class shared_register_space;

/**
 * @class gc_forwarder
 * @brief interface for gc's forwarding phase.
*/
class gc_forwarder {
public:
    /**
     * @brief deletes the gc_forwarder object
    */
    virtual ~gc_forwarder() = default;

    /**
     * @brief virtual function for forwarding elements of the thread local stack.
     * @param stack - reference to a thread local stack.
    */
    virtual void forward(thread_local_stack& stack) = 0;

    /**
     * @brief virtual function for marking the global roots.
     * @param global - reference to a global space.
    */
    virtual void forward(shared_global_space& global) = 0;

    /**
     * @brief virtual function for marking the register roots.
     * @param reg - reference to a register space.
    */
    virtual void forward(shared_register_space& reg) = 0;

};

#endif