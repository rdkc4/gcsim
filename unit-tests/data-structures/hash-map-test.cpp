#include <gtest/gtest.h>
#include <stdexcept>

#include "../../src/common/hash-map/hash-map.hpp"

TEST(hash_map, capacity_resizing){
    hash_map<int, int> map;
    size_t initial_capacity{ map.get_capacity() };
    
    for(int i{0}; i < static_cast<int>(initial_capacity); ++i){
        map.insert(i, i);
    }
    size_t increased_capacity{ map.get_capacity() };

    EXPECT_TRUE(increased_capacity > initial_capacity);
}

TEST(hash_map, insert_existing_key_overwrites_value){
    hash_map<int, int> map;
    map.insert(1, 1);

    EXPECT_NO_THROW(map.insert(1, 2));
    EXPECT_TRUE(map[1] == 2);
}

TEST(hash_map, find_key){
    hash_map<int, int> map;
    map.insert(1, 1);

    EXPECT_TRUE(map.find(1) != nullptr);
    EXPECT_TRUE(map.find(2) == nullptr);
}

TEST(hash_map, erase_element){
    hash_map<int, int> map;
    map.insert(1, 1);

    EXPECT_TRUE(map.erase(1));
    EXPECT_FALSE(map.erase(2));
}

TEST(hash_map, accessing_key){
    hash_map<int, int> map;
    map.insert(1, 1);

    EXPECT_NO_THROW(map[1]);
}

TEST(hash_map, accessing_invalid_key_throws){
    hash_map<int, int> map;
    EXPECT_THROW(map[1], std::out_of_range);
}