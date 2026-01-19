//
// Created by Musubi on 2026/1/18.
//

#include "../include/HuffmanTree.hpp"
#include "../include/BitStream.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>

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

bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1.is_open() || !f2.is_open()) {
        return false;
    }

    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    if (f1.tellg() != f2.tellg()) {
        return false;
    }

    f1.seekg(0, std::ios::beg);
    f2.seekg(0, std::ios::beg);

    char c1, c2;
    while (f1.get(c1) && f2.get(c2)) {
        if (c1 != c2) {
            return false;
        }
    }

    return true;
}

void testDecompression() {
    std::cout << "Testing decompression..." << std::endl;

    // 读取压缩文件
    std::string compressedFile = "../test/test_files/huffzip.huff";
    BitStream bitStream(compressedFile, BitStream::Mode::READ);

    // 读取树的大小
    uint32_t treeSize = 0;
    for (int i = 0; i < 4; ++i) {
        treeSize = (treeSize << 8) | bitStream.readByte();
    }
    std::cout << "Tree size from file: " << treeSize << " bytes" << std::endl;

    // 读取树数据
    std::vector<uint8_t> treeData(treeSize);
    for (size_t i = 0; i < treeSize; ++i) {
        treeData[i] = bitStream.readByte();
    }

    // 反序列化哈夫曼树
    HuffmanTree tree;
    size_t offset = 0;
    tree.deserialize(treeData, offset);
    tree.generateCodes();

    std::cout << "Tree deserialized successfully" << std::endl;

    // 读取原始数据大小
    uint32_t originalSize = 0;
    for (int i = 0; i < 4; ++i) {
        originalSize = (originalSize << 8) | bitStream.readByte();
    }
    std::cout << "Original data size: " << originalSize << " bytes" << std::endl;

    // 解压数据
    std::vector<uint8_t> decompressedData;
    HuffmanNode* current = tree.getRoot();

    while (decompressedData.size() < originalSize) {
        bool bit = bitStream.readBit();

        if (bit) {
            current = current->getRight();
        } else {
            current = current->getLeft();
        }

        // 到达叶子节点
        if (current->isLeaf()) {
            decompressedData.push_back(static_cast<uint8_t>(current->getCharacter()));
            current = tree.getRoot();
        }
    }

    bitStream.close();

    std::cout << "Decompressed data size: " << decompressedData.size() << " bytes" << std::endl;

    // 写入解压后的文件
    std::string outputFile = "../test/test_files/huffzip_decompressed.txt";
    std::ofstream outFile(outputFile, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(decompressedData.data()), decompressedData.size());
    outFile.close();

    // 比较原始文件和解压后的文件
    std::string originalFile = "../test/test_files/huffzip.txt";
    bool match = compareFiles(originalFile, outputFile);

    if (match) {
        std::cout << "Decompression test passed! Files match perfectly." << std::endl;
    } else {
        std::cout << "Decompression test failed! Files do not match." << std::endl;
    }

    assert(match);
}

int main() {
    try {
        testDecompression();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}