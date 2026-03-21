#ifndef THREAD_LOCAL_STACK_ENTRY_HPP
#define THREAD_LOCAL_STACK_ENTRY_HPP

#include <cstddef>
#include <cstdint>

#include "../header/header.hpp"

/**
 * @struct thread_local_stack_entry
 * @brief element of the thread_local_stack.
*/
struct thread_local_stack_entry {
    /// pointer to the header of the block containing the data of the variable on the heap.
    header* ref_to;

    /// id of the scope in which variable was initialized (simulation purposes).
    size_t scope;

    /// id of the variable.
    uint64_t variable_id;

};

#endif