#include "priority_queue.h"
#include <stdlib.h>
#include <string.h>

/* Helper functions for min-heap operations */
static void pq_swap(HuffmanNode **a, HuffmanNode **b) {
    HuffmanNode *temp = *a;
    *a = *b;
    *b = temp;
}

static uint32_t pq_parent(uint32_t i) {
    return (i - 1) / 2;
}

static uint32_t pq_left_child(uint32_t i) {
    return 2 * i + 1;
}

static uint32_t pq_right_child(uint32_t i) {
    return 2 * i + 2;
}

/* Bubble up operation for insertion */
static void pq_heapify_up(PriorityQueue *pq, uint32_t index) {
    while (index > 0) {
        uint32_t parent = pq_parent(index);
        
        if (pq->nodes[index]->frequency < pq->nodes[parent]->frequency) {
            pq_swap(&pq->nodes[index], &pq->nodes[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

/* Bubble down operation for extraction */
static void pq_heapify_down(PriorityQueue *pq, uint32_t index) {
    while (1) {
        uint32_t smallest = index;
        uint32_t left = pq_left_child(index);
        uint32_t right = pq_right_child(index);
        
        if (left < pq->size && pq->nodes[left]->frequency < pq->nodes[smallest]->frequency) {
            smallest = left;
        }
        
        if (right < pq->size && pq->nodes[right]->frequency < pq->nodes[smallest]->frequency) {
            smallest = right;
        }
        
        if (smallest != index) {
            pq_swap(&pq->nodes[index], &pq->nodes[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

/**
 * Create a new priority queue with initial capacity.
 */
PriorityQueue* pq_create(uint32_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = 256;  /* Default capacity */
    }
    
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (pq == NULL) {
        return NULL;
    }
    
    pq->nodes = (HuffmanNode **)malloc(initial_capacity * sizeof(HuffmanNode *));
    if (pq->nodes == NULL) {
        free(pq);
        return NULL;
    }
    
    pq->size = 0;
    pq->capacity = initial_capacity;
    
    return pq;
}

/**
 * Insert a node into the priority queue.
 */
int pq_insert(PriorityQueue *pq, HuffmanNode *node) {
    if (pq == NULL || node == NULL) {
        return -1;
    }
    
    /* Resize if necessary */
    if (pq->size >= pq->capacity) {
        uint32_t new_capacity = pq->capacity * 2;
        HuffmanNode **new_nodes = (HuffmanNode **)realloc(pq->nodes, new_capacity * sizeof(HuffmanNode *));
        
        if (new_nodes == NULL) {
            return -1;
        }
        
        pq->nodes = new_nodes;
        pq->capacity = new_capacity;
    }
    
    /* Add node at the end and bubble up */
    pq->nodes[pq->size] = node;
    pq_heapify_up(pq, pq->size);
    pq->size++;
    
    return 0;
}

/**
 * Extract and remove the node with minimum frequency from the queue.
 */
HuffmanNode* pq_extract_min(PriorityQueue *pq) {
    if (pq == NULL || pq->size == 0) {
        return NULL;
    }
    
    HuffmanNode *min_node = pq->nodes[0];
    
    if (pq->size > 1) {
        /* Move last element to root and bubble down */
        pq->nodes[0] = pq->nodes[pq->size - 1];
        pq_heapify_down(pq, 0);
    }
    
    pq->size--;
    
    return min_node;
}

/**
 * Check if the priority queue is empty.
 */
int pq_is_empty(PriorityQueue *pq) {
    if (pq == NULL) {
        return 1;
    }
    
    return pq->size == 0;
}

/**
 * Get the current number of elements in the priority queue.
 */
uint32_t pq_size(PriorityQueue *pq) {
    if (pq == NULL) {
        return 0;
    }
    
    return pq->size;
}

/**
 * Free all memory associated with the priority queue.
 */
void pq_free(PriorityQueue *pq) {
    if (pq == NULL) {
        return;
    }
    
    free(pq->nodes);
    free(pq);
}
