/*
 * Author: Ritik Jain, 18114068
 * Since: May 24, 2021
 * Brief: Auxiliary Program- Implements Dynamic Integer Stack
 */

#include "stdlib.h"

const unsigned int STACK_INITIAL_CAPACITY = 1;
const unsigned int STACK_EXPANSION_FACTOR = 2;

struct stack_struct{
    int *__stack;
    int __size;
    int __capacity;
};

void __stack_inflate(struct stack_struct *stack){
    if(stack->__size < stack->__capacity) return;
    int new_cap = STACK_EXPANSION_FACTOR*stack->__capacity;
    stack->__stack = (int*) realloc( (void*) stack->__stack, sizeof(int)*new_cap);
    stack->__capacity = new_cap;
}

void __stack_deflate(struct stack_struct *stack){
    if(stack->__capacity%STACK_EXPANSION_FACTOR || stack->__capacity < stack->__size*STACK_EXPANSION_FACTOR) return;
    int new_cap = stack->__capacity/STACK_EXPANSION_FACTOR;
    stack->__stack = (int*) realloc( (void*) stack->__stack, sizeof(int)*new_cap);
}

struct stack_struct *stack_alloc(){
    struct stack_struct *stack = (struct stack_struct*) malloc(sizeof(struct stack_struct));
    stack->__size = 0;
    stack->__capacity = STACK_INITIAL_CAPACITY;
    stack->__stack = (int*) malloc(STACK_INITIAL_CAPACITY*sizeof(int));
    return stack;
}

void stack_dealloc(struct stack_struct *stack){
    free(stack->__stack);
    free(stack);
}

int stack_empty(struct stack_struct *stack){
    return !(stack->__size);
}

int stack_full(struct stack_struct *stack){
    return (stack->__size == stack->__capacity);
}

int stack_size(struct stack_struct *stack){
    return stack->__size;
}

int stack_capacity(struct stack_struct *stack){
    return stack->__capacity;
}

void stack_push(int c, struct stack_struct *stack){
    __stack_inflate(stack);
    stack->__stack[stack->__size++] = c;
}

int stack_pop(struct stack_struct *stack){
    int c = stack->__stack[--stack->__size];
    __stack_deflate(stack);
    return c;
}

int stack_peek(struct stack_struct *stack){
    return stack->__stack[stack->__size-1];
}

int stack_lookback(int lookback, struct stack_struct *stack){
    return stack->__stack[stack->__size-1-lookback];
}

int stack_get(int index, struct stack_struct *stack){
    return stack->__stack[index];
}

int stack_set(int  index, int value, struct stack_struct *stack){
    stack->__stack[index] = value;
}

void stack_clear(struct stack_struct *stack){
    stack->__stack = (int*) realloc(stack->__stack, sizeof(int)*STACK_INITIAL_CAPACITY);
    stack->__size = 0;
    stack->__capacity = STACK_INITIAL_CAPACITY;
}

void stack_push_ptr(void *ptr, struct stack_struct *stack){
    unsigned long tmp = (unsigned long) ptr;
    stack_push((int)tmp,stack);
    stack_push((int)(tmp>>sizeof(int)*8),stack);
}

void *stack_pop_ptr(struct  stack_struct *stack){
    int tmp1 = stack_pop(stack);
    int tmp2 = stack_pop(stack);
    unsigned long tmp = 0;
    if(tmp1<0) tmp = ((unsigned long) 1 << sizeof(int)*8) + tmp1;
    else tmp = (unsigned long) tmp1;
    tmp <<= (sizeof(int)*8);
    if(tmp2<0) tmp += ((unsigned long) 1 << sizeof(int)*8) + tmp2;
    else tmp += tmp2;
    return (void*)tmp;
}

void *stack_get_ptr(int index, struct stack_struct *stack){
    int tmp2 = stack_get(2*index,stack);
    int tmp1 = stack_get(2*index+1,stack);
    unsigned long tmp = 0;
    if(tmp1<0) tmp = ((unsigned long) 1 << sizeof(int)*8) + tmp1;
    else tmp = (unsigned long) tmp1;
    tmp <<= (sizeof(int)*8);
    if(tmp2<0) tmp += ((unsigned long) 1 << sizeof(int)*8) + tmp2;
    else tmp += tmp2;
    return (void*)tmp;
}

void *stack_peek_ptr(struct stack_struct *stack){
    int tmp2 = stack_lookback(1,stack);
    int tmp1 = stack_peek(stack);
    unsigned long tmp = 0;
    if(tmp1<0) tmp = ((unsigned long) 1 << sizeof(int)*8) + tmp1;
    else tmp = (unsigned long) tmp1;
    tmp <<= (sizeof(int)*8);
    if(tmp2<0) tmp += ((unsigned long) 1 << sizeof(int)*8) + tmp2;
    else tmp += tmp2;
    return (void*)tmp;
}

struct stack_struct *stack_copy(struct stack_struct *stack){
    struct stack_struct * copy = (struct stack_struct *) malloc(sizeof(struct stack_struct));
    copy->__capacity = stack->__capacity;
    copy->__size = stack->__size;
    copy->__stack = (int*) malloc(sizeof(int)*copy->__capacity);
    memcpy(copy->__stack,stack->__stack,sizeof(int)*copy->__size);
    return copy;
}

int stack_size_ptr(struct stack_struct *stack){
    return stack->__size/2;
}

int stack_eq(struct stack_struct * s1, struct stack_struct * s2){
    if(s1->__size != s2->__size) return 0;
    for(int i=0;i<s1->__size;i++) if(s1->__stack[i] != s2->__stack[i]) return 0;
    return 1;
}