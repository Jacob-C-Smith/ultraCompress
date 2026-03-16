#include "audio_io_mp3.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mpg123.h>
#include <lame/lame.h>

/**
 * Initialize MPG123 library.
 * Safe to call multiple times.
 */
static int mpg123_initialized = 0;

static int ensure_mpg123_init(void) {
    if (mpg123_initialized) return 0;
    
    int err = mpg123_init();
    if (err != MPG123_OK) {
        fprintf(stderr, "mpg123_init failed: %s\n", mpg123_plain_strerror(err));
        return -1;
    }
    
    mpg123_initialized = 1;
    return 0;
}

/**
 * Read an MP3 file and decode to PCM.
 */
AudioBuffer* audio_mp3_read(const char *filename) {
    if (filename == NULL) return NULL;
    
    if (ensure_mpg123_init() != 0) return NULL;
    
    int err = MPG123_OK;
    mpg123_handle *mh = mpg123_new(NULL, &err);
    if (mh == NULL) {
        fprintf(stderr, "mpg123_new failed: %s\n", mpg123_plain_strerror(err));
        return NULL;
    }
    
    /* Open the MP3 file */
    if (mpg123_open(mh, filename) != MPG123_OK) {
        fprintf(stderr, "Failed to open MP3 file: %s\n", mpg123_strerror(mh));
        mpg123_delete(mh);
        return NULL;
    }
    
    /* Get audio format information */
    long rate = 0;
    int channels = 0, encoding = 0;
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        fprintf(stderr, "Failed to get MP3 format: %s\n", mpg123_strerror(mh));
        mpg123_close(mh);
        mpg123_delete(mh);
        return NULL;
    }
    
    /* Calculate total number of samples */
    off_t samples = mpg123_length(mh);
    if (samples < 0) {
        fprintf(stderr, "Could not determine MP3 length\n");
        mpg123_close(mh);
        mpg123_delete(mh);
        return NULL;
    }
    
    /* Allocate output buffer */
    AudioBuffer *audio = (AudioBuffer*)malloc(sizeof(AudioBuffer));
    if (audio == NULL) {
        mpg123_close(mh);
        mpg123_delete(mh);
        return NULL;
    }
    
    audio->sample_count = samples;
    audio->channels = channels;
    audio->sample_rate = rate;
    
    /* Allocate memory for all samples */
    uint64_t total_bytes = samples * channels * sizeof(int16_t);
    audio->data = (int16_t*)malloc(total_bytes);
    if (audio->data == NULL) {
        free(audio);
        mpg123_close(mh);
        mpg123_delete(mh);
        return NULL;
    }
    
    /* Decode MP3 to PCM */
    size_t buffer_size = 32768; /* 32KB decode buffer */
    unsigned char *temp_buffer = (unsigned char*)malloc(buffer_size);
    if (temp_buffer == NULL) {
        free(audio->data);
        free(audio);
        mpg123_close(mh);
        mpg123_delete(mh);
        return NULL;
    }
    
    uint64_t written_samples = 0;
    size_t done = 0;
    
    while (mpg123_read(mh, temp_buffer, buffer_size, &done) == MPG123_OK) {
        if (done == 0) break;
        
        /* Convert decoded bytes to 16-bit samples */
        size_t sample_count = done / sizeof(int16_t);
        if (written_samples + sample_count > samples) {
            sample_count = samples - written_samples;
        }
        
        memcpy(audio->data + written_samples, temp_buffer, sample_count * sizeof(int16_t));
        written_samples += sample_count;
        
        if (written_samples >= samples) break;
    }
    
    free(temp_buffer);
    mpg123_close(mh);
    mpg123_delete(mh);
    
    if (written_samples < samples) {
        fprintf(stderr, "Warning: Decoded %lu samples, expected %lu\n", 
                written_samples, samples);
    }
    
    return audio;
}

/**
 * Get MP3 file information without decoding.
 */
int audio_mp3_get_info(const char *filename, AudioInfo *info) {
    if (filename == NULL || info == NULL) return -1;
    
    if (ensure_mpg123_init() != 0) return -1;
    
    int err = MPG123_OK;
    mpg123_handle *mh = mpg123_new(NULL, &err);
    if (mh == NULL) return -1;
    
    if (mpg123_open(mh, filename) != MPG123_OK) {
        mpg123_delete(mh);
        return -1;
    }
    
    long rate = 0;
    int channels = 0, encoding = 0;
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        mpg123_close(mh);
        mpg123_delete(mh);
        return -1;
    }
    
    off_t samples = mpg123_length(mh);
    
    info->sample_rate = rate;
    info->channels = channels;
    info->bit_depth = 16; /* MP3 decoded to 16-bit */
    info->total_samples = (samples > 0) ? samples : 0;
    
    mpg123_close(mh);
    mpg123_delete(mh);
    
    return 0;
}

