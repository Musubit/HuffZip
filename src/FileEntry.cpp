//
// Created by Musubi on 2026/1/18.
//
#include "../include/FileEntry.hpp"
#include <stdexcept>

// 构造函数
FileEntry::FileEntry()
    : fileSize_(0)
    , compressedSize_(0)
    , isDirectory_(false) {
}

FileEntry::FileEntry(const std::string& relativePath, size_t fileSize, bool isDirectory)
    : relativePath_(relativePath)
    , fileSize_(fileSize)
    , compressedSize_(0)
    , isDirectory_(isDirectory) {
}

// 序列化
std::vector<uint8_t> FileEntry::serialize() const {
    std::vector<uint8_t> data;

    // 路径长度（2字节）
    if (relativePath_.size() > 65535) {
        throw std::runtime_error("Path too long");
    }

    uint16_t pathLength = static_cast<uint16_t>(relativePath_.size());
    data.push_back(static_cast<uint8_t>(pathLength >> 8));
    data.push_back(static_cast<uint8_t>(pathLength & 0xFF));

    // 路径字符串
    data.insert(data.end(), relativePath_.begin(), relativePath_.end());

    // 文件大小（8字节）
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((fileSize_ >> (i * 8)) & 0xFF));
    }

    // 压缩大小（8字节）
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((compressedSize_ >> (i * 8)) & 0xFF));
    }

    // 目录标志（1字节）
    data.push_back(isDirectory_ ? 1 : 0);

    return data;
}

// 反序列化
void FileEntry::deserialize(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset + 2 > data.size()) {
        throw std::runtime_error("Insufficient data for path length");
    }

    // 读取路径长度
    uint16_t pathLength = (static_cast<uint16_t>(data[offset]) << 8) |
                          static_cast<uint16_t>(data[offset + 1]);
    offset += 2;

    // 读取路径字符串
    if (offset + pathLength > data.size()) {
        throw std::runtime_error("Insufficient data for path");
    }

    relativePath_.assign(data.begin() + offset, data.begin() + offset + pathLength);
    offset += pathLength;

    // 读取文件大小（8字节）
    if (offset + 8 > data.size()) {
        throw std::runtime_error("Insufficient data for file size");
    }

    fileSize_ = 0;
    for (int i = 0; i < 8; ++i) {
        fileSize_ = (fileSize_ << 8) | data[offset++];
    }

    // 读取压缩大小（8字节）
    if (offset + 8 > data.size()) {
        throw std::runtime_error("Insufficient data for compressed size");
    }

    compressedSize_ = 0;
    for (int i = 0; i < 8; ++i) {
        compressedSize_ = (compressedSize_ << 8) | data[offset++];
    }

    // 读取目录标志（1字节）
    if (offset >= data.size()) {
        throw std::runtime_error("Insufficient data for directory flag");
    }

    isDirectory_ = (data[offset++] != 0);
}

// Getter 方法
const std::string& FileEntry::getRelativePath() const {
    return relativePath_;
}

size_t FileEntry::getFileSize() const {
    return fileSize_;
}

bool FileEntry::isDirectory() const {
    return isDirectory_;
}

size_t FileEntry::getCompressedSize() const {
    return compressedSize_;
}

// Setter 方法
void FileEntry::setRelativePath(const std::string& path) {
    relativePath_ = path;
}

void FileEntry::setFileSize(size_t size) {
    fileSize_ = size;
}

void FileEntry::setCompressedSize(size_t size) {
    compressedSize_ = size;
}

void FileEntry::setDirectory(bool isDirectory) {
    isDirectory_ = isDirectory;
}