#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <stdint.h>

/**
 * Represents a node in the Huffman tree.
 * Leaf nodes contain symbol data, while internal nodes are used for tree construction.
 */
typedef struct HuffmanNode {
    uint8_t symbol;           /* The symbol/byte value (only meaningful for leaf nodes) */
    uint32_t frequency;       /* Frequency/probability weight of this node */
    struct HuffmanNode *left; /* Pointer to left child (0 bit) */
    struct HuffmanNode *right;/* Pointer to right child (1 bit) */
    int is_leaf;              /* Flag indicating if this is a leaf node */
} HuffmanNode;

/**
 * Represents the complete Huffman tree.
 */
typedef struct {
    HuffmanNode *root;        /* Root node of the tree */
    uint32_t node_count;      /* Total number of nodes in the tree */
} HuffmanTree;

/**
 * Create a new leaf node for a symbol with given frequency.
 * 
 * @param symbol The symbol/byte value
 * @param frequency The frequency of the symbol
 * @return Pointer to the newly created node, or NULL on failure
 */
HuffmanNode* huffman_node_create(uint8_t symbol, uint32_t frequency);

/**
 * Create a new internal node by combining two child nodes.
 * 
 * @param left The left child node
 * @param right The right child node
 * @return Pointer to the newly created internal node, or NULL on failure
 */
HuffmanNode* huffman_node_create_internal(HuffmanNode *left, HuffmanNode *right);

/**
 * Free all memory associated with a Huffman node and its descendants.
 * 
 * @param node The node to free
 */
void huffman_node_free(HuffmanNode *node);

/**
 * Initialize a new Huffman tree structure.
 * 
 * @return Pointer to the newly created tree, or NULL on failure
 */
HuffmanTree* huffman_tree_create(void);

/**
 * Build a complete Huffman tree from an array of frequencies.
 * 
 * @param frequencies Array of 256 uint32_t values representing symbol frequencies
 * @return Pointer to the constructed tree, or NULL on failure
 */
HuffmanTree* huffman_tree_build(uint32_t frequencies[256]);

/**
 * Free all memory associated with a Huffman tree.
 * 
 * @param tree The tree to free
 */
void huffman_tree_free(HuffmanTree *tree);

/**
 * Generate Huffman codes for all symbols in the tree.
 * Stores codes as bit patterns in an output array.
 * 
 * @param tree The Huffman tree
 * @param codes Output array where codes will be stored (256 entries for all possible bytes)
 * @param code_lengths Output array for code lengths (256 entries)
 * @return 0 on success, -1 on failure
 */
int huffman_tree_generate_codes(HuffmanTree *tree, uint32_t codes[256], uint8_t code_lengths[256]);

#endif /* HUFFMAN_TREE_H */
