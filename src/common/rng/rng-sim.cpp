#include "rng-sim.hpp"

#include <cassert>
#include <random>
#include <thread>

#include "../cfg/sim-cfg.hpp"

namespace rng::sim {
    
    /// random number generator.
    thread_local std::mt19937 random_number_generator {
        []{
            std::random_device rd;
            std::seed_seq seq {
                rd(),
                rd(),
                static_cast<unsigned>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
            };

            return std::mt19937(seq);
        }()
    };

    /// distribution for object size category.
    thread_local std::uniform_int_distribution<uint32_t> object_category_distribution {
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
        uint32_t category = object_category_distribution(random_number_generator);

        if (category <= cfg::sim::SMALL_OBJECT_CATEGORY_THRESHOLD) {
            return small_object_distribution(random_number_generator);
        } else if (category <= cfg::sim::MEDIUM_OBJECT_CATEGORY_THRESHOLD) {
            return medium_object_distribution(random_number_generator);
        } else {
            return large_object_distribution(random_number_generator);
        }
    }

    namespace shared_space {
        /// distribution of indexes of the shared spaces
        thread_local std::uniform_int_distribution<size_t> index_distribution;

        size_t generate_index(size_t n){
            assert(n > 0);

            return index_distribution(
                random_number_generator, 
                decltype(index_distribution)::param_type(0, n - 1)
            );
        }

    };
};