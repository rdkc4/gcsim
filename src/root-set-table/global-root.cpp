#include "global-root.hpp"

global_root::global_root(uint64_t id, header* var_ptr) 
    : global_id{ id }, global_variable_ptr{ var_ptr } {}

uint64_t global_root::get_global_id() const noexcept {
    return global_id;
}

void global_root::set_global_variable(header* var_ptr) noexcept {
    std::lock_guard<std::mutex> global_lock(global_mutex);
    global_variable_ptr = var_ptr;
}

void global_root::accept(gc_visitor& visitor) noexcept {
    std::lock_guard<std::mutex> global_lock(global_mutex);
    visitor.visit(*this);
}

header* global_root::get_global_variable_unlocked() noexcept {
    return global_variable_ptr;
}