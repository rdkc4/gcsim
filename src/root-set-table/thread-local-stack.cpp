#include "thread-local-stack.hpp"

#include <mutex>
#include <stdexcept>
#include <utility>

thread_local_stack::thread_local_stack() : scope(1) {}

thread_local_stack::thread_local_stack(size_t hash_map_capacity) : scope(1), var_to_idx(hash_map_capacity) {}

thread_local_stack::~thread_local_stack() {
    pop_scope(true);
}

thread_local_stack::thread_local_stack(thread_local_stack&& other) noexcept : scope(std::exchange(other.scope, 0)),
    thread_stack(std::move(other.thread_stack)), var_to_idx(std::move(other.var_to_idx)) {}

thread_local_stack& thread_local_stack::operator=(thread_local_stack&& other) noexcept {
    if(this != &other){
        scope = std::exchange(other.scope, 0);
        thread_stack = std::move(other.thread_stack);
        var_to_idx = std::move(other.var_to_idx);
    }
    return *this;
}

void thread_local_stack::init(uint64_t variable_id, header* heap_ptr){
    std::lock_guard<std::mutex> tls_lock(tls_mutex);

    if(var_to_idx.contains(variable_id)){
        throw std::invalid_argument("Variable already exists");
    }
    var_to_idx.insert(variable_id, thread_stack.get_size());
    thread_stack.push(
        thread_local_stack_entry{
            .ref_to = heap_ptr, 
            .scope = scope, 
            .variable_id = std::move(variable_id)
        }
    );
}

void thread_local_stack::reassign_ref(uint64_t variable_id, header* new_ref_to){
    std::lock_guard<std::mutex> tls_lock(tls_mutex);

    if(!var_to_idx.contains(variable_id)){
        throw std::invalid_argument("Variable doesn't exist");
    }
    size_t idx = var_to_idx[variable_id];
    thread_stack[idx].ref_to = new_ref_to;
}

void thread_local_stack::remove_ref(uint64_t variable_id){
    std::lock_guard<std::mutex> tls_lock(tls_mutex);

    if(!var_to_idx.contains(variable_id)){
        throw std::invalid_argument("Variable doesn't exist");
    }
    size_t idx = var_to_idx[variable_id];
    thread_stack[idx].ref_to = nullptr;
}

void thread_local_stack::push_scope() noexcept {
    std::lock_guard<std::mutex> tls_lock(tls_mutex);
    ++scope;
}

void thread_local_stack::pop_scope(bool destr){
    std::lock_guard<std::mutex> tls_lock(tls_mutex);

    if((scope <= 1 && !destr) || scope == 0){
        return;
    }

    while(!thread_stack.empty() && thread_stack.peek().scope == scope){
        var_to_idx.erase(thread_stack.peek().variable_id);
        thread_stack.pop();
    }
    --scope;
}

void thread_local_stack::accept(gc_visitor& visitor) noexcept {
    std::lock_guard<std::mutex> tls_lock(tls_mutex);
    visitor.visit(*this);
}

indexed_stack<thread_local_stack_entry>& thread_local_stack::get_thread_stack_unlocked() noexcept {
    return thread_stack;
}