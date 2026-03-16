# MP3 Audio I/O & Huffman Compression

## Overview
This is an overview to the MP3 file I/O (reading and writing) combined with Huffman compression for audio PCM data in this project. The implementation successfully reads MP3 files, extracts PCM audio, applies Huffman compression, and can write the decompressed audio back to MP3 format.

## Features

### Audio File Operations
- **MP3 Decoding**: Converts MP3 files to PCM 16-bit audio using libmpg123
- **MP3 Encoding**: Encodes PCM audio back to MP3 format using libmp3lame
- **Format Information**: Queries MP3 file metadata (sample rate, channels, duration, etc)

### Huffman Compression
- **Tree Construction**: Analyzes PCM byte frequencies and builds optimal Huffman tree
- **Code Assignment**: Generates unique binary codes for each byte value
- **Lossless Compression**: Encodes PCM data with perfect reconstruction
- **Decompression**: Uses tree traversal to recover original audio data

## Test Results

### Real Audio Test (test_audio.mp3)
```
Input File:           test_audio.mp3
Duration:             7.04 seconds (310,480 samples)
Sample Rate:          44,100 Hz
Channels:             Stereo (2)
Bit Depth:            16 bits

PCM Data:             1,241,920 bytes
Huffman Compressed:   547,254 bytes
Compression Ratio:    55.93% reduction
Space Saved:          694,666 bytes

Output File:          test_audio_huffman_compressed.mp3
Verification:         PASSED - Perfect reconstruction
```

This demonstrates that Huffman compression achieves significant reduction (nearly 56%) on real audio data without any loss of fidelity.

## Implementation Details

### Libraries Used
- **libmpg123**: High-performance MP3 decoder
- **libmp3lame**: MP3 encoder (MPEG Audio Encoder)
- **Data Structures**: Priority Queue, Huffman Tree, Frequency Counter

## Compilation

### Requirements
```bash
sudo apt-get install libmpg123-dev libmp3lame-dev
```

### Build
```bash
cd tests
make test_mp3_io
```

### Run
```bash
./test_mp3_io
```
