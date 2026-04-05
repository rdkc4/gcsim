#ifndef ROOT_GUARD_HPP
#define ROOT_GUARD_HPP

#include "thread-local-stack.hpp"

/**
 * @class root_guard
 * @brief manages the lifetime of the temporary roots.
*/
class root_guard {
private:
    /// pointer to a stack of the thread that is allocating object.
    thread_local_stack* tls{nullptr};

    /// slot of the temporary variable.
    header** slot{nullptr};

    /// flag if temporary root was pushed onto stack.
    bool active{false};

public:
    /**
     * @brief pushes the temporary root on the thread's stack.
     * @param tls - pointer to a stack of the thread that is allocating object.
     * @param obj - pointer to the header of the object on the heap.
    */
    root_guard(thread_local_stack* tls, header* obj) : tls{tls} {
        if(tls){
            slot = tls->push_temp(obj);
            active = true;
        }
    }

    /**
     * @brief pops the temporary root from the thread's stack.
    */
    ~root_guard(){
        if(active){
            tls->pop_temp();
        }
    }

    /// deleted copy constructor.
    root_guard(const root_guard&) = delete;

    /// deleted assignment operator.
    root_guard& operator=(const root_guard&) = delete;

    /**
     * @brief getter for the slot of the temporary root.
     * @returns slot where root is stored.
    */
    header** get_slot() const noexcept {
        return slot;
    }

};

#endif