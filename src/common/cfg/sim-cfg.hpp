#ifndef SIM_CFG_HPP
#define SIM_CFG_HPP

#include <cstddef>

/**
 * @namespace cfg::sim
 * @brief module defining the constant parameters of the simulation.
*/
namespace cfg::sim {
    /// number of allocations per tls in stress mode.
    constexpr size_t TLS_ALLOC_STRESS_THRESHOLD = 65536;

    /// number of scopes per tls for stress mode.
    constexpr size_t TLS_SCOPE_COUNT_STRESS = 8;

    /// number of allocations per scope for tls in stress mode.
    constexpr size_t TLS_ALLOC_STRESS_THRESHOLD_PER_SCOPE = TLS_ALLOC_STRESS_THRESHOLD / TLS_SCOPE_COUNT_STRESS;

    /// capacity of the hash-map that maps tls variable to its index in stress mode.
    constexpr size_t TLS_MAP_CAPACITY_STRESS = TLS_ALLOC_STRESS_THRESHOLD_PER_SCOPE << 1;

    /// number of allocations per tls in relaxed mode.
    constexpr size_t TLS_ALLOC_RELAXED_THRESHOLD = 1024;

    /// number of scopes per tls in relaxed mode.
    constexpr size_t TLS_SCOPE_COUNT_RELAXED = 8;

    /// number of allocations per scope for tls in relaxed mode.
    constexpr size_t TLS_ALLOC_RELAXED_THRESHOLD_PER_SCOPE = TLS_ALLOC_RELAXED_THRESHOLD / TLS_SCOPE_COUNT_RELAXED;

    /// capacity of the hash-map that maps tls variable to its index in relaxed mode.
    constexpr size_t TLS_MAP_CAPACITY_RELAXED = TLS_ALLOC_RELAXED_THRESHOLD_PER_SCOPE << 1;

};

#endif