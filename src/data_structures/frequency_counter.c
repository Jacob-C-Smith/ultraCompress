#include "frequency_counter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Create a new frequency counter initialized to zero.
 */
FrequencyCounter* fc_create(void) {
    FrequencyCounter *counter = (FrequencyCounter *)malloc(sizeof(FrequencyCounter));
    if (counter == NULL) {
        return NULL;
    }
    
    /* Initialize all frequencies to 0 */
    memset(counter->frequencies, 0, sizeof(uint32_t) * 256);
    counter->total_symbols = 0;
    
    return counter;
}

/**
 * Count frequencies in a buffer of data.
 */
int fc_process_data(FrequencyCounter *counter, const uint8_t *data, uint64_t length) {
    if (counter == NULL || data == NULL) {
        return -1;
    }
    
    for (uint64_t i = 0; i < length; i++) {
        counter->frequencies[data[i]]++;
        counter->total_symbols++;
    }
    
    return 0;
}

/**
 * Count frequencies from a file.
 */
int fc_process_file(FrequencyCounter *counter, const char *filename) {
    if (counter == NULL || filename == NULL) {
        return -1;
    }
    
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }
    
    /* Read file in chunks to avoid loading entire file into memory */
    uint8_t buffer[8192];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (fc_process_data(counter, buffer, bytes_read) != 0) {
            fclose(file);
            return -1;
        }
    }
    
    if (ferror(file)) {
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;
}

/**
 * Get the frequency of a specific symbol.
 */
uint32_t fc_get_frequency(FrequencyCounter *counter, uint8_t symbol) {
    if (counter == NULL) {
        return 0;
    }
    
    return counter->frequencies[symbol];
}

/**
 * Get the total number of symbols processed.
 */
uint64_t fc_get_total_symbols(FrequencyCounter *counter) {
    if (counter == NULL) {
        return 0;
    }
    
    return counter->total_symbols;
}

/**
 * Reset the frequency counter to all zeros.
 */
int fc_reset(FrequencyCounter *counter) {
    if (counter == NULL) {
        return -1;
    }
    
    memset(counter->frequencies, 0, sizeof(uint32_t) * 256);
    counter->total_symbols = 0;
    
    return 0;
}

/**
 * Get a copy of the raw frequency array.
 */
int fc_get_frequencies(FrequencyCounter *counter, uint32_t out_frequencies[256]) {
    if (counter == NULL || out_frequencies == NULL) {
        return -1;
    }
    
    memcpy(out_frequencies, counter->frequencies, sizeof(uint32_t) * 256);
    
    return 0;
}

/**
 * Free all memory associated with the frequency counter.
 */
void fc_free(FrequencyCounter *counter) {
    if (counter == NULL) {
        return;
    }
    
    free(counter);
}
