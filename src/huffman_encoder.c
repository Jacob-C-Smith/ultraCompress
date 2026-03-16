#include "huffman_encoder.h"
#include "data_structures/priority_queue.h"
#include <stdlib.h>
#include <string.h>

/**
 * Helper function: Traverse tree and assign codes using DFS.
 * Builds the encoding table by traversing the Huffman tree.
 */
static void assign_codes_recursive(HuffmanNode *node, HuffmanCode *codes, uint64_t code, uint8_t code_length) {
    if (node == NULL) return;
    
    /* If leaf node, store the code */
    if (node->is_leaf) {
        codes[node->symbol].symbol = node->symbol;
        codes[node->symbol].code = code;
        codes[node->symbol].code_length = code_length;
        return;
    }
    
    /* Traverse left (append 0) */
    if (node->left != NULL) {
        assign_codes_recursive(node->left, codes, code << 1, code_length + 1);
    }
    
    /* Traverse right (append 1) */
    if (node->right != NULL) {
        uint64_t right_code = (code << 1) | 1;
        assign_codes_recursive(node->right, codes, right_code, code_length + 1);
    }
}

/**
 * Create encoder and build Huffman tree.
 * Calculates frequencies, creates nodes, and builds tree.
 */
HuffmanEncoder* huffman_encoder_create(const uint8_t *data, uint64_t length) {
    if (data == NULL || length == 0) {
        return NULL;
    }
    
    HuffmanEncoder *encoder = (HuffmanEncoder*)malloc(sizeof(HuffmanEncoder));
    if (encoder == NULL) return NULL;
    
    /* STEP 1: Calculate frequencies */
    encoder->freqs = fc_create();
    if (encoder->freqs == NULL) {
        free(encoder);
        return NULL;
    }
    
    if (fc_process_data(encoder->freqs, data, length) != 0) {
        free(encoder->freqs);
        free(encoder);
        return NULL;
    }
    
    /* STEP 2 & 3: Create initial nodes and build tree */
    PriorityQueue *pq = pq_create(256);
    if (pq == NULL) {
        free(encoder->freqs);
        free(encoder);
        return NULL;
    }
    
    /* Create leaf nodes for each symbol that appears in data */
    for (int i = 0; i < 256; i++) {
        uint32_t freq = fc_get_frequency(encoder->freqs, i);
        if (freq > 0) {
            HuffmanNode *node = huffman_node_create(i, freq);
            if (node == NULL || pq_insert(pq, node) != 0) {
                /* Cleanup on failure */
                huffman_node_free(node);
                free(pq->nodes);
                free(pq);
                free(encoder->freqs);
                free(encoder);
                return NULL;
            }
        }
    }
    
    /* Build tree by combining nodes */
    while (pq->size > 1) {
        HuffmanNode *left = pq_extract_min(pq);
        HuffmanNode *right = pq_extract_min(pq);
        
        if (left == NULL || right == NULL) {
            /* Cleanup on failure */
            huffman_node_free(left);
            huffman_node_free(right);
            free(pq->nodes);
            free(pq);
            free(encoder->freqs);
            free(encoder);
            return NULL;
        }
        
        HuffmanNode *parent = huffman_node_create_internal(left, right);
        if (parent == NULL || pq_insert(pq, parent) != 0) {
            huffman_node_free(parent);
            free(pq->nodes);
            free(pq);
            free(encoder->freqs);
            free(encoder);
            return NULL;
        }
    }
    
    /* Extract root of tree */
    HuffmanNode *root = pq_extract_min(pq);
    
    encoder->tree = (HuffmanTree*)malloc(sizeof(HuffmanTree));
    if (encoder->tree == NULL) {
        huffman_node_free(root);
        free(pq->nodes);
        free(pq);
        free(encoder->freqs);
        free(encoder);
        return NULL;
    }
    
    encoder->tree->root = root;
    encoder->tree->node_count = 0; /* Not critical for our use */
    encoder->valid_codes = 0;
    
    /* Cleanup priority queue */
    free(pq->nodes);
    free(pq);
    
    return encoder;
}

/**
 * Assign codes by traversing tree.
 * Populates the encoding table with binary codes for each symbol.
 */
