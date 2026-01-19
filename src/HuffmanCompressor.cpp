//
// Created by Musubi on 2026/1/18.
//
#include "../include/HuffmanCompressor.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

const uint32_t HuffmanCompressor::MAGIC_NUMBER;
const uint8_t HuffmanCompressor::VERSION;

// 构造函数
HuffmanCompressor::HuffmanCompressor()
    : stats_{0, 0, 0.0, 0.0} {
}

// 压缩单个文件
void HuffmanCompressor::compressFile(const std::string& inputFile,
                                     const std::string& outputFile) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // 验证输入
    if (inputFile.empty() || outputFile.empty()) {
        throw std::invalid_argument("File paths cannot be empty");
    }

    // 检查输入文件
    if (!std::filesystem::exists(inputFile)) {
        throw std::runtime_error("Input file does not exist: " + inputFile);
    }

    // 统计字符频率
    std::unordered_map<char, size_t> frequencyMap = calculateFrequency(inputFile);

    // 构建哈夫曼树
    huffmanTree_.buildTree(frequencyMap);
    huffmanTree_.generateCodes();

    // 序列化哈夫曼树
    std::vector<uint8_t> treeData = huffmanTree_.serialize();

    // 获取原始文件大小
    size_t originalSize = std::filesystem::file_size(inputFile);

    // 写入文件头和哈夫曼树
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open output file: " + outputFile);
    }

    // 写入文件头（单文件模式）
    std::string inputFileName = std::filesystem::path(inputFile).filename().string();
    writeHeader(outFile, inputFileName, originalSize, treeData.size(), 0, false);

    // 写入哈夫曼树
    outFile.write(reinterpret_cast<const char*>(treeData.data()), treeData.size());

    // 压缩并写入数据
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        outFile.close();
        throw std::runtime_error("Failed to open input file: " + inputFile);
    }

    // 使用缓冲区收集压缩的位
    uint8_t buffer = 0;
    size_t bitPosition = 0;

    char ch;
    while (inFile.get(ch)) {
        std::string code = huffmanTree_.encode(ch);
        for (char bit : code) {
            if (bit == '1') {
                buffer |= (1 << (7 - bitPosition));
            }
            bitPosition++;

            if (bitPosition == 8) {
                outFile.write(reinterpret_cast<const char*>(&buffer), 1);
                buffer = 0;
                bitPosition = 0;
            }
        }
    }

    // 写入最后不完整的字节
    if (bitPosition > 0) {
        outFile.write(reinterpret_cast<const char*>(&buffer), 1);
    }

    inFile.close();
    outFile.close();

    // 计算统计信息
    auto endTime = std::chrono::high_resolution_clock::now();
    stats_.originalSize = std::filesystem::file_size(inputFile);
    stats_.compressedSize = std::filesystem::file_size(outputFile);
    stats_.compressionRatio = (static_cast<double>(stats_.compressedSize) / stats_.originalSize) * 100.0;
    stats_.compressionTime = std::chrono::duration<double>(endTime - startTime).count();

    std::cout << "Compression completed!" << std::endl;
    std::cout << "Original size: " << stats_.originalSize << " bytes" << std::endl;
    std::cout << "Compressed size: " << stats_.compressedSize << " bytes" << std::endl;
    std::cout << "Compression ratio: " << stats_.compressionRatio << "%" << std::endl;
    std::cout << "Compression time: " << stats_.compressionTime << " seconds" << std::endl;
}

