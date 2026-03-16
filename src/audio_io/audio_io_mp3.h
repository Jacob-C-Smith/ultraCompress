#ifndef AUDIO_IO_MP3_H
#define AUDIO_IO_MP3_H

#include <stdint.h>

/**
 * Audio frame information from MP3 file.
 * Represents the audio metadata needed for processing.
 */
typedef struct {
    int sample_rate;      /* Samples per second (Hz) */
    int channels;         /* Number of audio channels (1 = mono, 2 = stereo) */
    int bit_depth;        /* Bits per sample (typically 16) */
    uint64_t total_samples; /* Total number of samples in file */
} AudioInfo;

/**
 * Audio data buffer containing raw PCM samples.
 * Samples are stored as 16-bit signed integers.
 */
typedef struct {
    int16_t *data;        /* PCM audio samples */
    uint64_t sample_count; /* Number of samples (per channel) */
    int channels;         /* Number of channels */
    int sample_rate;      /* Sample rate in Hz */
} AudioBuffer;

/**
 * Read an MP3 file and decode it to PCM audio.
 * 
 * @param filename Path to the MP3 file
 * @return Allocated AudioBuffer with decoded PCM data, or NULL on failure
 */
AudioBuffer* audio_mp3_read(const char *filename);

/**
 * Get information about an MP3 file without decoding it.
 * 
 * @param filename Path to the MP3 file
 * @param info Pointer to AudioInfo structure to fill
 * @return 0 on success, -1 on failure
 */
int audio_mp3_get_info(const char *filename, AudioInfo *info);

/**
 * Encode PCM audio to MP3 file.
 * 
 * @param filename Output MP3 file path
 * @param audio AudioBuffer containing PCM data
 * @param quality LAME quality setting (0-9, lower=better, higher=faster)
 * @return 0 on success, -1 on failure
 */
int audio_mp3_write(const char *filename, const AudioBuffer *audio, int quality);

/**
 * Free audio buffer memory.
 * 
 * @param audio The AudioBuffer to free
 */
void audio_buffer_free(AudioBuffer *audio);

/**
 * Allocate a new audio buffer.
 * 
 * @param sample_count Number of samples per channel
 * @param channels Number of channels
 * @param sample_rate Sample rate in Hz
 * @return Allocated AudioBuffer, or NULL on failure
 */
AudioBuffer* audio_buffer_create(uint64_t sample_count, int channels, int sample_rate);

#endif // AUDIO_IO_MP3_H
