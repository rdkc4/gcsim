#include "rng-sim.hpp"

#include <random>
#include <thread>

#include "../cfg/sim-cfg.hpp"

namespace rng::sim {
    
    /// random number generator.
    thread_local std::mt19937 random_number_generator {
        std::random_device{}() + std::hash<std::thread::id>{}(std::this_thread::get_id())
    };

    /// distribution for object size category.
    thread_local std::uniform_int_distribution<int> object_category_distribution {
        MIN_OBJECT_DISTRIBUTION,
        MAX_OBJECT_DISTRIBUTION
    };

    /// distribution for small object size.
    thread_local std::uniform_int_distribution<uint32_t> small_object_distribution {
        MIN_SMALL_OBJECT_DISTRIBUTION,
        MAX_SMALL_OBJECT_DISTRIBUTION
    };

    /// distribution for medium object size.
    thread_local std::uniform_int_distribution<uint32_t> medium_object_distribution {
        MIN_MEDIUM_OBJECT_DISTRIBUTION,
        MAX_MEDIUM_OBJECT_DISTRIBUTION
    };

    /// distribution for large object size.
    thread_local std::uniform_int_distribution<uint32_t> large_object_distribution {
        MIN_LARGE_OBJECT_DISTRIBUTION,
        MAX_LARGE_OBJECT_DISTRIBUTION
    };

    uint32_t generate_object_size(){
        int category = rng::sim::object_category_distribution(rng::sim::random_number_generator);

        if (category <= cfg::sim::SMALL_OBJECT_CATEGORY_THRESHOLD) {
            return rng::sim::small_object_distribution(rng::sim::random_number_generator);
        } else if (category <= cfg::sim::MEDIUM_OBJECT_CATEGORY_THRESHOLD) {
            return rng::sim::medium_object_distribution(rng::sim::random_number_generator);
        } else {
            return rng::sim::large_object_distribution(rng::sim::random_number_generator);
        }
    }
};