// 压缩目录
void HuffmanCompressor::compressDirectory(const std::string& inputDir,
                                          const std::string& outputFile) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // 验证输入
    if (!std::filesystem::exists(inputDir)) {
        throw std::runtime_error("Input directory does not exist: " + inputDir);
    }

    if (!std::filesystem::is_directory(inputDir)) {
        throw std::invalid_argument("Input path is not a directory: " + inputDir);
    }

    // 遍历目录
    std::vector<FileEntry> fileEntries = traverseDirectory(inputDir);

    // 统计所有文件的字符频率
    std::unordered_map<char, size_t> totalFrequencyMap;
    for (const auto& entry : fileEntries) {
        if (!entry.isDirectory()) {
            std::string fullPath = inputDir + "/" + entry.getRelativePath();
            auto freqMap = calculateFrequency(fullPath);
            for (const auto& pair : freqMap) {
                totalFrequencyMap[pair.first] += pair.second;
            }
        }
    }

    // 构建哈夫曼树
    huffmanTree_.buildTree(totalFrequencyMap);
    huffmanTree_.generateCodes();

    // 序列化哈夫曼树
    std::vector<uint8_t> treeData = huffmanTree_.serialize();

    // 写入压缩文件
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open output file: " + outputFile);
    }

    // 写入文件头
    size_t totalOriginalSize = 0;
    for (const auto& entry : fileEntries) {
        totalOriginalSize += entry.getFileSize();
    }

    writeHeader(outFile, inputDir, totalOriginalSize, treeData.size(), 0, true);

    // 写入哈夫曼树
    outFile.write(reinterpret_cast<const char*>(treeData.data()), treeData.size());

    // 写入文件条目数量
    uint32_t entryCount = static_cast<uint32_t>(fileEntries.size());
    outFile.write(reinterpret_cast<const char*>(&entryCount), 4);

    // 写入文件条目
    for (const auto& entry : fileEntries) {
        auto entryData = entry.serialize();
        outFile.write(reinterpret_cast<const char*>(entryData.data()), entryData.size());
    }

    // 压缩并写入文件数据
    for (const auto& entry : fileEntries) {
        if (!entry.isDirectory()) {
            std::string fullPath = inputDir + "/" + entry.getRelativePath();

            BitStream bitStream(outputFile, BitStream::Mode::WRITE);
            std::ifstream inFile(fullPath, std::ios::binary);

            char ch;
            while (inFile.get(ch)) {
                std::string code = huffmanTree_.encode(ch);
                for (char bit : code) {
                    bitStream.writeBit(bit == '1');
                }
            }

            bitStream.flush();
            bitStream.close();
        }
    }

    outFile.close();

    // 计算统计信息
    auto endTime = std::chrono::high_resolution_clock::now();
    stats_.originalSize = totalOriginalSize;
    stats_.compressedSize = std::filesystem::file_size(outputFile);
    stats_.compressionRatio = (static_cast<double>(stats_.compressedSize) / stats_.originalSize) * 100.0;
    stats_.compressionTime = std::chrono::duration<double>(endTime - startTime).count();

    std::cout << "Directory compression completed!" << std::endl;
    std::cout << "Files compressed: " << fileEntries.size() << std::endl;
    std::cout << "Original size: " << stats_.originalSize << " bytes" << std::endl;
    std::cout << "Compressed size: " << stats_.compressedSize << " bytes" << std::endl;
    std::cout << "Compression ratio: " << stats_.compressionRatio << "%" << std::endl;
    std::cout << "Compression time: " << stats_.compressionTime << " seconds" << std::endl;
}