int huffman_encoder_assign_codes(HuffmanEncoder *encoder) {
    if (encoder == NULL || encoder->tree == NULL || encoder->tree->root == NULL) {
        return -1;
    }
    
    /* Initialize codes array */
    memset(encoder->codes, 0, sizeof(encoder->codes));
    
    /* Special case: single symbol in data */
    if (encoder->tree->root->is_leaf) {
        encoder->codes[encoder->tree->root->symbol].symbol = encoder->tree->root->symbol;
        encoder->codes[encoder->tree->root->symbol].code = 0;
        encoder->codes[encoder->tree->root->symbol].code_length = 1;
        encoder->valid_codes = 1;
        return 0;
    }
    
    /* Traverse tree and assign codes */
    assign_codes_recursive(encoder->tree->root, encoder->codes, 0, 0);
    encoder->valid_codes = 1;
    
    return 0;
}

/**
 * Encode (compress) data using Huffman codes.
 * Replaces each symbol with its binary code.
 */
int huffman_encode(HuffmanEncoder *encoder, const uint8_t *input,
                   uint64_t input_length, uint8_t *output, uint64_t output_size,
                   uint64_t *compressed_length) {
    if (encoder == NULL || !encoder->valid_codes || input == NULL || 
        output == NULL || compressed_length == NULL) {
        return -1;
    }
    
    uint64_t bit_pos = 0;      /* Current bit position in output */
    uint64_t byte_pos = 0;     /* Current byte position in output */
    uint8_t current_byte = 0;  /* Byte being built */
    
    for (uint64_t i = 0; i < input_length; i++) {
        uint8_t symbol = input[i];
        HuffmanCode code = encoder->codes[symbol];
        
        if (code.code_length == 0) {
            /* Symbol not found in tree */
            return -1;
        }
        
        /* Write code bits to output */
        for (int bit = code.code_length - 1; bit >= 0; bit--) {
            uint8_t bit_val = (code.code >> bit) & 1;
            current_byte = (current_byte << 1) | bit_val;
            bit_pos++;
            
            /* If byte is full, write it */
            if (bit_pos == 8) {
                if (byte_pos >= output_size) {
                    return -1; /* Output buffer too small */
                }
                output[byte_pos++] = current_byte;
                current_byte = 0;
                bit_pos = 0;
            }
        }
    }
    
    /* Write remaining partial byte */
    if (bit_pos > 0) {
        if (byte_pos >= output_size) {
            return -1;
        }
        /* Pad with zeros on the right */
        current_byte <<= (8 - bit_pos);
        output[byte_pos++] = current_byte;
    }
    
    *compressed_length = byte_pos;
    return 0;
}

/**
 * Decode (decompress) data using Huffman tree.
 * Follows tree using bitstream to recover original symbols.
 */
int huffman_decode(HuffmanEncoder *encoder, const uint8_t *input,
                   uint64_t input_length, uint8_t *output, uint64_t output_size,
                   uint64_t *decompressed_length) {
    if (encoder == NULL || encoder->tree == NULL || input == NULL || 
        output == NULL || decompressed_length == NULL) {
        return -1;
    }
    
    HuffmanNode *current = encoder->tree->root;
    uint64_t output_pos = 0;
    
    /* Special case: single symbol tree */
    if (current->is_leaf) {
        /* Not a typical case, but handle it */
        *decompressed_length = 0;
        return 0;
    }
    
    /* Traverse input bit by bit */
    for (uint64_t byte_pos = 0; byte_pos < input_length; byte_pos++) {
        uint8_t byte = input[byte_pos];
        
        for (int bit_pos = 7; bit_pos >= 0; bit_pos--) {
            uint8_t bit = (byte >> bit_pos) & 1;
            
            /* Follow tree based on bit */
            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }
            
            /* Check if we reached a leaf node */
            if (current == NULL) {
                return -1; /* Invalid code sequence */
            }
            
            if (current->is_leaf) {
                if (output_pos >= output_size) {
                    return -1; /* Output buffer too small */
                }
                output[output_pos++] = current->symbol;
                current = encoder->tree->root;
            }
        }
    }
    
    *decompressed_length = output_pos;
    return 0;
}

/**
 * Get the Huffman code for a specific symbol.
 */
HuffmanCode huffman_encoder_get_code(HuffmanEncoder *encoder, uint8_t symbol) {
    HuffmanCode empty = {0, 0, 0};
    if (encoder == NULL || !encoder->valid_codes) {
        return empty;
    }
    return encoder->codes[symbol];
}

/**
 * Free all memory associated with the encoder.
 */
void huffman_encoder_free(HuffmanEncoder *encoder) {
    if (encoder == NULL) return;
    
    if (encoder->tree != NULL) {
        if (encoder->tree->root != NULL) {
            huffman_node_free(encoder->tree->root);
        }
        free(encoder->tree);
    }
    
    if (encoder->freqs != NULL) {
        /* Assuming frequency counter has no dynamic allocation in its structure */
        free(encoder->freqs);
    }
    
    free(encoder);
}
