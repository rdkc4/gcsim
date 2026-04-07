#ifndef FIXED_STACK_HPP
#define FIXED_STACK_HPP

#include <cstddef>
#include <stdexcept>

/**
 * @class fixed_stack
 * @brief fixed capacity stack.
 * @tparam T - type of the stack elements.
 * @tparam capacity - max capacity of the stack.
*/
template<typename T, size_t capacity>
class fixed_stack {
private:
    /// elements of the stack.
    T data[capacity];

    /// index of the top of the stack.
    size_t top;

public:
    /**
     * @brief creates the instance of the fixed stack.
     * @details top is set to 0.
    */
    fixed_stack() : top { 0 } {};

    /// deleted copy constructor.
    fixed_stack(const fixed_stack&) = delete;

    /// deleted assignment operator.
    fixed_stack& operator=(const fixed_stack&) = delete;

    /**
     * @brief pushes an element on the stack.
     * @param value - const reference to an element that is being pushed.
     * @returns pointer to the element.
     * @throws out_of_range if stack is full.
    */
    T* push(const T& value){
        if(top >= capacity){
            throw std::out_of_range("Stack is full, cannot push");
        }
        data[top] = value;
        return &data[top++];
    }

    /**
     * @brief pushes an element on the stack.
     * @param value - element that is being pushed.
     * @returns pointer to the element.
     * @throws out_of_range if stack is full.
    */
    T* push(T&& value) {
        if(top >= capacity){
            throw std::out_of_range("Stack is full, cannot push");
        }
        data[top] = std::move(value);
        return &data[top++];
    }

    /**
     * @brief pops the element off the stack.
     * @throws out_of_range if the stack is empty.
    */
    void pop(){
        if(top == 0){
            throw std::out_of_range("Cannot pop from an empty stack");
        }
        --top;
    }

    /**
     * @brief peeks at the element on the top of the stack.
     * @returns pointer to an element, or nullptr if stack is empty.
    */
    T* peek() noexcept {
        return (top > 0) ? &data[top - 1] : nullptr;
    }

    /**
     * @brief operator for direct access.
     * @param i - index on the stack.
     * @returns reference to an element on the stack at given index.
     * @throws std::out_of_range when index is bigger than top.
    */
    T& operator[](size_t i){
        if(i >= top){
            throw std::out_of_range("Index out of range");
        }
        return data[i];
    }

    /**
     * @brief operator for direct access.
     * @param i - index on the stack.
     * @returns const reference to an element on the stack at given index.
     * @throws std::out_of_range when index is bigger than top.
    */
    const T& operator[](size_t i) const {
        if(i >= top){
            throw std::out_of_range("Index out of range");
        }
        return data[i];
    }

    /**
     * @brief getter for the size of the stack.
     * @returns number of elements on the stack.
    */
    size_t get_size() const noexcept { 
        return top; 
    }

    /**
     * @brief getter for the capacity of the stack.
     * @returns capacity of the stack.
    */
    constexpr size_t get_capacity() const noexcept { 
        return capacity; 
    }

};

#endif