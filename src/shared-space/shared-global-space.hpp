#ifndef SHARED_GLOBAL_SPACE_HPP
#define SHARED_GLOBAL_SPACE_HPP

#include <memory>
#include <mutex>

#include "../common/header/header.hpp"
#include "../common/indexed-stack/indexed-stack.hpp"
#include "../root-set-table/global-root.hpp"
#include "../heap-manager/heap-manager.hpp"

/**
 * @class shared_global_space
 * @brief space for global variables accessible from all threads.
*/
class shared_global_space {
private:
    /// mutex for concurrent access to global variables.
    std::mutex global_space_mtx;

    /// reference to a heap manager where global variables point to.
    heap_manager& heap_manager_ref;

    /// indexed stack of pointers to global variables.
    indexed_stack<std::unique_ptr<global_root>> global_variables;

public:
    /**
     * @brief creates the instance of the shared_global_space
    */
    shared_global_space(heap_manager& heap_mngr);

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

};

#endif