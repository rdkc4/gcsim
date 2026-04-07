#include <gtest/gtest.h>
#include <stdexcept>

#include "../../src/common/queue/queue.hpp"

TEST(queue, front_on_empty_queue_throws){
    queue<int> q;
    EXPECT_THROW(q.front(), std::out_of_range);
}

TEST(queue, pop_on_empty_queue_throws){
    queue<int> q;
    EXPECT_THROW(q.pop(), std::out_of_range);
}