#include "allocators.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <format>
#include <cstring>

#include "../common/rng/rng-sim.hpp"
#include "../common/core/identity/identity.hpp"

allocators::allocators(heap_manager& heap_manager_ref, size_t thread_count) 
    : heap_manager_ref(heap_manager_ref), alloc_thread_pool(thread_count) {}

void allocators::simulate_alloc(size_t tls_count, size_t global_count, size_t register_count, simulation_mode mode){
    std::cout << std::format("Initializing {} simulation\n", simulation_mode_name(mode));
    const auto start_time = std::chrono::high_resolution_clock::now();

    std::latch completion_latch(tls_count + global_count + register_count);
    
    const size_t tls_scopes = tls_scope_count(mode);
    const size_t tls_allocs = tls_allocs_per_scope(mode);
    const size_t global_allocs = global_alloc_count(mode);
    const size_t reg_allocs = register_alloc_count(mode);

    for(size_t i = 0; i < tls_count; ++i){
        uint64_t id = core::identity::generate_identity(core::identity::type::thread_local_t);
        auto tls = create_root<thread_local_stack>(id, tls_map_capacity(mode));
        enqueue_simulation([this, tls=std::move(tls), tls_scopes, tls_allocs, id] -> void {
            simulate_tls_alloc(tls.get(), tls_scopes, tls_allocs);
            heap_manager_ref.remove_root(id);
        }, completion_latch);
    }

    for(size_t i = 0; i < global_count; ++i){
        uint64_t id = core::identity::generate_identity(core::identity::type::global_t);
        auto global = create_root<global_root>(id, id, nullptr);
        enqueue_simulation([this, global=std::move(global), global_allocs, id] -> void {
            simulate_global_alloc(global.get(), global_allocs);
            heap_manager_ref.remove_root(id);
        }, completion_latch);
    }

    for(size_t i = 0; i < register_count; ++i){
        uint64_t id = core::identity::generate_identity(core::identity::type::register_t);
        auto reg = create_root<register_root>(id, id, nullptr);
        enqueue_simulation([this, reg=std::move(reg), reg_allocs, id] -> void {
            simulate_register_alloc(reg.get(), reg_allocs);
            heap_manager_ref.remove_root(id);
        }, completion_latch);
    }

    completion_latch.wait();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time);

    const size_t total_allocs = tls_scopes * tls_allocs * tls_count + global_allocs * global_count + reg_allocs * register_count;
    std::cout << std::format("Total allocation count: {} allocations\n", total_allocs);
    std::cout << std::format("Total execution time: {} ms ({} s)\n", duration.count(), duration.count() / 1000.0);

    std::cout << std::format("Allocation throughput: {:.2f} allocs/ms ({:.2f} allocs/s)\n", 
        static_cast<double>(total_allocs) / duration.count(), 
        static_cast<double>(total_allocs) / duration.count() * 1000
    );

    std::cout << "Cleaning up after simulation\n";
    heap_manager_ref.clear_roots();
    heap_manager_ref.collect_garbage();
}

void allocators::simulate_tls_alloc(thread_local_stack* tls, size_t scope_count, size_t allocs_per_scope){
    if(!tls) return;

    for(size_t scope = 0; scope < scope_count; ++scope){
        tls->push_scope();

        for(size_t i = 0; i < allocs_per_scope; ++i){
            const uint64_t ref_count = 1;
            const size_t obj_size = sizeof(type_descriptor) + ref_count * sizeof(header*) + rng::sim::generate_object_size();

            header* obj = heap_manager_ref.allocate(obj_size);
            if(!obj) continue;

            auto* td = reinterpret_cast<type_descriptor*>(obj + 1);
            td->ref_count = ref_count;

            for(uint64_t r = 0; r < ref_count; ++r) {
                header** refs = reinterpret_cast<header**>(td + 1);
                
                refs[r] = heap_manager_ref.allocate(sizeof(type_descriptor) + 1);

                if(refs[r]){
                    auto* nested_td = reinterpret_cast<type_descriptor*>(refs[r]->data_ptr());
                    nested_td->ref_count = 0;
                }
            }

            tls->init(core::identity::generate_variable_identity(), obj);
        }

        tls->pop_scope();
    }
}

void allocators::simulate_global_alloc(global_root* global, size_t global_allocs){
    if(!global) return;
    for(size_t i = 0; i < global_allocs; ++i){
        if(i & 1){
            global->set_global_variable(nullptr);
            continue;
        }
        header* obj = heap_manager_ref.allocate(
            sizeof(type_descriptor) + rng::sim::generate_object_size()
        );
        if(obj){
            type_descriptor* td = reinterpret_cast<type_descriptor*>(obj->data_ptr());
            td->ref_count = 0;
        }
        global->set_global_variable(obj);
    }
}

void allocators::simulate_register_alloc(register_root* reg, size_t register_allocs){
    if(!reg) return;
    for(size_t i = 0; i < register_allocs; ++i){
        if(i & 1){
            reg->set_register_variable(nullptr);
            continue;
        }
        header* obj = heap_manager_ref.allocate(
            sizeof(type_descriptor) + rng::sim::generate_object_size()
        );
        if(obj){
            type_descriptor* td = reinterpret_cast<type_descriptor*>(obj->data_ptr());
            td->ref_count = 0;
        }
        reg->set_register_variable(obj);
    }
}
