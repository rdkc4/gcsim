#include "thread-local-stack.hpp"

thread_local_stack::thread_local_stack(heap_manager& heap_mngr, uint64_t id) 
    : scope{ 1 }, tls_id{ id }, heap_manager_ref{ heap_mngr } {}

thread_local_stack::~thread_local_stack() {
    pop_scope(true);
    heap_manager_ref.remove_root(tls_id);
}

void thread_local_stack::init(header* heap_ptr){
    tls.push(
        thread_local_stack_entry{
            .ref_to = heap_ptr, 
            .scope = scope
        }
    );
}

void thread_local_stack::reassign_ref(size_t index, header* new_ref_to){
    tls[index].ref_to = new_ref_to;
}

void thread_local_stack::remove_ref(size_t index){
    tls[index].ref_to = nullptr;
}

header** thread_local_stack::push_temp(header* heap_ptr){
    return temporaries.push(heap_ptr);
}

void thread_local_stack::pop_temp(){
    temporaries.pop();
}

void thread_local_stack::push_scope() noexcept {
    ++scope;
}

void thread_local_stack::pop_scope(bool destr){
    if((scope <= 1 && !destr) || scope == 0){
        return;
    }

    while(!tls.empty() && tls.peek().scope == scope){
        tls.pop();
    }
    --scope;
}

void thread_local_stack::accept(gc_visitor& visitor) noexcept {
    visitor.visit(*this);
}

void thread_local_stack::accept_forward(gc_forwarder& forwarder) noexcept {
    forwarder.forward(*this);
}
