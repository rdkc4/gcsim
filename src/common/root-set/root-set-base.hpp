#ifndef ROOT_SET_BASE_HPP
#define ROOT_SET_BASE_HPP

#include "../gc/gc-visitor.hpp"
#include "../gc/gc-forwarder.hpp"

/**
 * @class root_set_base
 * @brief parent class of root-set table entries.
*/
class root_set_base {
public:
    /**
     * @brief deletes the root_set_base object.
    */
    virtual ~root_set_base() = default;
    
    /**
     * @brief abstract method for accepting visitor.
     * @param visitor - reference to a gc visitor.
    */
    virtual void accept(gc_visitor& visitor) noexcept = 0;

    /**
     * @brief abstract method for accepting forwarder.
     * @param forwarder - reference to a gc forwarder.
    */
    virtual void accept_forward(gc_forwarder& forwarder) noexcept = 0;

};

#endif