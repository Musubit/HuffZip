//
// Created by Musubi on 2026/1/18.
//

#ifndef HUFFMAN_COMPRESSOR_HPP
#define HUFFMAN_COMPRESSOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <chrono>
#include "HuffmanTree.hpp"
#include "BitStream.hpp"
#include "FileEntry.hpp"

class HuffmanCompressor {
public:
    // 压缩统计信息
    struct CompressionStats {
        size_t originalSize;      // 原始大小
        size_t compressedSize;    // 压缩后大小
        double compressionRatio;  // 压缩率
        double compressionTime;   // 压缩时间（秒）
    };

    // 构造函数
    HuffmanCompressor();
    ~HuffmanCompressor() = default;

    // 压缩单个文件
    void compressFile(const std::string& inputFile, const std::string& outputFile);

    // 压缩目录
    void compressDirectory(const std::string& inputDir, const std::string& outputFile);

    // 解压
    void decompress(const std::string& inputFile, const std::string& outputDir);

    // 获取统计信息
    CompressionStats getCompressionStats() const;

private:
    HuffmanTree huffmanTree_;
    CompressionStats stats_;

    // 文件头常量
    static const uint32_t MAGIC_NUMBER = 0x46465548;  // "HUFF"
    static const uint8_t VERSION = 1;

    // 内部方法
    std::unordered_map<char, size_t> calculateFrequency(const std::string& filePath);
    void writeHeader(std::ofstream& outFile, const std::string& inputPath,
                     size_t originalSize, size_t treeSize, size_t dataSize,
                     bool isDirectory);
    void readHeader(std::ifstream& inFile, std::string& originalPath,
                    size_t& originalSize, size_t& treeSize, size_t& dataSize,
                    bool& isDirectory);
    std::vector<FileEntry> traverseDirectory(const std::string& dirPath);
    void createDirectory(const std::string& dirPath);
    std::string getRelativePath(const std::string& basePath, const std::string& fullPath);
};

#endif // HUFFMAN_COMPRESSOR_HPP