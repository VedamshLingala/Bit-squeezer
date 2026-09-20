# BitSqueezer

BitSqueezer is a command-line C++ file compressor that uses Huffman coding to perform **lossless** compression. It supports arbitrary binary files, reconstructs the same deterministic Huffman tree during decompression, and verifies the expected decoded byte count.

## How it works

1. Scans the input in binary mode and counts byte frequencies.
2. Builds a Huffman tree with a min-priority queue. Ties are resolved by byte value, making the tree reproducible.
3. Stores a compact header containing the original size and only the byte frequencies that occur in the file.
4. Packs Huffman codes into bits and writes the compressed payload.
5. Rebuilds the tree from the header and emits exactly the original number of bytes while decompressing.

The `.bsq` format begins with the `BSQ1` magic value. It is designed for this project and is not compatible with ZIP or gzip.

## Build

### CMake

```bash
cmake -S . -B build
cmake --build build
```

### g++

```bash
g++ -std=c++14 -O2 main.cpp Huffman.cpp FrequencyCounter.cpp Node.cpp -o BitSqueezer
```

## Usage

```bash
# Compress
./BitSqueezer compress input.txt output.bsq

# Decompress
./BitSqueezer decompress output.bsq restored.txt
```

On Windows, use `BitSqueezer.exe` in place of `./BitSqueezer`.

The tool prints elapsed time, input/output size, and the compressed-to-original ratio. A ratio below `1.0` means the file became smaller.

## Complexity

For a file with `n` bytes and `k` distinct byte values (`k ≤ 256`):

- Frequency counting: `O(n)` time, `O(k)` space
- Tree construction: `O(k log k)` time, `O(k)` space
- Encoding and decoding: `O(n)` time, excluding the variable code-length factor

## Notes

- Compression is lossless: decompressing a valid `.bsq` file restores the original bytes exactly.
- Huffman coding is most effective on files with uneven byte distributions, such as natural-language text.
- Small files and already-compressed data (images, archives, videos) can grow because of the format header and the nature of lossless compression.
- The decoder rejects files with an invalid magic value, malformed frequency metadata, or an incomplete compressed payload.

## Project structure

| File | Purpose |
| --- | --- |
| `main.cpp` | Command-line interface and run statistics |
| `FrequencyCounter.*` | Binary-safe byte frequency counting |
| `Huffman.*` | Tree building, bit packing, header handling, and decoding |
| `Node.*` | Huffman-tree node representation |
