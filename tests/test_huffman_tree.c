#include "../src/data_structures/huffman_tree.h"
#include "test_utils.h"
#include <stdio.h>

void test_huffman_node_creation_leaf(void) {
    printf("Testing Huffman leaf node creation...\n");
    
    HuffmanNode *node = huffman_node_create('a', 10);
    ASSERT_NOT_NULL(node);
    ASSERT_EQUAL(node->symbol, 'a');
    ASSERT_EQUAL(node->frequency, 10);
    ASSERT_NULL(node->left);
    ASSERT_NULL(node->right);
    ASSERT_TRUE(node->is_leaf);
    
    huffman_node_free(node);
}

void test_huffman_node_creation_internal(void) {
    printf("Testing Huffman internal node creation...\n");
    
    HuffmanNode *left = huffman_node_create('a', 5);
    HuffmanNode *right = huffman_node_create('b', 3);
    
    HuffmanNode *parent = huffman_node_create_internal(left, right);
    ASSERT_NOT_NULL(parent);
    ASSERT_EQUAL(parent->frequency, 8);  /* 5 + 3 */
    ASSERT_EQUAL(parent->left, left);
    ASSERT_EQUAL(parent->right, right);
    ASSERT_FALSE(parent->is_leaf);
    
    huffman_node_free(parent);
}

void test_huffman_tree_creation(void) {
    printf("Testing Huffman tree creation...\n");
    
    HuffmanTree *tree = huffman_tree_create();
    ASSERT_NOT_NULL(tree);
    ASSERT_NULL(tree->root);
    ASSERT_EQUAL(tree->node_count, 0);
    
    huffman_tree_free(tree);
}

void test_huffman_node_free_leaf(void) {
    printf("Testing Huffman leaf node freeing...\n");
    
    HuffmanNode *node = huffman_node_create('a', 10);
    huffman_node_free(node);  /* Should not crash */
    
    ASSERT_TRUE(1);  /* If we reach here, test passed */
}

void test_huffman_node_free_tree(void) {
    printf("Testing Huffman tree node freeing...\n");
    
    /* Build a small tree */
    HuffmanNode *left = huffman_node_create('a', 5);
    HuffmanNode *right = huffman_node_create('b', 3);
    HuffmanNode *parent = huffman_node_create_internal(left, right);
    
    huffman_node_free(parent);  /* Should recursively free all nodes */
    
    ASSERT_TRUE(1);  /* If we reach here, test passed */
}

void test_huffman_node_invalid_operations(void) {
    printf("Testing Huffman node invalid operations...\n");
    
    huffman_node_free(NULL);  /* Should not crash */
    
    HuffmanTree *tree = huffman_tree_create();
    huffman_tree_free(tree);  /* Should not crash */
    
    ASSERT_TRUE(1);
}

void test_huffman_tree_multiple_nodes(void) {
    printf("Testing Huffman tree with multiple nodes...\n");
    
    /* Create a more complex tree */
    HuffmanNode *n1 = huffman_node_create('a', 5);
    HuffmanNode *n2 = huffman_node_create('b', 3);
    HuffmanNode *p1 = huffman_node_create_internal(n1, n2);
    
    HuffmanNode *n3 = huffman_node_create('c', 2);
    HuffmanNode *n4 = huffman_node_create('d', 1);
    HuffmanNode *p2 = huffman_node_create_internal(n3, n4);
    
    HuffmanNode *root = huffman_node_create_internal(p1, p2);
    
    /* Verify structure */
    ASSERT_EQUAL(root->frequency, 11);  /* 5+3+2+1 */
    ASSERT_FALSE(root->is_leaf);
    ASSERT_NOT_NULL(root->left);
    ASSERT_NOT_NULL(root->right);
    
    huffman_node_free(root);
}

int main(void) {
    printf("======= HUFFMAN TREE TESTS =======\n\n");
    
    test_huffman_node_creation_leaf();
    test_huffman_node_creation_internal();
    test_huffman_tree_creation();
    test_huffman_node_free_leaf();
    test_huffman_node_free_tree();
    test_huffman_node_invalid_operations();
    test_huffman_tree_multiple_nodes();
    
    print_test_summary();
    
    return EXIT_SUCCESS;
}
