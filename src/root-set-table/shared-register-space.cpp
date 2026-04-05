#include "shared-register-space.hpp"

#include "../common/cfg/sim-cfg.hpp"
#include "../common/rng/rng-sim.hpp"

shared_register_space::shared_register_space(heap_manager& heap_mngr, uint64_t id) 
    : register_id{ id }, 
      heap_manager_ref{ heap_mngr },
      registers{ cfg::sim::REGISTER_COUNT } {

        for(size_t i{0}; i < cfg::sim::REGISTER_COUNT; ++i){
            registers.push(nullptr);
        }

        heap_manager_ref.add_root(id, this);
    }

shared_register_space::~shared_register_space(){
    heap_manager_ref.remove_root(register_id);
}

void shared_register_space::reallocate_random(header* heap_ptr){
    std::lock_guard<std::mutex> register_space_lock{ register_space_mtx };
    
    const size_t size{ registers.get_size() };
    if(size == 0) [[unlikely]] return;

    registers[rng::sim::shared_space::generate_index(size)] = heap_ptr;
}

void shared_register_space::accept(gc_visitor& visitor) noexcept {
    visitor.visit(*this);
}

void shared_register_space::accept_forward(gc_forwarder& forwarder) noexcept {
    forwarder.forward(*this);
}