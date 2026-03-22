#include "register-root.hpp"

#include <mutex>

register_root::register_root(uint64_t reg_id, header* var_ptr) 
    : register_id{ reg_id }, register_variable{ var_ptr } {}

uint64_t register_root::get_register_id() const noexcept {
    return register_id;
}

void register_root::set_register_variable(header* var_ptr) noexcept {
    std::lock_guard<std::mutex> register_lock(register_mutex);
    register_variable = var_ptr;
}

void register_root::accept(gc_visitor& visitor) noexcept {
    std::lock_guard<std::mutex> register_lock(register_mutex);
    visitor.visit(*this);
}

void register_root::accept_forward(gc_forwarder& forwarder) noexcept {
    std::lock_guard<std::mutex> register_lock(register_mutex);
    forwarder.forward(*this);
}

header*& register_root::get_register_variable_unlocked() noexcept {
    return register_variable;
}