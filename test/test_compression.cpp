//
// Created by Musubi on 2026/1/18.
//

#include "../include/HuffmanTree.hpp"
#include "../include/BitStream.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip>

std::vector<uint8_t> readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    return data;
}

std::unordered_map<char, size_t> calculateFrequency(const std::vector<uint8_t>& data) {
    std::unordered_map<char, size_t> freqMap;
    for (uint8_t byte : data) {
        freqMap[static_cast<char>(byte)]++;
    }
    return freqMap;
}

void testCompression() {
    std::cout << "Testing compression..." << std::endl;

    // 读取输入文件
    std::string inputFile = "../test/test_files/huffzip.txt";
    std::vector<uint8_t> inputData = readFile(inputFile);
    std::cout << "Original file size: " << inputData.size() << " bytes" << std::endl;

    // 统计字符频率
    std::unordered_map<char, size_t> freqMap = calculateFrequency(inputData);
    std::cout << "Unique characters: " << freqMap.size() << std::endl;

    // 构建哈夫曼树
    HuffmanTree tree;
    tree.buildTree(freqMap);

    // 生成编码表
    tree.generateCodes();
    const auto& encodingTable = tree.getEncodingTable();

    // 打印部分编码
    std::cout << "Sample Huffman codes:" << std::endl;
    int count = 0;
    for (const auto& pair : encodingTable) {
        if (count++ >= 10) break;
        std::cout << "  '" << pair.first << "': " << pair.second << std::endl;
    }

    // 序列化哈夫曼树
    std::vector<uint8_t> treeData = tree.serialize();
    std::cout << "Tree serialization size: " << treeData.size() << " bytes" << std::endl;

    // 压缩数据
    std::string outputFile = "../test/test_files/huffzip.huff";
    BitStream bitStream(outputFile, BitStream::Mode::WRITE);

    // 写入树的大小
    uint32_t treeSize = static_cast<uint32_t>(treeData.size());
    for (int i = 3; i >= 0; --i) {
        bitStream.writeByte((treeSize >> (i * 8)) & 0xFF);
    }

    // 写入树数据
    for (uint8_t byte : treeData) {
        bitStream.writeByte(byte);
    }

    // 写入原始数据大小
    uint32_t originalSize = static_cast<uint32_t>(inputData.size());
    for (int i = 3; i >= 0; --i) {
        bitStream.writeByte((originalSize >> (i * 8)) & 0xFF);
    }

    // 写入压缩数据
    for (uint8_t byte : inputData) {
        std::string code = tree.encode(static_cast<char>(byte));
        for (char bit : code) {
            bitStream.writeBit(bit == '1');
        }
    }

    bitStream.flush();
    bitStream.close();

    // 计算压缩率
    std::ifstream compressedFile(outputFile, std::ios::binary | std::ios::ate);
    size_t compressedSize = compressedFile.tellg();
    compressedFile.close();

    double compressionRatio = (1.0 - static_cast<double>(compressedSize) / inputData.size()) * 100.0;
    std::cout << "Compressed file size: " << compressedSize << " bytes" << std::endl;
    std::cout << "Compression ratio: " << std::fixed << std::setprecision(2) << compressionRatio << "%" << std::endl;

    assert(compressedSize > 0);
    std::cout << "Compression test passed!" << std::endl;
}

int main() {
    try {
        testCompression();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}