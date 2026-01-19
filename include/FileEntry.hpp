//
// Created by Musubi on 2026/1/18.
//

#ifndef FILE_ENTRY_HPP
#define FILE_ENTRY_HPP

#include <string>
#include <vector>
#include <cstdint>

class FileEntry {
public:
    // 构造函数
    FileEntry();
    FileEntry(const std::string& relativePath, size_t fileSize, bool isDirectory);

    // 序列化
    std::vector<uint8_t> serialize() const;

    // 反序列化
    void deserialize(const std::vector<uint8_t>& data, size_t& offset);

    // Getter 方法
    const std::string& getRelativePath() const;
    size_t getFileSize() const;
    bool isDirectory() const;
    size_t getCompressedSize() const;

    // Setter 方法
    void setRelativePath(const std::string& path);
    void setFileSize(size_t size);
    void setCompressedSize(size_t size);
    void setDirectory(bool isDirectory);

private:
    std::string relativePath_;   // 相对路径
    size_t fileSize_;            // 原始文件大小
    size_t compressedSize_;      // 压缩后大小
    bool isDirectory_;           // 是否为目录
};

#endif // FILE_ENTRY_HPP