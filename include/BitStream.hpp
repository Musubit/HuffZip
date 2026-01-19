//
// Created by Musubi on 2026/1/18.
//

#ifndef BIT_STREAM_HPP
#define BIT_STREAM_HPP

#include <string>
#include <fstream>
#include <cstdint>

class BitStream {
public:
    enum class Mode {
        READ,
        WRITE
    };

    // 构造函数
    BitStream(const std::string& filePath, Mode mode);

    // 析构函数
    ~BitStream();

    // 禁止拷贝
    BitStream(const BitStream&) = delete;
    BitStream& operator=(const BitStream&) = delete;

    // 位操作
    void writeBit(bool bit);
    bool readBit();

    // 字节操作
    void writeByte(uint8_t byte);
    uint8_t readByte();

    // 缓冲区管理
    void flush();
    size_t getPendingBits() const;

    // 文件操作
    void close();
    bool isEOF() const;

private:
    std::fstream fileStream_;
    Mode mode_;
    uint8_t buffer_;
    size_t bitPosition_;
    bool isOpen_;

    // 辅助方法
    void writeBuffer();
    void readBuffer();
};

#endif // BIT_STREAM_HPP