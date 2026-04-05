#include "shared-global-space.hpp"

#include "../common/rng/rng-sim.hpp"

shared_global_space::shared_global_space(heap_manager& heap_mngr, uint64_t id)
    : root_id{id}, heap_manager_ref{ heap_mngr } {
        heap_manager_ref.add_root(id, this);
    }

shared_global_space::~shared_global_space(){
    std::lock_guard<std::mutex> global_space_lock{ global_space_mtx };
    heap_manager_ref.remove_root(root_id);
}

void shared_global_space::allocate(header* heap_ptr){
    std::lock_guard<std::mutex> global_space_lock{ global_space_mtx };
    allocate_internal(heap_ptr);
}

void shared_global_space::reallocate_random(header* heap_ptr){
    std::lock_guard<std::mutex> global_space_lock{ global_space_mtx };
    
    const size_t size = global_variables.get_size();
    if(size == 0) {
        allocate_internal(heap_ptr);
        return;
    }

    global_variables[rng::sim::shared_space::generate_index(size)] = heap_ptr;
}

void shared_global_space::allocate_internal(header* heap_ptr){
    global_variables.push(heap_ptr);
}

void shared_global_space::accept(gc_visitor& visitor) noexcept {
    visitor.visit(*this);
}

void shared_global_space::accept_forward(gc_forwarder& forwarder) noexcept {
    forwarder.forward(*this);
}