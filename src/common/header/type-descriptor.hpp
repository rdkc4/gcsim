#ifndef TYPE_DESCRIPTOR_HPP
#define TYPE_DESCRIPTOR_HPP

#include <cstdint>

struct header;

/**
 * @struct type_descriptor
 * @brief defines the references to the objects owned by other object.
*/
struct type_descriptor {
    /// number of references owned by object.
    uint64_t ref_count;

};

#endif