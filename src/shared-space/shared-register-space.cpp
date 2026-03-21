#include "shared-register-space.hpp"

#include "../common/cfg/sim-cfg.hpp"
#include "../common/rng/rng-sim.hpp"
#include "../common/core/identity/identity.hpp"

shared_register_space::shared_register_space(heap_manager& heap_mngr) 
    : heap_manager_ref{ heap_mngr } {
        for(size_t i = 0; i < cfg::sim::REGISTER_COUNT; ++i){
            uint64_t id = core::identity::generate_identity(core::identity::type::register_t);
            registers.push(std::make_unique<register_root>(id, nullptr));
        }

        for(const auto& reg : registers){
            heap_manager_ref.add_root(reg->get_register_id(), reg.get());
        }
    }

shared_register_space::~shared_register_space(){
    std::lock_guard<std::mutex> register_space_lock{ register_space_mtx };
    while(!registers.empty()){
        heap_manager_ref.remove_root(registers.peek()->get_register_id());
        registers.pop();
    }
}

void shared_register_space::reallocate_random(header* heap_ptr){
    std::lock_guard<std::mutex> register_space_lock{ register_space_mtx };
    
    const size_t size = registers.get_size();
    if(size == 0) [[unlikely]] return;

    registers[rng::sim::shared_space::generate_index(size)]->set_register_variable(heap_ptr);
}