# # Text File Compression Algorithms in C++

This repository contains implementations of popular lossless Text File compression algorithms in C++, including:

- **Huffman Coding**
- **Lempel–Ziv 77 (LZ77)**
- **Lempel–Ziv 78 (LZ78)**

These algorithms are fundamental to understanding how modern compression techniques work.

## Algorithms Overview

### 1. Huffman Coding
A variable-length coding algorithm that assigns shorter codes to frequent characters and longer codes to infrequent characters.  
Used in file compression and transmission systems like JPEG and MP3.

### 2. LZ77
A sliding-window algorithm that replaces repeating sequences with a `(offset, length, next_char)` triplet.  
Basis of many real-world compressors like **DEFLATE (ZIP/PNG)**.

### 3. LZ78
Builds an explicit dictionary of substrings and outputs `(index, next_char)` pairs.  
Foundation for formats like **LZW (GIF)**.

## Key Differences Between LZ77 and LZ78

| Feature                | **LZ77**                   | **LZ78**                   |
|------------------------|-----------------------------|-----------------------------|
| Uses sliding window    | ✅ Yes                     | ❌ No                      |
| Builds dictionary      | Implicitly                 | Explicitly                 |
| Output format          | `(offset, length, nextChar)` | `(index, nextChar)`        |
| Best for               | Repeated patterns in nearby positions | General patterns across the text |


## Features
- Compression and decompression (text input)
- Clean, modular C++ implementations
- Commented code for learning
- Simple command-line interfaces

