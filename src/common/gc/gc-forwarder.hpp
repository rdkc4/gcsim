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
     * @brief creates the instance of the gc_forwarder object.
    */
    gc_forwarder() = default;

    /**
     * @brief deletes the gc_forwarder object
    */
    virtual ~gc_forwarder() = default;

    /// deleted copy constructor.
    gc_forwarder(const gc_forwarder&) = delete;

    /// deleted copy assignment operator.
    gc_forwarder& operator=(const gc_forwarder&) = delete;

    /// deleted move constructor.
    gc_forwarder(gc_forwarder&&) noexcept = delete;

    /// deleted move assignment operator.
    gc_forwarder& operator=(gc_forwarder&&) noexcept = delete;

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