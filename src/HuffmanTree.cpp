//
// Created by Musubi on 2026/1/18.
//

#include "../include/HuffmanTree.hpp"
#include <queue>
#include <stdexcept>

HuffmanTree::HuffmanTree()
    : root_(nullptr) {
}

void HuffmanTree::buildTree(const std::unordered_map<char, size_t>& frequencyMap) {
    if (frequencyMap.empty()) {
        throw std::invalid_argument("Frequency map cannot be empty");
    }

    // 创建优先队列
    auto cmp = [](HuffmanNode *a, HuffmanNode *b) { return *a < *b; };
    std::priority_queue<HuffmanNode *, std::vector<HuffmanNode*>, decltype(cmp)> pq(cmp);

    for (const auto& pair : frequencyMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();

        HuffmanNode* right = pq.top();
        pq.pop();

        size_t combineFreq = left->getFrequency() + right->getFrequency();
        HuffmanNode* parent = new HuffmanNode(combineFreq);
        parent->setLeft(left);
        parent->setRight(right);

        pq.push(parent);
    }

    root_.reset(pq.top());
}

void HuffmanTree::generateCodes() {
    if (!root_) {
        throw std::runtime_error("Tree not build");
    }

    encodingTable_.clear();
    generateCodesHelper(root_.get(), "");
}

void HuffmanTree::generateCodesHelper(HuffmanNode *node, std::string code) {
    if (!node) {
        return;
    }

    if (node->isLeaf()) {
        encodingTable_[node->getCharacter()] = code;
        return;
    }

    generateCodesHelper(node->getLeft(), code + "0");
    generateCodesHelper(node->getRight(), code + "1");
}

std::vector<uint8_t> HuffmanTree::serialize() const {
    if (!root_) {
        throw std::runtime_error("Tree not build");
    }

    std::vector<uint8_t> data;
    serializeHelper(root_.get(), data);
    return data;
}

void HuffmanTree::serializeHelper(HuffmanNode *node, std::vector<uint8_t> &data) const{
    if (!node) {
        return;
    }
    if (node->isLeaf()) {
        data.push_back(1);
        data.push_back(static_cast<uint8_t>(node->getCharacter()));
    } else {
        data.push_back(0);

        serializeHelper(node->getLeft(), data);
        serializeHelper(node->getRight(), data);
    }
}

void HuffmanTree::deserialize(const std::vector<uint8_t> &data, size_t& offset) {
    if (offset >= data.size()) {
        throw std::runtime_error("Insufficient data for deserialization");
    }

    root_.reset(deserializeHelper(data, offset));
}

HuffmanNode* HuffmanTree::deserializeHelper(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size()) {
        throw std::runtime_error("Insufficient data for deserialization");
    }

    uint8_t nodeType = data[offset++];

    if (nodeType == 1) {
        if (offset >= data.size()) {
            throw std::runtime_error("Insufficient data for character");
        }

        char character = static_cast<char>(data[offset++]);
        return new HuffmanNode(character, 0);
    } else {
        HuffmanNode* node = new HuffmanNode(0);
        node->setLeft(deserializeHelper(data, offset));
        node->setRight(deserializeHelper(data, offset));
        return node;
    }
}

const std::unordered_map<char, std::string>& HuffmanTree::getEncodingTable() const {
    return encodingTable_;
}

std::string HuffmanTree::encode(char character) const {
    auto it = encodingTable_.find(character);
    if (it == encodingTable_.end()) {
        throw std::runtime_error("Character not found in encoding table");
    }
    return it->second;
}

void HuffmanTree::clear() {
    root_.reset();
    encodingTable_.clear();
}

HuffmanNode* HuffmanTree::getRoot() const {
    return root_.get();
}
