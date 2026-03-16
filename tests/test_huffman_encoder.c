#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/huffman_encoder.h"

/**
 * Test program demonstrating Huffman coding for audio compression.
 * Shows each step of the algorithm.
 */

void print_huffman_codes(HuffmanEncoder *encoder) {
    printf("\n===== HUFFMAN CODES =====\n");
    printf("Symbol | Frequency | Code (binary)\n");
    printf("-------|-----------|---------------\n");
    
    for (int i = 0; i < 256; i++) {
        uint32_t freq = fc_get_frequency(encoder->freqs, i);
        if (freq > 0) {
            HuffmanCode code = huffman_encoder_get_code(encoder, i);
            if (code.code_length > 0) {
                printf(" 0x%02X  |  %7u  | ", i, freq);
                for (int bit = code.code_length - 1; bit >= 0; bit--) {
                    printf("%ld", (code.code >> bit) & 1);
                }
                printf(" (%d bits)\n", code.code_length);
            }
        }
    }
}

int main(void) {
    printf("======================================\n");
    printf("  HUFFMAN CODING - AUDIO COMPRESSION\n");
    printf("======================================\n\n");
    
    /* Create sample audio data (simulating 16-bit audio samples as bytes) */
    uint8_t audio_data[] = {
        0x0F, 0x1F, 0x2F, 0x3F, 0x0F, 0x1F, 0x2F, 0x3F,
        0x0F, 0x1F, 0x2F, 0x3F, 0x0F, 0x1F, 0x2F, 0x3F,
        0x0A, 0x0A, 0x0B, 0x0B, 0x0A, 0x0A, 0x0B, 0x0B,
        0x0A, 0x0A, 0x0B, 0x0B, 0x0A, 0x0A, 0x0B, 0x0B
    };
    uint64_t audio_length = sizeof(audio_data);
    
    printf("Calculating frequencies and building Huffman tree...\n");
    HuffmanEncoder *encoder = huffman_encoder_create(audio_data, audio_length);
    if (encoder == NULL) {
        fprintf(stderr, "Failed to create encoder\n");
        return 1;
    }
    printf("Tree constructed with %lu unique symbols\n", encoder->freqs->total_symbols);
    
    printf("\n");
    printf("Symbol Frequencies from audio data:\n");
    printf("--------\n");
    int unique_symbols = 0;
    for (int i = 0; i < 256; i++) {
        uint32_t freq = fc_get_frequency(encoder->freqs, i);
        if (freq > 0) {
            printf("  Symbol 0x%02X: %u occurrences\n", i, freq);
            unique_symbols++;
        }
    }
    printf("Total unique symbols: %d\n", unique_symbols);
    
    printf("\nAssigning binary codes to each symbol...\n");
    if (huffman_encoder_assign_codes(encoder) != 0) {
        fprintf(stderr, "Failed to assign codes\n");
        huffman_encoder_free(encoder);
        return 1;
    }
    printf("Codes assigned\n");
    
    print_huffman_codes(encoder);
    
    printf("\nEncoding (compressing) audio data...\n");
    uint8_t compressed_buffer[1024];
    uint64_t compressed_length;
    if (huffman_encode(encoder, audio_data, audio_length, 
                       compressed_buffer, sizeof(compressed_buffer), 
                       &compressed_length) != 0) {
        fprintf(stderr, "Failed to encode\n");
        huffman_encoder_free(encoder);
        return 1;
    }
    printf("Compression complete\n");
    printf("  Original size:   %lu bytes\n", audio_length);
    printf("  Compressed size: %lu bytes\n", compressed_length);
    printf("  Compression ratio: %.2f%%\n", 
           (1.0 - (double)compressed_length / audio_length) * 100.0);
    
    printf("\nDecoding (decompressing) audio data...\n");
    uint8_t decompressed_buffer[1024];
    uint64_t decompressed_length;
    if (huffman_decode(encoder, compressed_buffer, compressed_length,
                       decompressed_buffer, sizeof(decompressed_buffer),
                       &decompressed_length) != 0) {
        fprintf(stderr, "Failed to decode\n");
        huffman_encoder_free(encoder);
        return 1;
    }
    printf("Decompression complete\n");
    printf("  Decompressed size: %lu bytes\n", decompressed_length);
    
    /* Verify decompression */
    if (decompressed_length == audio_length &&
        memcmp(audio_data, decompressed_buffer, audio_length) == 0) {
        printf("  Verification PASSED - Decompressed data matches original!\n");
    } else {
        printf("  Verification FAILED - Data mismatch!\n");
        huffman_encoder_free(encoder);
        return 1;
    }
    
    huffman_encoder_free(encoder);
    
    printf("\n======================================\n");
    printf("  All steps completed successfully!\n");
    printf("======================================\n");
    
    return 0;
}
