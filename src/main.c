#include "main.h"

// --------|
// globals |
// --------|

node_t* base_node;

// ----------|
// functions |
// ----------|

node_t* allocate_node(void* base_addr, uint64_t size) {

    if ((void*)base_addr == NULL) {
        printf("Address is NULL and thus cannot have\
                a node allocated at the location.\n");
        exit(1);
    }

    if ((size_t)size < sizeof(node_t)) {
        printf("The size of the given block is too small\
                to allocate the node struct to the address\
                without causing problems.\n");
        exit(1);
    }

    node_t* node = (node_t*) base_addr;

    printf("%p -> %p\n", base_addr, (void*)(base_addr + sizeof(node_t)));

    node->addr = (void*)(base_addr + sizeof(node_t));
    node->size = (uint64_t) (size - sizeof(node_t));
    node->prev = NULL;
    node->next = NULL;
    node->present = false;

    return node;

}

node_t* move_node(node_t* node, void* new_addr, uint64_t new_size) {
    void* addr = node->addr;
    node_t* next = node->next;
    node_t* prev = node->prev;
    bool present = node->present;

    node_t* new_node = (node_t*) new_addr;
    new_node->addr = (void*)(new_addr + sizeof(node_t));
    new_node->next = next;
    new_node->prev = prev;
    new_node->present = present;
    new_node->size = (new_size - sizeof(node_t));

    return new_node;
}

void* dmalloc(size_t size) {

    node_t* current_node = base_node;

    uint64_t true_size = (uint64_t)(size + sizeof(node_t));

    while (current_node->next != NULL) {
        if (current_node->size >= true_size)
            break;
    }

    if (current_node->size < true_size) {
        printf("Not enough free memory. Exiting.\n");
        exit(1);
    }

    if (current_node->size == true_size) {
        // Infect the current node to be in use and remove it from the empty list.
        // TODO
        printf("unimplemented");
    }

    void* base_addr = current_node->addr;

    // Resize the current block of memory.
    current_node = move_node(
                        current_node,
                        (void*)(current_node->addr + true_size),
                        (uint64_t)(current_node->size - true_size)
                    );


    node_t* new_node = allocate_node(base_addr, true_size);
    new_node->present = true;

    return new_node->addr;
}

int dfree(void* addr) {

    if (addr == NULL || (size_t)addr < sizeof(node_t)) {
        return 1;
    }

    node_t* returned_node = (node_t*) (void*)(addr - sizeof(node_t));

    printf("%p - %p\n", &returned_node, returned_node->addr);

    returned_node->present = false; // Segfaults...

    node_t* prev_node = base_node;


    while (base_node->addr < returned_node->addr) {
        if (prev_node->next != NULL) {
            prev_node = prev_node->next;
            continue;
        }

        prev_node->next = returned_node;
        returned_node->prev = prev_node;

        return 0;

    }

    if (prev_node->prev == NULL) {

        // Yes I know naming.
        prev_node->prev = returned_node;
        returned_node->next = prev_node;

        base_node = returned_node;

        return 0;
    }

    returned_node->prev = prev_node;

    if (returned_node->next != NULL) {
        returned_node->next = prev_node->next;

        node_t* next = returned_node->next;
        next->prev = returned_node;
    }

    prev_node->next = returned_node;

    return 0;
}

void defragment() {
    return;
}

void print_heap(bool everything) {
    if (!everything) {

        node_t* current_node = base_node;

        do {
            printf("Address: %x; Size: %lu\n", current_node->addr, current_node->size);
        } while (current_node->next != NULL);

        return;
    }

    // TODO
}

// ------|
// start |
// ------|

int main(int argc, char *argv[]) {

    uint64_t heap_size = getpagesize();

    // void *mmap(void addr, size_t length, int prot, int flags, int fd, off_t offset);
    void* mem_start = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (mem_start == MAP_FAILED) {
        printf("Memory failed to map... Exiting.\n");
        return 1;
    }

    printf("Memory mapped successfully.\n");

    base_node = allocate_node(mem_start, heap_size);

    printf("%lu\n", base_node->size);

    // Testing

    char* test = (char*)dmalloc(sizeof(char) * 4);
    test = "Hi!";

    char* test2 = (char*)dmalloc(sizeof(char) * 4);
    test2 = "Hi!";

    printf("test: %s\n", test);
    printf("test2: %s\n", test2);
    test2 = "EE!";
    printf("test2: %s\n", test2);

    //dfree(test);

    print_heap(false);

    // End testing

    // int munmap(void *addr, size_t len);
    int mum_r = munmap(mem_start, heap_size);

    if (mum_r == 1) {
        printf("Memory failed to unmap... Exiting.\n");
        return 1;
    }

    printf("Memory unmapped successfully.\n");

    return 0;
}