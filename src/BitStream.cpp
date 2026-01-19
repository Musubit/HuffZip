//
// Created by Musubi on 2026/1/18.
//

#include "../include/BitStream.hpp"
#include <stdexcept>

// 构造函数
BitStream::BitStream(const std::string& filePath, Mode mode)
    : mode_(mode)
    , buffer_(0)
    , bitPosition_(0)
    , isOpen_(false) {

    std::ios::openmode openMode = std::ios::binary;
    if (mode == Mode::READ) {
        openMode |= std::ios::in;
    } else {
        openMode |= std::ios::out;
    }

    fileStream_.open(filePath, openMode);
    if (!fileStream_.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    isOpen_ = true;

    // 如果是读模式，读取第一个字节到缓冲区
    if (mode == Mode::READ) {
        readBuffer();
    }
}

// 析构函数
BitStream::~BitStream() {
    if (isOpen_) {
        if (mode_ == Mode::WRITE) {
            flush();
        }
        close();
    }
}

// 写入单个位
void BitStream::writeBit(bool bit) {
    if (mode_ != Mode::WRITE) {
        throw std::runtime_error("BitStream not in write mode");
    }

    // 将位写入缓冲区（从高位到低位）
    if (bit) {
        buffer_ |= (1 << (7 - bitPosition_));
    }

    bitPosition_++;

    // 缓冲区满时写入文件
    if (bitPosition_ == 8) {
        writeBuffer();
    }
}

// 读取单个位
bool BitStream::readBit() {
    if (mode_ != Mode::READ) {
        throw std::runtime_error("BitStream not in read mode");
    }

    if (isEOF()) {
        throw std::runtime_error("Unexpected end of file");
    }

    // 从缓冲区读取位（从高位到低位）
    bool bit = (buffer_ & (1 << (7 - bitPosition_))) != 0;
    bitPosition_++;

    // 缓冲区空时读取下一个字节
    if (bitPosition_ == 8) {
        readBuffer();
    }

    return bit;
}

// 写入字节
void BitStream::writeByte(uint8_t byte) {
    if (mode_ != Mode::WRITE) {
        throw std::runtime_error("BitStream not in write mode");
    }

    // 写入完整的字节
    for (int i = 7; i >= 0; --i) {
        writeBit((byte & (1 << i)) != 0);
    }
}

// 读取字节
uint8_t BitStream::readByte() {
    if (mode_ != Mode::READ) {
        throw std::runtime_error("BitStream not in read mode");
    }

    uint8_t byte = 0;
    for (int i = 7; i >= 0; --i) {
        if (readBit()) {
            byte |= (1 << i);
        }
    }
    return byte;
}

// 刷新缓冲区
void BitStream::flush() {
    if (mode_ != Mode::WRITE || bitPosition_ == 0) {
        return;
    }

    writeBuffer();
}

// 获取未写入的位数
size_t BitStream::getPendingBits() const {
    return mode_ == Mode::WRITE ? bitPosition_ : 0;
}

// 关闭文件
void BitStream::close() {
    if (isOpen_) {
        if (mode_ == Mode::WRITE) {
            flush();
        }
        fileStream_.close();
        isOpen_ = false;
    }
}

// 检查是否到达文件末尾
bool BitStream::isEOF() const {
    return mode_ == Mode::READ && fileStream_.eof() && bitPosition_ >= 8;
}

// 辅助方法：写入缓冲区
void BitStream::writeBuffer() {
    fileStream_.write(reinterpret_cast<const char*>(&buffer_), 1);
    buffer_ = 0;
    bitPosition_ = 0;
}

// 辅助方法：读取缓冲区
void BitStream::readBuffer() {
    if (fileStream_.eof()) {
        return;
    }

    fileStream_.read(reinterpret_cast<char*>(&buffer_), 1);
    bitPosition_ = 0;
}