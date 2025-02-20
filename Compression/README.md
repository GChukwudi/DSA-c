# Huffman Coding File Compressor/Decompressor

## Description
This C program implements Huffman coding to compress and decompress files. It provides a simple command-line interface for users to compress any file and decompress previously compressed files.

## Features
- File compression using Huffman coding algorithm
- File decompression of Huffman-coded files
- Display of file sizes and compression ratio

## Compilation
To compile the program, use the following command:

gcc -o huffman huffman.c

## Usage
Run the compiled program:

./huffman

Follow the on-screen menu to:
1. Compress a file
2. Decompress a file
3. Exit the program

## File Handling
- When compressing, the program will create a file named "compressed.txt"
- When decompressing, the program reads from "compressed.txt" and creates "decompressed.txt"
- Ensure you have read/write permissions in the directory

## Notes
- Large files are handled efficiently
- The program calculates and displays original file size, compressed file size, and compression ratio

## Requirements
- C compiler (e.g., GCC)
- Standard C libraries