/**
 * Encode PCM audio to MP3 file.
 */
int audio_mp3_write(const char *filename, const AudioBuffer *audio, int quality) {
    if (filename == NULL || audio == NULL) return -1;
    
    if (quality < 0) quality = 0;
    if (quality > 9) quality = 9;
    
    lame_t lame = lame_init();
    if (lame == NULL) {
        fprintf(stderr, "Failed to initialize LAME encoder\n");
        return -1;
    }
    
    /* Configure encoder */
    lame_set_in_samplerate(lame, audio->sample_rate);
    lame_set_num_channels(lame, audio->channels);
    lame_set_quality(lame, quality);
    
    if (lame_init_params(lame) < 0) {
        fprintf(stderr, "Failed to initialize LAME parameters\n");
        lame_close(lame);
        return -1;
    }
    
    /* Open output file */
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open output file: %s\n", filename);
        lame_close(lame);
        return -1;
    }
    
    /* Encode in chunks */
    const size_t chunk_samples = 4096;
    uint8_t mp3_buffer[32768];
    uint64_t pos = 0;
    
    while (pos < audio->sample_count) {
        size_t samples_to_encode = chunk_samples;
        if (pos + samples_to_encode > audio->sample_count) {
            samples_to_encode = audio->sample_count - pos;
        }
        
        /* Handle mono vs stereo */
        int encoded = 0;
        if (audio->channels == 2) {
            /* Interleaved stereo: [L, R, L, R, etc] */
            int16_t *left = (int16_t*)malloc(samples_to_encode * sizeof(int16_t));
            int16_t *right = (int16_t*)malloc(samples_to_encode * sizeof(int16_t));
            
            if (left == NULL || right == NULL) {
                free(left);
                free(right);
                fclose(fp);
                lame_close(lame);
                return -1;
            }
            
            /* Deinterleave samples */
            for (size_t i = 0; i < samples_to_encode; i++) {
                left[i] = audio->data[pos * 2 + i * 2];
                right[i] = audio->data[pos * 2 + i * 2 + 1];
            }
            
            encoded = lame_encode_buffer(lame, left, right, samples_to_encode, mp3_buffer, sizeof(mp3_buffer));
            free(left);
            free(right);
        } else {
            /* Mono */
            encoded = lame_encode_buffer(lame, audio->data + pos, NULL, samples_to_encode, mp3_buffer, sizeof(mp3_buffer));
        }
        
        if (encoded < 0) {
            fprintf(stderr, "LAME encoding error: %d\n", encoded);
            fclose(fp);
            lame_close(lame);
            return -1;
        }
        
        if (encoded > 0) {
            size_t written = fwrite(mp3_buffer, 1, encoded, fp);
            if (written != (size_t)encoded) {
                fprintf(stderr, "Failed to write MP3 data\n");
                fclose(fp);
                lame_close(lame);
                return -1;
            }
        }
        
        pos += samples_to_encode;
    }
    
    /* Flush remaining data */
    int final = lame_encode_flush(lame, mp3_buffer, sizeof(mp3_buffer));
    if (final < 0) {
        fprintf(stderr, "LAME flush error: %d\n", final);
        fclose(fp);
        lame_close(lame);
        return -1;
    }
    
    if (final > 0) {
        fwrite(mp3_buffer, 1, final, fp);
    }
    
    fclose(fp);
    lame_close(lame);
    
    return 0;
}

/**
 * Allocate a new audio buffer.
 */
AudioBuffer* audio_buffer_create(uint64_t sample_count, int channels, int sample_rate) {
    AudioBuffer *audio = (AudioBuffer*)malloc(sizeof(AudioBuffer));
    if (audio == NULL) return NULL;
    
    audio->data = (int16_t*)malloc(sample_count * channels * sizeof(int16_t));
    if (audio->data == NULL) {
        free(audio);
        return NULL;
    }
    
    audio->sample_count = sample_count;
    audio->channels = channels;
    audio->sample_rate = sample_rate;
    
    return audio;
}

/**
 * Free audio buffer.
 */
void audio_buffer_free(AudioBuffer *audio) {
    if (audio == NULL) return;
    
    if (audio->data != NULL) {
        free(audio->data);
    }
    free(audio);
}
