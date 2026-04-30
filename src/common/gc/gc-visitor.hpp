#ifndef GC_VISITOR_HPP
#define GC_VISITOR_HPP

class thread_local_stack;
class shared_global_space;
class shared_register_space;

/**
 * @class gc_visitor
 * @brief interface for GC's marking phase.
*/
class gc_visitor {
public:
    /**
     * @brief creates the instance of gc_visitor object.
    */
    gc_visitor() = default;

    /**
     * @brief deletes the gc_visitor object.
    */
    virtual ~gc_visitor() = default;

    /// deleted copy constructor.
    gc_visitor(const gc_visitor&) = delete;

    /// deleted copy assignment operator.
    gc_visitor& operator=(const gc_visitor&) = delete;

    /// deleted move constructor.
    gc_visitor(gc_visitor&&) noexcept = delete;

    /// deleted move assignment operator.
    gc_visitor& operator=(gc_visitor&&) noexcept = delete;
    
    /**
     * @brief virtual function for marking elements of the thread local stack.
     * @param stack - reference to a thread local stack.
    */
    virtual void visit(thread_local_stack& stack) = 0;

    /**
     * @brief virtual function for marking the global roots.
     * @param global - reference to a global space.
    */
    virtual void visit(shared_global_space& global) = 0;

    /**
     * @brief virtual function for marking the register roots.
     * @param reg - reference to a register space.
    */
    virtual void visit(shared_register_space& reg) = 0;
};

#endif