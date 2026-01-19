//
// Created by Musubi on 2026/1/18.
//
#include "../include/HuffmanCompressor.hpp"
#include <iostream>
#include <string>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <command> <input> <output>" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  compress-file   - Compress a single file" << std::endl;
    std::cout << "  compress-dir    - Compress a directory" << std::endl;
    std::cout << "  decompress      - Decompress a file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " compress-file input.txt output.huff" << std::endl;
    std::cout << "  " << programName << " compress-dir mydir archive.huff" << std::endl;
    std::cout << "  " << programName << " decompress archive.huff outputdir" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];

    try {
        HuffmanCompressor compressor;

        if (command == "compress-file") {
            compressor.compressFile(input, output);
        } else if (command == "compress-dir") {
            compressor.compressDirectory(input, output);
        } else if (command == "decompress") {
            compressor.decompress(input, output);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printUsage(argv[0]);
            return 1;
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}