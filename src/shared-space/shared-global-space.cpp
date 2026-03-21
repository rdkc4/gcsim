#include "shared-global-space.hpp"
#include <cstdint>
#include <memory>
#include <mutex>

#include "../common/rng/rng-sim.hpp"
#include "../common/core/identity/identity.hpp"

shared_global_space::shared_global_space(heap_manager& heap_mngr)
    : heap_manager_ref{ heap_mngr } {}

shared_global_space::~shared_global_space(){
    std::lock_guard<std::mutex> global_space_lock{ global_space_mtx };
    while(!global_variables.empty()){
        heap_manager_ref.remove_root(global_variables.peek()->get_global_id());
        global_variables.pop();
    }
}

void shared_global_space::allocate(header* heap_ptr){
    std::lock_guard<std::mutex> global_space_lock{ global_space_mtx };
    uint64_t id = core::identity::generate_identity(core::identity::type::global_t);

    auto global_var = std::make_unique<global_root>(id, heap_ptr);
    auto* raw = global_var.get();

    global_variables.push(std::move(global_var));
    heap_manager_ref.add_root(id, raw);
}

void shared_global_space::reallocate_random(header* heap_ptr){
    std::lock_guard<std::mutex> global_space_lock{ global_space_mtx };
    
    const size_t size = global_variables.get_size();
    if(size == 0) [[unlikely]] return;

    global_variables[rng::sim::shared_space::generate_index(size)]->set_global_variable(heap_ptr);
}