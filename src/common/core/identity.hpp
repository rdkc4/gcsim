#ifndef CORE_IDENTITY_HPP
#define CORE_IDENTITY_HPP

#include <atomic>
#include <cstdint>

/**
 * @namespace core::identity
 * @brief module defining the identity of the variables.
*/
namespace core::identity {

    /// counter for global roots.
    inline std::atomic<uint64_t> global_root_counter{0};

    /// counter for register roots.
    inline std::atomic<uint64_t> register_root_counter{0};

    /// counter for thread-local roots.
    inline std::atomic<uint64_t> thread_root_counter{0};

    /// counter for thread-local variables.
    inline thread_local uint64_t thread_local_variable_counter{0};

    /**
     * @enum type
     * @brief type of the variable.
    */
    enum class type : uint64_t {
        global_t = 1,        //< prefix of the global root id.
        thread_local_t = 2,  //< prefix of the thread root id.
        register_t = 3       //< prefix of the register root id.
    };

    /**
     * @brief generates the identity for the variable.
     * @param t - type of the variable.
     * @returns identity of the variable.
     * @details identity = [type: 8b][counter: 56b]
    */
    [[nodiscard]] inline uint64_t generate_identity(type t) noexcept {
        uint64_t counter{0};
        switch(t){
            case type::global_t:
                counter = global_root_counter.fetch_add(1);
                break;

            case type::register_t:
                counter = register_root_counter.fetch_add(1);
                break;

            case type::thread_local_t:
                counter = thread_root_counter.fetch_add(1);
                break;

        }
        return (static_cast<uint64_t>(t) << 56) | counter;
    }

    /**
     * @brief generates the id of the thread local variable.
     * @returns thread local variable id.
    */
    [[nodiscard]] inline uint64_t generate_variable_identity() noexcept {
        return thread_local_variable_counter++;
    }

};

#endif