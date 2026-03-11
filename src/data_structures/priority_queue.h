#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "huffman_tree.h"

/**
 * A min-heap based priority queue for Huffman tree construction.
 * Lower frequencies have higher priority (extracted first).
 */
typedef struct {
    HuffmanNode **nodes;      /* Array of node pointers */
    uint32_t size;            /* Current number of elements */
    uint32_t capacity;        /* Maximum capacity before reallocation */
} PriorityQueue;

/**
 * Create a new priority queue with initial capacity.
 * 
 * @param initial_capacity The initial capacity of the queue
 * @return Pointer to the newly created queue, or NULL on failure
 */
PriorityQueue* pq_create(uint32_t initial_capacity);

/**
 * Insert a node into the priority queue.
 * Maintains min-heap property (lower frequency has higher priority).
 * 
 * @param pq The priority queue
 * @param node The node to insert
 * @return 0 on success, -1 on failure
 */
int pq_insert(PriorityQueue *pq, HuffmanNode *node);

/**
 * Extract and remove the node with minimum frequency from the queue.
 * 
 * @param pq The priority queue
 * @return Pointer to the extracted node, or NULL if queue is empty
 */
HuffmanNode* pq_extract_min(PriorityQueue *pq);

/**
 * Check if the priority queue is empty.
 * 
 * @param pq The priority queue
 * @return 1 if empty, 0 if not empty
 */
int pq_is_empty(PriorityQueue *pq);

/**
 * Get the current number of elements in the priority queue.
 * 
 * @param pq The priority queue
 * @return The number of elements
 */
uint32_t pq_size(PriorityQueue *pq);

/**
 * Free all memory associated with the priority queue.
 * Note: This does NOT free the nodes themselves, only the queue structure.
 * 
 * @param pq The priority queue to free
 */
void pq_free(PriorityQueue *pq);

#endif /* PRIORITY_QUEUE_H */
