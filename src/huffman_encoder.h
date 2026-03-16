#ifndef HUFFMAN_ENCODER_H
#define HUFFMAN_ENCODER_H

#include <stdint.h>
#include "data_structures/huffman_tree.h"
#include "data_structures/frequency_counter.h"

/**
 * Represents a Huffman code for a single symbol.
 * The code is stored as a bit sequence with a length.
 */
typedef struct {
    uint8_t symbol;           /* The symbol this code represents */
    uint64_t code;            /* The actual code bits (up to 64 bits) */
    uint8_t code_length;      /* Number of bits in the code (1-64) */
} HuffmanCode;

/**
 * Huffman encoder/decoder for audio compression.
 * Contains the tree, frequency data, and encoding table.
 */
typedef struct {
    HuffmanTree *tree;        /* The Huffman tree */
    FrequencyCounter *freqs;  /* Symbol frequencies */
    HuffmanCode codes[256];   /* Encoding table for all 256 symbols */
    int valid_codes;          /* Flag indicating if code table is populated */
} HuffmanEncoder;

/**
 * Create encoder by calculating frequencies and building tree.
 * 
 * @param data Input audio data buffer
 * @param length Length of data in bytes
 * @return Pointer to the encoder, or NULL on failure
 */
HuffmanEncoder* huffman_encoder_create(const uint8_t *data, uint64_t length);

/**
 * Generate the encoding table by traversing the tree.
 * Must be called after encoder creation.
 * 
 * @param encoder The Huffman encoder
 * @return 0 on success, -1 on failure
 */
int huffman_encoder_assign_codes(HuffmanEncoder *encoder);

/**
 * Encode (compress) audio data.
 * 
 * @param encoder The Huffman encoder with valid codes
 * @param input Input audio data
 * @param input_length Length of input data
 * @param output Output buffer for compressed data
 * @param output_size Size of output buffer
 * @param compressed_length Pointer to store actual compressed length
 * @return 0 on success, -1 on failure
 */
int huffman_encode(HuffmanEncoder *encoder, const uint8_t *input, uint64_t input_length, uint8_t *output, uint64_t output_size, uint64_t *compressed_length);

/**
 * Decode (decompress) audio data.
 * 
 * @param encoder The Huffman encoder with valid tree
 * @param input Compressed data
 * @param input_length Length of compressed data
 * @param output Output buffer for decompressed data
 * @param output_size Size of output buffer
 * @param decompressed_length Pointer to store actual decompressed length
 * @return 0 on success, -1 on failure
 */
int huffman_decode(HuffmanEncoder *encoder, const uint8_t *input, uint64_t input_length, uint8_t *output, uint64_t output_size, uint64_t *decompressed_length);

/**
 * Get the Huffman code for a specific symbol.
 * 
 * @param encoder The Huffman encoder
 * @param symbol The symbol to get code for
 * @return The HuffmanCode, or zeros if not found
 */
HuffmanCode huffman_encoder_get_code(HuffmanEncoder *encoder, uint8_t symbol);

/**
 * Free all memory associated with the encoder.
 * 
 * @param encoder The encoder to free
 */
void huffman_encoder_free(HuffmanEncoder *encoder);

#endif /* HUFFMAN_ENCODER_H */ 
