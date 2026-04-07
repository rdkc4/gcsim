#ifndef HEADER_CFG_HPP
#define HEADER_CFG_HPP

#include <cstdint>

/**
 * @namespace cfg::header
 * @brief module defining constants related to header.
*/
namespace cfg::header {
    /// mask for the free bit of the header.
    constexpr uint8_t IS_FREE{ 0b00000001 };

    /// mask for the marked bit of the header.
    constexpr uint8_t IS_MARKED{ 0b00000010 };

};

#endif