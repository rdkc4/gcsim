#ifndef SHARED_REGISTER_SPACE_HPP
#define SHARED_REGISTER_SPACE_HPP

#include <memory>
#include <mutex>

#include "../common/header/header.hpp"
#include "../common/indexed-stack/indexed-stack.hpp"
#include "../root-set-table/register-root.hpp"
#include "../heap-manager/heap-manager.hpp"

/**
 * @class shared_register_space
 * @brief space for registers accessible from all threads.
*/
class shared_register_space {
private:
    /// mutex for concurrent access to registers.
    std::mutex register_space_mtx;

    /// reference to a heap manager where registers point to.
    heap_manager& heap_manager_ref;

    /// indexed stack of registers.
    indexed_stack<std::unique_ptr<register_root>> registers;

public:
    /**
     * @brief creates the instance of the shared register space.
     * initializes all registers as nullptrs.
    */
    shared_register_space(heap_manager& heap_mngr);

    /**
     * @brief deletes the instance of the shared register space.
     * removes the register roots from the root-set table.
    */
    ~shared_register_space();
    
    /**
     * @brief updates the value of the register at the random index.
     * @param heap_ptr - pointer to the new header on the heap.
    */
    void reallocate_random(header* heap_ptr);

};

#endif