// 解压
void HuffmanCompressor::decompress(const std::string& inputFile,
                                   const std::string& outputDir) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // 验证输入
    if (!std::filesystem::exists(inputFile)) {
        throw std::runtime_error("Input file does not exist: " + inputFile);
    }

    // 创建输出目录
    createDirectory(outputDir);

    // 读取文件头
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open input file: " + inputFile);
    }

    std::string originalPath;
    size_t originalSize, treeSize, dataSize;
    bool isDirectory;

    readHeader(inFile, originalPath, originalSize, treeSize, dataSize, isDirectory);

    // 读取哈夫曼树
    std::vector<uint8_t> treeData(treeSize);
    inFile.read(reinterpret_cast<char*>(treeData.data()), treeSize);

    size_t offset = 0;
    huffmanTree_.deserialize(treeData, offset);

    if (isDirectory) {
        // 读取文件条目数量
        uint32_t entryCount;
        inFile.read(reinterpret_cast<char*>(&entryCount), 4);

        // 读取文件条目
        std::vector<FileEntry> fileEntries;
        for (uint32_t i = 0; i < entryCount; ++i) {
            FileEntry entry;
            std::vector<uint8_t> entryData;

            // 读取条目大小（这里简化处理，实际应该先读取大小）
            // 实际实现中需要更复杂的逻辑来确定条目边界
            offset = 0;
            entry.deserialize(entryData, offset);
            fileEntries.push_back(entry);
        }

        // 解压文件
        BitStream bitStream(inputFile, BitStream::Mode::READ);
        HuffmanNode* root = huffmanTree_.getRoot();

        for (const auto& entry : fileEntries) {
            if (entry.isDirectory()) {
                std::string dirPath = outputDir + "/" + entry.getRelativePath();
                createDirectory(dirPath);
            } else {
                std::string filePath = outputDir + "/" + entry.getRelativePath();
                std::string dirPath = filePath.substr(0, filePath.find_last_of('/'));
                createDirectory(dirPath);

                std::ofstream outFile(filePath, std::ios::binary);

                HuffmanNode* current = root;
                size_t bytesWritten = 0;

                while (bytesWritten < entry.getFileSize()) {
                    bool bit = bitStream.readBit();

                    if (bit) {
                        current = current->getRight();
                    } else {
                        current = current->getLeft();
                    }

                    if (current->isLeaf()) {
                        outFile.put(current->getCharacter());
                        current = root;
                        bytesWritten++;
                    }
                }

                outFile.close();
            }
        }

        bitStream.close();
    } else {
        // 单文件解压
        HuffmanNode* root = huffmanTree_.getRoot();

        // 创建输出文件
        std::ofstream outFile(outputDir + "/" + originalPath, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to create output file: " + outputDir + "/" + originalPath);
        }

        HuffmanNode* current = root;
        size_t bytesWritten = 0;

        // 从 inFile 当前位置读取压缩数据
        uint8_t buffer = 0;
        size_t bitPosition = 0;

        while (bytesWritten < originalSize) {
            // 读取下一个字节（如果需要）
            if (bitPosition == 0) {
                if (!inFile.read(reinterpret_cast<char*>(&buffer), 1)) {
                    throw std::runtime_error("Unexpected end of file while decompressing");
                }
            }

            // 读取一个位（从高位到低位）
            bool bit = (buffer & (1 << (7 - bitPosition))) != 0;
            bitPosition++;
            if (bitPosition == 8) {
                bitPosition = 0;
            }

            // 遍历哈夫曼树
            if (bit) {
                current = current->getRight();
            } else {
                current = current->getLeft();
            }

            if (current->isLeaf()) {
                outFile.put(current->getCharacter());
                current = root;
                bytesWritten++;
            }
        }

        outFile.close();
    }

    inFile.close();

    // 计算统计信息
    auto endTime = std::chrono::high_resolution_clock::now();
    stats_.originalSize = originalSize;
    stats_.compressedSize = std::filesystem::file_size(inputFile);
    stats_.compressionRatio = (static_cast<double>(stats_.compressedSize) / stats_.originalSize) * 100.0;
    stats_.compressionTime = std::chrono::duration<double>(endTime - startTime).count();

    std::cout << "Decompression completed!" << std::endl;
    std::cout << "Decompressed size: " << stats_.originalSize << " bytes" << std::endl;
    std::cout << "Decompression time: " << stats_.compressionTime << " seconds" << std::endl;
}

// 获取统计信息
HuffmanCompressor::CompressionStats HuffmanCompressor::getCompressionStats() const {
    return stats_;
}

// 计算字符频率
std::unordered_map<char, size_t> HuffmanCompressor::calculateFrequency(const std::string& filePath) {
    std::unordered_map<char, size_t> frequencyMap;

    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    char ch;
    while (inFile.get(ch)) {
        frequencyMap[ch]++;
    }

    inFile.close();

    return frequencyMap;
}

