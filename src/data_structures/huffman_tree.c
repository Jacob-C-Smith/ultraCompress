#include "huffman_tree.h"
#include <stdlib.h>
#include <string.h>

/**
 * Create a new leaf node for a symbol with given frequency.
 */
HuffmanNode* huffman_node_create(uint8_t symbol, uint32_t frequency) {
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    node->is_leaf = 1;
    
    return node;
}

/**
 * Create a new internal node by combining two child nodes.
 */
HuffmanNode* huffman_node_create_internal(HuffmanNode *left, HuffmanNode *right) {
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->symbol = 0;  /* Internal nodes don't have symbols */
    node->frequency = left->frequency + right->frequency;
    node->left = left;
    node->right = right;
    node->is_leaf = 0;
    
    return node;
}

/**
 * Free all memory associated with a Huffman node and its descendants.
 */
void huffman_node_free(HuffmanNode *node) {
    if (node == NULL) {
        return;
    }
    
    huffman_node_free(node->left);
    huffman_node_free(node->right);
    free(node);
}

/**
 * Initialize a new Huffman tree structure.
 */
HuffmanTree* huffman_tree_create(void) {
    HuffmanTree *tree = (HuffmanTree *)malloc(sizeof(HuffmanTree));
    if (tree == NULL) {
        return NULL;
    }
    
    tree->root = NULL;
    tree->node_count = 0;
    
    return tree;
}

/**
 * Build a complete Huffman tree from an array of frequencies.
 * This function requires a priority queue to be implemented.
 */
HuffmanTree* huffman_tree_build(uint32_t frequencies[256]) {
    if (frequencies == NULL) {
        return NULL;
    }
    
    HuffmanTree *tree = huffman_tree_create();
    if (tree == NULL) {
        return NULL;
    }
    
    /* TODO: Implement tree building using priority queue */
    /* This will be completed once priority queue is implemented */
    
    return tree;
}

/**
 * Free all memory associated with a Huffman tree.
 */
void huffman_tree_free(HuffmanTree *tree) {
    if (tree == NULL) {
        return;
    }
    
    huffman_node_free(tree->root);
    free(tree);
}

/**
 * Generate Huffman codes for all symbols in the tree.
 */
int huffman_tree_generate_codes(HuffmanTree *tree, uint32_t codes[256], uint8_t code_lengths[256]) {
    if (tree == NULL || codes == NULL || code_lengths == NULL) {
        return -1;
    }
    
    /* TODO: Implement code generation */
    /* This will traverse the tree and generate bit patterns for each symbol */
    
    return 0;
}
