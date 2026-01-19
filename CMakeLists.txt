cmake_minimum_required(VERSION 4.1)
project(HuffZip)

set(CMAKE_CXX_STANDARD 17)

add_executable(HuffZip src/main.cpp
        src/BitStream.cpp
        include/BitStream.hpp
        src/main.cpp
        include/HuffmanNode.hpp
        include/HuffmanTree.hpp
        include/FileEntry.hpp
        include/HuffmanCompressor.hpp
        src/HuffmanNode.cpp
        src/HuffmanTree.cpp
        src/FileEntry.cpp
        src/HuffmanCompressor.cpp
        src/HuffmanException.cpp
        include/HuffmanException.hpp
)

# 压缩测试
add_executable(test_compression test/test_compression.cpp
        src/HuffmanTree.cpp
        src/HuffmanNode.cpp
        src/BitStream.cpp
        include/HuffmanTree.hpp
        include/HuffmanNode.hpp
        include/BitStream.hpp
)

# 解压测试
add_executable(test_decompression test/test_decompression.cpp
        src/HuffmanTree.cpp
        src/HuffmanNode.cpp
        src/BitStream.cpp
        include/HuffmanTree.hpp
        include/HuffmanNode.hpp
        include/BitStream.hpp
)
