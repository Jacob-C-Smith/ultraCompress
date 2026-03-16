#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../src/audio_io/audio_io_mp3.h"
#include "../src/huffman_encoder.h"

/**
 * Test program for MP3 I/O and audio compression.
 * Demonstrates reading MP3, extracting PCM, compressing with Huffman, and writing back.
 */

void print_audio_info(const AudioInfo *info) {
    printf("\nAudio Information:\n");
    printf("  Sample Rate:    %d Hz\n", info->sample_rate);
    printf("  Channels:       %d (%s)\n", info->channels, 
           info->channels == 1 ? "Mono" : info->channels == 2 ? "Stereo" : "Multi-channel");
    printf("  Bit Depth:      %d bits\n", info->bit_depth);
    printf("  Total Samples:  %lu\n", info->total_samples);
    
    double duration = (double)info->total_samples / info->sample_rate;
    int minutes = (int)duration / 60;
    int seconds = (int)duration % 60;
    printf("  Duration:       %d:%02d (%.2f seconds)\n", minutes, seconds, duration);
}

int main(int argc, char *argv[]) {
    const char *input_file = "test_audio.mp3";
    
    printf("=====================================\n");
    printf("  MP3 AUDIO I/O & HUFFMAN COMPRESSION\n");
    printf("=====================================\n\n");
    
    /* Get MP3 file information */
    printf("Reading MP3 file information...\n");
    printf("File: %s\n", input_file);
    
    AudioInfo info;
    if (audio_mp3_get_info(input_file, &info) != 0) {
        fprintf(stderr, "Failed to read MP3 file info\n");
        return 1;
    }
    print_audio_info(&info);
    
    /* Decode MP3 to PCM */
    printf("\nDecoding MP3 to PCM audio...\n");
    AudioBuffer *audio = audio_mp3_read(input_file);
    if (audio == NULL) {
        fprintf(stderr, "Failed to decode MP3 file\n");
        return 1;
    }
    printf("Decoded %lu samples (%lu bytes)\n", 
           audio->sample_count, audio->sample_count * audio->channels * sizeof(int16_t));
    
    /* Convert PCM samples to bytes for Huffman compression */
    printf("\nPreparing audio data for Huffman compression...\n");
    uint64_t pcm_size = audio->sample_count * audio->channels * sizeof(int16_t);
    uint8_t *pcm_bytes = (uint8_t *)audio->data;
    printf("PCM Data: %lu bytes\n", pcm_size);
    
    /* Create Huffman encoder and analyze frequencies */
    printf("\nBuilding Huffman encoder...\n");
    HuffmanEncoder *encoder = huffman_encoder_create(pcm_bytes, pcm_size);
    if (encoder == NULL) {
        fprintf(stderr, "Failed to create Huffman encoder\n");
        audio_buffer_free(audio);
        return 1;
    }
    printf("Huffman tree constructed\n");
    
    /* Assign codes */
    printf("\nAssigning Huffman codes...\n");
    if (huffman_encoder_assign_codes(encoder) != 0) {
        fprintf(stderr, "Failed to assign codes\n");
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    printf("Codes assigned\n");
    
    /* Compress PCM data */
    printf("\nCompressing PCM data with Huffman...\n");
    uint8_t *compressed_buffer = (uint8_t*)malloc(pcm_size * 2); /* Allocate extra space */
    if (compressed_buffer == NULL) {
        fprintf(stderr, "Failed to allocate compression buffer\n");
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    
    uint64_t compressed_size;
    if (huffman_encode(encoder, pcm_bytes, pcm_size, compressed_buffer, pcm_size * 2, &compressed_size) != 0) {
        fprintf(stderr, "Failed to compress data\n");
        free(compressed_buffer);
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    
    printf("Compression complete\n");
    printf("  Original size:   %lu bytes\n", pcm_size);
    printf("  Compressed size: %lu bytes\n", compressed_size);
    double compression_ratio = (1.0 - (double)compressed_size / pcm_size) * 100.0;
    printf("  Compression ratio: %.2f%%\n", compression_ratio);
    printf("  Space saved: %lu bytes\n", pcm_size - compressed_size);
    
    /* Decompress and verify */
    printf("\nDecompressing and verifying...\n");
    uint8_t *decompressed_buffer = (uint8_t*)malloc(pcm_size);
    if (decompressed_buffer == NULL) {
        fprintf(stderr, "Failed to allocate decompression buffer\n");
        free(compressed_buffer);
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    
    uint64_t decompressed_size;
    if (huffman_decode(encoder, compressed_buffer, compressed_size, decompressed_buffer, pcm_size, &decompressed_size) != 0) {
        fprintf(stderr, "Failed to decompress data\n");
        free(decompressed_buffer);
        free(compressed_buffer);
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    
    printf("Decompression complete\n");
    printf("  Decompressed size: %lu bytes\n", decompressed_size);
    
    /* Verify decompression */
    if (decompressed_size == pcm_size && 
        memcmp(pcm_bytes, decompressed_buffer, pcm_size) == 0) {
        printf("VERIFICATION PASSED - Perfect reconstruction!\n");
    } else {
        printf("VERIFICATION FAILED - Data mismatch!\n");
        free(decompressed_buffer);
        free(compressed_buffer);
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    
    /* Encode compressed data back to MP3 */
    printf("\nEncoding decompressed audio back to MP3...\n");
    AudioBuffer *decompressed_audio = audio_buffer_create(audio->sample_count, audio->channels, audio->sample_rate);
    if (decompressed_audio == NULL) {
        fprintf(stderr, "Failed to create decompressed audio buffer\n");
        free(decompressed_buffer);
        free(compressed_buffer);
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    
    memcpy(decompressed_audio->data, decompressed_buffer, pcm_size);
    
    const char *output_file = "test_audio_huffman_compressed.mp3";
    if (audio_mp3_write(output_file, decompressed_audio, 2) != 0) {
        fprintf(stderr, "Failed to write output MP3 file\n");
        audio_buffer_free(decompressed_audio);
        free(decompressed_buffer);
        free(compressed_buffer);
        huffman_encoder_free(encoder);
        audio_buffer_free(audio);
        return 1;
    }
    printf("Output file written: %s\n", output_file);
    
    /* Summary */
    printf("\n=====================================\n");
    printf("  COMPRESSION SUMMARY\n");
    printf("=====================================\n");
    printf("Input Audio:  %s\n", input_file);
    printf("Output Audio: %s\n", output_file);
    printf("\nPCM Data Compression (Huffman):\n");
    printf("  Original:   %lu bytes\n", pcm_size);
    printf("  Compressed: %lu bytes\n", compressed_size);
    printf("  Ratio:      %.2f%% reduction\n", compression_ratio);
    printf("\nTotal Samples: %lu\n", audio->sample_count);
    printf("Sample Rate:   %d Hz\n", audio->sample_rate);
    printf("Channels:      %d\n", audio->channels);
    printf("\nAll steps completed successfully!\n");
    printf("=====================================\n");
    
    /* Cleanup */
    audio_buffer_free(decompressed_audio);
    free(decompressed_buffer);
    free(compressed_buffer);
    huffman_encoder_free(encoder);
    audio_buffer_free(audio);
    
    return 0;
}