// 写入文件头
void HuffmanCompressor::writeHeader(std::ofstream& outFile, const std::string& inputPath,
                                     size_t originalSize, size_t treeSize, size_t dataSize,
                                     bool isDirectory) {
    // 魔数（4字节）
    outFile.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), 4);

    // 版本号（1字节）
    outFile.write(reinterpret_cast<const char*>(&VERSION), 1);

    // 原始大小（8字节）
    uint64_t originalSize64 = static_cast<uint64_t>(originalSize);
    outFile.write(reinterpret_cast<const char*>(&originalSize64), 8);

    // 树大小（4字节）
    uint32_t treeSize32 = static_cast<uint32_t>(treeSize);
    outFile.write(reinterpret_cast<const char*>(&treeSize32), 4);

    // 数据大小（4字节）
    uint32_t dataSize32 = static_cast<uint32_t>(dataSize);
    outFile.write(reinterpret_cast<const char*>(&dataSize32), 4);

    // 类型标志（1字节）
    uint8_t typeFlag = isDirectory ? 1 : 0;
    outFile.write(reinterpret_cast<const char*>(&typeFlag), 1);

    // 文件名长度（2字节）
    uint16_t pathLength = static_cast<uint16_t>(inputPath.length());
    outFile.write(reinterpret_cast<const char*>(&pathLength), 2);

    // 文件名（变长）
    outFile.write(inputPath.c_str(), pathLength);

    // 保留字段（2字节，从4字节改为2字节以平衡文件名长度）
    uint16_t reserved = 0;
    outFile.write(reinterpret_cast<const char*>(&reserved), 2);
}

// 读取文件头
void HuffmanCompressor::readHeader(std::ifstream& inFile, std::string& originalPath,
                                   size_t& originalSize, size_t& treeSize, size_t& dataSize,
                                   bool& isDirectory) {
    // 读取魔数
    uint32_t magic;
    inFile.read(reinterpret_cast<char*>(&magic), 4);
    if (magic != MAGIC_NUMBER) {
        throw std::runtime_error("Invalid magic number");
    }

    // 读取版本号
    uint8_t version;
    inFile.read(reinterpret_cast<char*>(&version), 1);
    if (version != VERSION) {
        throw std::runtime_error("Unsupported version: " + std::to_string(version));
    }

    // 读取原始大小
    uint64_t originalSize64;
    inFile.read(reinterpret_cast<char*>(&originalSize64), 8);
    originalSize = static_cast<size_t>(originalSize64);

    // 读取树大小
    uint32_t treeSize32;
    inFile.read(reinterpret_cast<char*>(&treeSize32), 4);
    treeSize = static_cast<size_t>(treeSize32);

    // 读取数据大小
    uint32_t dataSize32;
    inFile.read(reinterpret_cast<char*>(&dataSize32), 4);
    dataSize = static_cast<size_t>(dataSize32);

    // 读取类型标志
    uint8_t typeFlag;
    inFile.read(reinterpret_cast<char*>(&typeFlag), 1);
    isDirectory = (typeFlag != 0);

    // 读取文件名长度
    uint16_t pathLength;
    inFile.read(reinterpret_cast<char*>(&pathLength), 2);

    // 读取文件名
    std::vector<char> pathBuffer(pathLength + 1, '\0');
    inFile.read(pathBuffer.data(), pathLength);
    originalPath = std::string(pathBuffer.data());

    // 跳过保留字段
    uint16_t reserved;
    inFile.read(reinterpret_cast<char*>(&reserved), 2);
}

// 遍历目录
std::vector<FileEntry> HuffmanCompressor::traverseDirectory(const std::string& dirPath) {
    std::vector<FileEntry> fileEntries;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
        std::string relativePath = getRelativePath(dirPath, entry.path().string());

        if (entry.is_directory()) {
            fileEntries.emplace_back(relativePath, 0, true);
        } else {
            fileEntries.emplace_back(relativePath, entry.file_size(), false);
        }
    }

    return fileEntries;
}

// 创建目录
void HuffmanCompressor::createDirectory(const std::string& dirPath) {
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
    }
}

// 获取相对路径
std::string HuffmanCompressor::getRelativePath(const std::string& basePath,
                                                const std::string& fullPath) {
    // 标准化路径
    std::filesystem::path base(basePath);
    std::filesystem::path full(fullPath);

    std::filesystem::path relative = std::filesystem::relative(full, base);
    return relative.string();
}