#pragma once

#include <sys/mman.h>
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    void* addr;
    uint64_t size;
    struct node* prev;
    struct node* next;
    bool present;
} node_t;

node_t* allocate_node(void* addr, uint64_t size);
