#ifndef MUTATOR_GUARD_HPP
#define MUTATOR_GUARD_HPP

#include "../heap-manager/heap-manager.hpp"

/**
 * @struct mutator_guard
 * @brief ensures mutator is unregistered when exiting scope.
*/
struct mutator_guard {
private:
    /// reference to a heap manager.
    heap_manager& hm;

public:
    /**
     * @brief registers the mutator.
     * @param manager - reference to a heap manager.
    */
    mutator_guard(heap_manager& manager) : hm{ manager } {
        hm.register_mutator();
    }

    /**
     * @brief unregisters the mutator.
    */
    ~mutator_guard() {
        hm.unregister_mutator();
    }

};

#endif