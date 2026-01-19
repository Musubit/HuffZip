//
// Created by Musubi on 2026/1/18.
//

#ifndef HUFFZIP_HUFFMANNODE_HPP
#define HUFFZIP_HUFFMANNODE_HPP

#include <cstddef>
#include <memory>

/*
 *HuffmanNode功能
 * 1. 叶子节点：存储实际的字符及其频率
 * 2. 内部节点：存储合并后的频率，用于构建树结构
 */

class HuffmanNode {
public:
    // 构造函数：1-叶子节点  2-内部节点
    HuffmanNode(char character, size_t frequency);
    HuffmanNode(size_t frequency);

    ~HuffmanNode() = default;

    bool isLeaf() const;

    // 重载比较运算符用于优先队列
    bool operator<(const HuffmanNode& other) const;

    char getCharacter() const;
    size_t getFrequency() const;
    HuffmanNode* getLeft() const;
    HuffmanNode* getRight() const;

    void setLeft(HuffmanNode* left);
    void setRight(HuffmanNode* right);

private:
    // 1-字符 2-频率  3-左子节点  4-右子节点
    char character_;
    size_t frequency_;
    std::unique_ptr<HuffmanNode> left_;
    std::unique_ptr<HuffmanNode> right_;
};

#endif //HUFFZIP_HUFFMANNODE_HPP