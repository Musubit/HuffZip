//
// Created by Musubi on 2026/1/18.
//

#include "../include/HuffmanNode.hpp"

HuffmanNode::HuffmanNode(char character, size_t frequency)
    : character_(character)
    , frequency_(frequency)
    , left_(nullptr)
    , right_(nullptr) {
}

HuffmanNode::HuffmanNode(size_t frequency)
    : character_('\0')
    , frequency_(frequency)
    , left_(nullptr)
    , right_(nullptr) {
}

bool HuffmanNode::isLeaf() const {
    return left_ == nullptr && right_ == nullptr;
}

// 频率小的优先，用于优先队列
bool HuffmanNode::operator<(const HuffmanNode& other) const {
    return frequency_ > other.frequency_;
}

char HuffmanNode::getCharacter() const {
    return character_;
}

size_t HuffmanNode::getFrequency() const {
    return frequency_;
}

HuffmanNode* HuffmanNode::getLeft() const {
    return left_.get();
}

HuffmanNode* HuffmanNode::getRight() const {
    return right_.get();
}

void HuffmanNode::setLeft(HuffmanNode* left) {
    left_.reset(left);
}

void HuffmanNode::setRight(HuffmanNode* right) {
    right_.reset(right);
}