#ifndef REGISTER_ROOT_HPP
#define REGISTER_ROOT_HPP

#include <mutex>
#include <cstdint>

#include "../common/header/header.hpp"
#include "../common/root-set/root-set-base.hpp"
#include "../common/gc/gc-visitor.hpp"

/**
 * @class register_root
 * @brief defines the structure of the register variable in the root-set table.
 * Inherits from the root_set_base.
*/
class register_root final : public root_set_base {
private:
    /// used for register synchronization.
    mutable std::mutex register_mutex;
    
    /// id of the register.
    const uint64_t register_id;

    /// pointer to a header of the register variable on the heap.
    header* register_variable;


    /**
     * @brief getter for the variable.
     * @warning must be called when lock is held already.
     * @returns pointer to a header of the variable.
    */
    header* get_register_variable_unlocked() noexcept;

    /// allowing mark-sweep gc to access getter for the variable.
    friend class ms_garbage_collector;

public:
    /**
     * @brief creates the instance of the register variable.
     * @param reg_id - id of the register.
     * @param var_ptr - pointer to a header of the register variable on the heap.
    */
    register_root(uint64_t reg_id, header* var_ptr);

    /**
     * @brief getter for the register id.
     * @returns id of the register.
    */
    uint64_t get_register_id() const noexcept;

    /**
     * @brief setter for the register variable
     * @param var_ptr - pointer to a header of the variable on the heap.
    */
    void set_register_variable(header* var_ptr) noexcept;

    /**
     * @brief accepts the gc visitor.
     * @param visitor - reference to a gc visitor.
     * Calls marking on the gc visitor for register variable element.
    */
    virtual void accept(gc_visitor& visitor) noexcept override final;

};

#endif