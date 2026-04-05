#include "rng-sim.hpp"

#include <cassert>
#include <random>
#include <thread>

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

    object_probability_t obj_size_probability{};

    ref_probability_t ref_count_probability{};

    operation_probability_t op_type_t{};

    uint32_t generate_object_size(){
        uint32_t category = random_number_generator() % 100;

        if(category < obj_size_probability.small_obj_p){
            return MIN_SMALL_OBJECT_DISTRIBUTION +
                (random_number_generator() % (MAX_SMALL_OBJECT_DISTRIBUTION - MIN_SMALL_OBJECT_DISTRIBUTION + 1));
        }
        else if(category < obj_size_probability.medium_obj_p){
            return MIN_MEDIUM_OBJECT_DISTRIBUTION +
                (random_number_generator() % (MAX_MEDIUM_OBJECT_DISTRIBUTION - MIN_MEDIUM_OBJECT_DISTRIBUTION + 1));
        }
        /// category < obj_size_probability.large_obj_p
        return MIN_LARGE_OBJECT_DISTRIBUTION +
            (random_number_generator() % (MAX_LARGE_OBJECT_DISTRIBUTION - MIN_LARGE_OBJECT_DISTRIBUTION + 1));
        
    }

    uint64_t generate_reference_count(){
        uint32_t category = random_number_generator() % 100 ;

        if(category < ref_count_probability.no_ref_p){
            return 0;
        }
        else if(category < ref_count_probability.one_ref_p){
            return 1;
        }
        /// category < ref_count_probability.two_ref_p
        return 2;
        
    }

    uint32_t generate_simulation_operation(){
        uint32_t category = random_number_generator() % 100;

        if(category < op_type_t.tls_alloc_p){
            return 0;
        }
        else if(category < op_type_t.global_alloc_p){
            return 1;
        }
        else if(category < op_type_t.global_realloc_p){
            return 2;
        }
        /// category < op_type_t.register_realloc_p
        return 3;
        
    }

    namespace shared_space {
        size_t generate_index(size_t n){
            assert(n > 0);

            return random_number_generator() % n;
        }
    };
};