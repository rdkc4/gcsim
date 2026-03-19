#include "root-set-table.hpp"

#include <utility>

void root_set_table::add_root(std::string key, root_set_base* root) {
    roots.insert(std::move(key), root);
}

void root_set_table::remove_root(const std::string& key) {
    roots.erase(key);
}

root_set_base* root_set_table::get_root(const std::string& key) noexcept {
    auto* entry = roots.find(key);
    return entry ? *entry : nullptr;
}

const root_set_base* root_set_table::get_root(const std::string& key) const noexcept {
    const auto* entry = roots.find(key);
    return entry ? *entry : nullptr;
}

hash_map<std::string, root_set_base*>& root_set_table::get_roots() noexcept {
    return roots;
}

const hash_map<std::string, root_set_base*>& root_set_table::get_roots() const noexcept {
    return roots;
}

void root_set_table::clear() noexcept {
    roots.clear();
}

size_t root_set_table::get_root_count() const noexcept {
    return roots.get_size();
}