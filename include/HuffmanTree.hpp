//
// Created by Musubi on 2026/1/18.
//

#ifndef HUFFZIP_HUFFMANTREE_HPP
#define HUFFZIP_HUFFMANTREE_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>
#include "HuffmanNode.hpp"

/*
 * HuffmanTree功能
 * 1. 根据字符频率构建哈夫曼树
 * 2. 生成字符的二进制编码表
 * 3. 序列化/反序列化树结构（用于存储和传输）
 * 4. 提供字符编码查询接口
 */
class HuffmanTree {
public:
    HuffmanTree();
    ~HuffmanTree() = default;

    void buildTree(const std::unordered_map<char, size_t>& frequencyMap);

    void generateCodes();

    std::vector<uint8_t> serialize() const;

    void deserialize(const std::vector<uint8_t> &data, size_t &offset);

    const std::unordered_map<char, std::string>& getEncodingTable() const;

    std::string encode(char character) const;

    void clear();

    HuffmanNode* getRoot() const;

private:
    std::unique_ptr<HuffmanNode> root_;
    std::unordered_map<char, std::string> encodingTable_;

    // 辅助方法，借助递归 (生成编码/序列化/反序列化)
    void generateCodesHelper(HuffmanNode* node, std::string code);
    void serializeHelper(HuffmanNode* node, std::vector<uint8_t>& data) const;
    HuffmanNode* deserializeHelper(const std::vector<uint8_t>& data, size_t& offset);
};

#endif //HUFFZIP_HUFFMANTREE_HPP