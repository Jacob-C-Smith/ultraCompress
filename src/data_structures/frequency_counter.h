#ifndef FREQUENCY_COUNTER_H
#define FREQUENCY_COUNTER_H

#include <stdint.h>

/**
 * Frequency counter for analyzing symbol distribution in input data.
 * Maintains a count for each of the 256 possible byte values.
 */
typedef struct {
    uint32_t frequencies[256]; /* Frequency count for each byte value (0-255) */
    uint64_t total_symbols;    /* Total number of symbols processed */
} FrequencyCounter;

/**
 * Create a new frequency counter initialized to zero.
 * 
 * @return Pointer to the newly created counter, or NULL on failure
 */
FrequencyCounter* fc_create(void);

/**
 * Count frequencies in a buffer of data.
 * Updates the frequency counter with occurrences of each byte.
 * 
 * @param counter The frequency counter to update
 * @param data The input data buffer
 * @param length The length of the input data in bytes
 * @return 0 on success, -1 on failure
 */
int fc_process_data(FrequencyCounter *counter, const uint8_t *data, uint64_t length);

/**
 * Count frequencies from a file.
 * 
 * @param counter The frequency counter to update
 * @param filename Path to the file to analyze
 * @return 0 on success, -1 on failure (e.g., file not found)
 */
int fc_process_file(FrequencyCounter *counter, const char *filename);

/**
 * Get the frequency of a specific symbol.
 * 
 * @param counter The frequency counter
 * @param symbol The symbol/byte value (0-255)
 * @return The frequency count for the symbol
 */
uint32_t fc_get_frequency(FrequencyCounter *counter, uint8_t symbol);

/**
 * Get the total number of symbols processed.
 * 
 * @param counter The frequency counter
 * @return The total symbol count
 */
uint64_t fc_get_total_symbols(FrequencyCounter *counter);

/**
 * Reset the frequency counter to all zeros.
 * 
 * @param counter The frequency counter to reset
 * @return 0 on success, -1 on failure
 */
int fc_reset(FrequencyCounter *counter);

/**
 * Get a copy of the raw frequency array.
 * Useful for building the Huffman tree.
 * 
 * @param counter The frequency counter
 * @param out_frequencies Output array to store frequencies (must be at least 256 uint32_t)
 * @return 0 on success, -1 on failure
 */
int fc_get_frequencies(FrequencyCounter *counter, uint32_t out_frequencies[256]);

/**
 * Free all memory associated with the frequency counter.
 * 
 * @param counter The counter to free
 */
void fc_free(FrequencyCounter *counter);

#endif /* FREQUENCY_COUNTER_H */
