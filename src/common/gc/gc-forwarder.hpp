#ifndef GC_FORWARDER_HPP
#define GC_FORWARDER_HPP

class thread_local_stack;
class global_root;
class register_root;

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
     * @brief virtual function for forwarding the global root.
     * @param global - reference to a global variable.
    */
    virtual void forward(global_root& global) = 0;
    
    /**
     * @brief virtual function for forwarding the register root
     * @param reg - reference to a register variable.
    */
    virtual void forward(register_root& reg) = 0;

};

#endif