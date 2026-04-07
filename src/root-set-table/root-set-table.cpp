#include "root-set-table.hpp"

#include <utility>

void root_set_table::add_root(uint64_t id, root_set_base* root) {
    roots.insert(std::move(id), root);
}

void root_set_table::remove_root(uint64_t id) {
    roots.erase(id);
}

root_set_base* root_set_table::get_root(uint64_t id) noexcept {
    auto* entry{ roots.find(id) };
    return entry ? *entry : nullptr;
}

const root_set_base* root_set_table::get_root(uint64_t id) const noexcept {
    const auto* entry{ roots.find(id) };
    return entry ? *entry : nullptr;
}

hash_map<uint64_t, root_set_base*>& root_set_table::get_roots() noexcept {
    return roots;
}

const hash_map<uint64_t, root_set_base*>& root_set_table::get_roots() const noexcept {
    return roots;
}

void root_set_table::clear() noexcept {
    roots.clear();
}

size_t root_set_table::get_root_count() const noexcept {
    return roots.get_size();
}