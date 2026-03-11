#include "../src/data_structures/huffman_tree.h"
#include "../src/data_structures/priority_queue.h"
#include "test_utils.h"
#include <stdio.h>

void test_priority_queue_creation(void) {
    printf("Testing priority queue creation...\n");
    
    PriorityQueue *pq = pq_create(10);
    ASSERT_NOT_NULL(pq);
    ASSERT_EQUAL(pq_size(pq), 0);
    ASSERT_TRUE(pq_is_empty(pq));
    
    pq_free(pq);
}

void test_priority_queue_insertion(void) {
    printf("Testing priority queue insertion...\n");
    
    PriorityQueue *pq = pq_create(10);
    
    HuffmanNode *node1 = huffman_node_create('a', 5);
    HuffmanNode *node2 = huffman_node_create('b', 3);
    HuffmanNode *node3 = huffman_node_create('c', 7);
    
    ASSERT_EQUAL(pq_insert(pq, node1), 0);
    ASSERT_EQUAL(pq_size(pq), 1);
    
    ASSERT_EQUAL(pq_insert(pq, node2), 0);
    ASSERT_EQUAL(pq_size(pq), 2);
    
    ASSERT_EQUAL(pq_insert(pq, node3), 0);
    ASSERT_EQUAL(pq_size(pq), 3);
    
    ASSERT_FALSE(pq_is_empty(pq));
    
    pq_free(pq);
    huffman_node_free(node1);
    huffman_node_free(node2);
    huffman_node_free(node3);
}

void test_priority_queue_extraction_order(void) {
    printf("Testing priority queue extraction order...\n");
    
    PriorityQueue *pq = pq_create(10);
    
    /* Insert nodes with different frequencies */
    HuffmanNode *node1 = huffman_node_create('a', 5);
    HuffmanNode *node2 = huffman_node_create('b', 3);
    HuffmanNode *node3 = huffman_node_create('c', 7);
    HuffmanNode *node4 = huffman_node_create('d', 1);
    
    pq_insert(pq, node1);
    pq_insert(pq, node2);
    pq_insert(pq, node3);
    pq_insert(pq, node4);
    
    /* Extract should return in order of frequency: 1, 3, 5, 7 */
    HuffmanNode *min1 = pq_extract_min(pq);
    ASSERT_EQUAL(min1->frequency, 1);
    ASSERT_EQUAL(pq_size(pq), 3);
    
    HuffmanNode *min2 = pq_extract_min(pq);
    ASSERT_EQUAL(min2->frequency, 3);
    ASSERT_EQUAL(pq_size(pq), 2);
    
    HuffmanNode *min3 = pq_extract_min(pq);
    ASSERT_EQUAL(min3->frequency, 5);
    ASSERT_EQUAL(pq_size(pq), 1);
    
    HuffmanNode *min4 = pq_extract_min(pq);
    ASSERT_EQUAL(min4->frequency, 7);
    ASSERT_TRUE(pq_is_empty(pq));
    
    pq_free(pq);
}

void test_priority_queue_empty_extraction(void) {
    printf("Testing priority queue extraction from empty queue...\n");
    
    PriorityQueue *pq = pq_create(10);
    
    HuffmanNode *node = pq_extract_min(pq);
    ASSERT_NULL(node);
    
    pq_free(pq);
}

void test_priority_queue_dynamic_resize(void) {
    printf("Testing priority queue dynamic resizing...\n");
    
    PriorityQueue *pq = pq_create(2);  /* Small initial capacity */
    
    /* Insert more nodes than initial capacity */
    for (int i = 0; i < 10; i++) {
        HuffmanNode *node = huffman_node_create('a' + i, i + 1);
        ASSERT_EQUAL(pq_insert(pq, node), 0);
    }
    
    ASSERT_EQUAL(pq_size(pq), 10);
    
    /* Verify heap property by extracting all */
    uint32_t prev_freq = 0;
    while (!pq_is_empty(pq)) {
        HuffmanNode *node = pq_extract_min(pq);
        ASSERT_TRUE(node->frequency >= prev_freq);
        prev_freq = node->frequency;
    }
    
    pq_free(pq);
}

void test_priority_queue_invalid_operations(void) {
    printf("Testing priority queue invalid operations...\n");
    
    /* NULL pointer operations */
    ASSERT_EQUAL(pq_insert(NULL, NULL), -1);
    ASSERT_NULL(pq_extract_min(NULL));
    ASSERT_TRUE(pq_is_empty(NULL));
    ASSERT_EQUAL(pq_size(NULL), 0);
    
    pq_free(NULL);  /* Should not crash */
}

int main(void) {
    printf("======= PRIORITY QUEUE TESTS =======\n\n");
    
    test_priority_queue_creation();
    test_priority_queue_insertion();
    test_priority_queue_extraction_order();
    test_priority_queue_empty_extraction();
    test_priority_queue_dynamic_resize();
    test_priority_queue_invalid_operations();
    
    print_test_summary();
    
    return EXIT_SUCCESS;
}
