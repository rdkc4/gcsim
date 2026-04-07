#include <gtest/gtest.h>
#include <cstddef>
#include <stdexcept>

#include "../../src/common/stack/indexed-stack.hpp"
#include "../../src/common/stack/fixed-stack.hpp"

TEST(indexed_stack, capacity_resizing){
    indexed_stack<int> stack;
    size_t initial_capacity{ stack.get_capacity() };

    for(int i{0}; i < 32; ++i){
        stack.push(i);
    }
    size_t increased_capacity_after_resize{ stack.get_capacity() };
    EXPECT_TRUE(increased_capacity_after_resize > initial_capacity);

    for(int i{0}; i < 32; ++i){
        stack.pop();
    }
    size_t decreased_capacity_after_resize{ stack.get_capacity() };
    EXPECT_TRUE(decreased_capacity_after_resize < increased_capacity_after_resize); 
}

TEST(indexed_stack, peek_on_empty_stack_throws){
    indexed_stack<int> stack;
    EXPECT_THROW(stack.peek(), std::out_of_range);
}

TEST(indexed_stack, pop_on_empty_stack_throws){
    indexed_stack<int> stack;
    EXPECT_THROW(stack.pop(), std::out_of_range);
}

TEST(indexed_stack, accessing_invalid_index_throws){
    indexed_stack<int> stack;
    EXPECT_THROW(stack[3], std::out_of_range);
}

TEST(fixed_stack, exceeding_capacity_throws){
    fixed_stack<int, 2> stack;
    for(int i{0}; i < 2; ++i){
        stack.push(i);
    }

    EXPECT_THROW(stack.push(3), std::out_of_range);
}

TEST(fixed_stack, pop_on_empty_stack_throws){
    fixed_stack<int, 1> stack;
    EXPECT_THROW(stack.pop(), std::out_of_range);
}

TEST(fixed_stack, accessing_invalid_index_throws){
    fixed_stack<int, 2> stack;
    EXPECT_THROW(stack[1], std::out_of_range);
}

TEST(fixed_stack, peek_on_empty_stack_returns_nullptr){
    fixed_stack<int, 1> stack;
    EXPECT_TRUE(stack.peek() == nullptr);
}