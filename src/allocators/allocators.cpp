#include "allocators.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <format>
#include <cstring>
#include <memory>

#include "../common/rng/rng-sim.hpp"
#include "../common/core/identity/identity.hpp"
#include "../root-set-table/root-guard.hpp"

allocators::allocators(heap_manager& heap_manager_ref, size_t thread_count) 
    : heap_manager_ref{heap_manager_ref}, 
      alloc_thread_pool{thread_count},
      register_space{heap_manager_ref, core::identity::generate_identity(core::identity::type::register_t)},
      global_space{heap_manager_ref, core::identity::generate_identity(core::identity::type::global_t)} {}

thread_local size_t allocators::per_thread_allocs{0};

thread_local size_t allocators::per_thread_failed_allocs{0};

void allocators::simulate_alloc(size_t tls_count, simulation_mode mode){
    total_allocs = 0;
    total_failed_allocs = 0;

    std::cout << std::format("Initializing {} simulation\n", simulation_mode_name(mode));
    const auto start_time{ std::chrono::high_resolution_clock::now() };

    std::latch completion_latch{ static_cast<ptrdiff_t>(tls_count) };
    
    const size_t tls_scopes{ tls_scope_count(mode) };
    const size_t tls_allocs{ tls_allocs_per_scope(mode) };

    for(size_t i{0}; i < tls_count; ++i){
        auto tls{ std::make_unique<thread_local_stack>(
            heap_manager_ref, 
            core::identity::generate_identity(core::identity::type::thread_local_t))
        };
        enqueue_simulation(
            [this, tls=std::move(tls), tls_scopes, tls_allocs] -> void {
                heap_manager_ref.register_mutator();
                simulate_thread_alloc(tls.get(), tls_scopes, tls_allocs);
                heap_manager_ref.unregister_mutator();
            }, 
            completion_latch
        );
    }

    completion_latch.wait();
    const auto duration{ 
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_time
        ) 
    };

    std::cout << std::format("Total allocation count: {} allocations\n", total_allocs.load(std::memory_order_relaxed));
    std::cout << std::format("Total execution time: {} ms ({} s)\n", duration.count(), duration.count() / 1000.0);

    std::cout << std::format("Allocation throughput: {:.2f} allocs/ms ({:.2f} allocs/s)\n", 
        static_cast<double>(total_allocs) / duration.count(), 
        static_cast<double>(total_allocs) / duration.count() * 1000
    );

    std::cout << std::format("Failed to allocate {} out of {} objects ({:.2f}%)\n", 
        total_failed_allocs.load(std::memory_order_relaxed),
        total_allocs.load(std::memory_order_relaxed),
        static_cast<double>(total_failed_allocs) / static_cast<double>(total_allocs)
    );

    std::cout << "Cleaning up after simulation\n";
    heap_manager_ref.clear_roots();
    heap_manager_ref.collect_garbage();
}

void allocators::simulate_thread_alloc(thread_local_stack* tls, size_t scope_count, size_t allocs_per_scope){
    if(!tls) return;
    per_thread_allocs = 0;
    per_thread_failed_allocs = 0;

    for(size_t scope{0}; scope < scope_count; ++scope){
        tls->push_scope();

        for(size_t i = 0; i < allocs_per_scope; ++i){
            heap_manager_ref.safepoint_poll();

            header* obj{ allocate_object(tls) };

            simulation_operation sim_op{ static_cast<simulation_operation>(rng::sim::generate_simulation_operation()) };

            switch(sim_op){
                case simulation_operation::tls_alloc:
                    tls->init(obj);
                    break;
                case simulation_operation::global_alloc:
                    global_space.allocate(obj);
                    break;
                case simulation_operation::global_realloc:
                    global_space.reallocate_random(obj);
                    break;
                case simulation_operation::register_realloc:
                    register_space.reallocate_random(obj);
                    break;
            }
        }

        tls->pop_scope();
    }

    total_allocs.fetch_add(per_thread_allocs, std::memory_order_relaxed);
    total_failed_allocs.fetch_add(per_thread_failed_allocs, std::memory_order_relaxed);
}

header* allocators::allocate_object(thread_local_stack* tls){
    ++per_thread_allocs;
    const uint64_t ref_count{ rng::sim::generate_reference_count() };

    header* object{ heap_manager_ref.allocate(
        sizeof(type_descriptor) + 
        ref_count * sizeof(header*) + 
        rng::sim::generate_object_size()
    )};

    if(!object){
        ++per_thread_failed_allocs;
        return nullptr;
    }

    root_guard guard{tls, object};
    
    header** slot{ guard.get_slot() };
    type_descriptor* td{ reinterpret_cast<type_descriptor*>((*slot)->data_ptr()) };
    td->ref_count = ref_count;

    if(ref_count == 0){
        return object;
    }

    header** init_refs{ reinterpret_cast<header**>(td + 1) };
    for(size_t i{0}; i < ref_count; ++i){
        init_refs[i] = nullptr;
    }

    for(size_t i{0}; i < ref_count; ++i){
        header** refs{ 
            reinterpret_cast<header**>(
                reinterpret_cast<uint8_t*>((*slot)->data_ptr()) + sizeof(type_descriptor)
            ) 
        };
        refs[i] = allocate_object(tls);
    }
    
    return object;
}
