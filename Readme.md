# Huffman Compression Tool

A fast and efficient file compression utility implementing the Huffman coding algorithm in C++. This command-line tool can compress and decompress any file type while providing real-time progress tracking.

## Authors

- **Moamen Eslam** 
- **Karim Amr**

## Features

- **Universal Compression**: Works with any file type (text, images, executables, etc.)
- **Huffman Algorithm**: Lossless compression using optimal prefix codes
- **Progress Tracking**: Real-time progress percentage and ETA display
- **Configurable Buffer**: Adjustable buffer size for performance optimization
- **Fast Processing**: Efficient implementation with buffered I/O operations

## How It Works

The program uses the Huffman coding algorithm for lossless data compression:

1. **Frequency Analysis**: Counts the occurrence of each byte in the input file
2. **Tree Construction**: Builds a binary tree based on byte frequencies
3. **Code Generation**: Assigns variable-length binary codes to each byte
4. **Compression**: Replaces original bytes with their Huffman codes
5. **Decompression**: Uses the stored tree to reconstruct the original file

## Installation

### Prerequisites

- C++ compiler with C++11 support (g++, clang++, MSVC)
- Standard C++ libraries

### Compilation

```bash
# Linux/Mac
g++ -o huffman main.cpp -std=c++11

# Windows (MSVC)
cl /EHsc main.cpp /Fe:huffman.exe

# Windows (MinGW)
g++ -o huffman.exe main.cpp -std=c++11
```

## Usage

### Basic Commands

```bash
# Compress a file
./huffman -c input.txt

# Decompress a file
./huffman -d input.txt.ece2103

# Change buffer size (in bytes)
./huffman -b 8192 -c largefile.bin
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `-c <file>` | Compress the specified file |
| `-d <file>` | Decompress the specified file |
| `-b <size>` | Set buffer size in bytes (default: 4096) |

### Examples

```bash
# Compress a text document
./huffman -c document.txt
# Output: document.txt.ece2103

# Compress an image with larger buffer
./huffman -b 16384 -c photo.png
# Output: photo.png.ece2103

# Decompress a file
./huffman -d photo.png.ece2103
# Output: photo.png
```

## File Format

Compressed files use the `.ece2103` extension and contain:

1. **Frequency Table**: 256 long long values (one for each possible byte)
2. **Huffman Codes**: Variable-length codes for each byte
3. **Compressed Data**: Bit-packed encoded data

## Performance

### Buffer Size Recommendations

- **Small files (< 1MB)**: 4KB buffer (default)
- **Medium files (1-100MB)**: 8-16KB buffer
- **Large files (> 100MB)**: 32-64KB buffer

### Compression Ratios

Typical compression ratios vary by file type:

- **Text files**: 40-60% of original size
- **Source code**: 35-55% of original size
- **Already compressed files** (ZIP, JPEG, MP3): ~100% (no improvement)

## Implementation Details

### Data Structures

- **Custom Linked List**: For building the priority queue
- **Binary Tree**: For storing Huffman codes
- **Frequency Array**: 256-element array for byte counts

### Key Functions

- `CountFrequencies()`: Analyzes input file and builds frequency table
- `BuildHuffmanTree()`: Constructs the Huffman tree from frequencies
- `GenerateCodes()`: Recursively generates binary codes
- `CompressFile()`: Encodes and writes compressed data
- `DecompressFile()`: Decodes compressed data back to original

## Limitations

- Maximum file size: Limited by available memory and file system
- Buffer size: Must be positive integer
- Compressed files must maintain the `.ece2103` extension for automatic decompression

## Error Handling

The program handles various error conditions:

- File not found or inaccessible
- Insufficient memory for buffers
- Corrupted compressed files
- Invalid command-line arguments

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This project is created for educational purposes as part of the ECE2103 course.

## Acknowledgments

- AI assistance was used for specific implementation details (bit manipulation and binary I/O operations)
- Huffman coding algorithm originally developed by David A. Huffman in 1952

## Contact

For questions or issues, please contact:
- Moamen Eslam 

---

**Note**: This tool is designed for educational purposes to demonstrate the Huffman compression algorithm. For production use, consider established compression libraries like zlib or LZMA.