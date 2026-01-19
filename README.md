# HuffZip

基于哈夫曼编码算法的文件压缩/解压缩程序，支持单个文件和目录的压缩。

## 功能特性

- ✅ **文件压缩**：使用哈夫曼编码压缩单个文件
- ✅ **目录压缩**：递归压缩整个目录
- ✅ **文件解压**：解压压缩文件并还原原始内容
- ✅ **无损压缩**：保证压缩/解压后数据完全一致
- ✅ **位级操作**：使用 BitStream 实现精确的位级读写
- ✅ **序列化支持**：自动序列化哈夫曼树结构

## 构建方法

### 前置要求

- CMake 4.1 或更高版本
- C++17 兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）

### 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake ..

# 编译
cmake --build .

# 运行程序
./HuffZip
```

### 运行测试

```bash
# 编译压缩测试
cmake --build . --target test_compression

# 编译解压测试
cmake --build . --target test_decompression

# 运行测试
./test_compression
./test_decompression
```

## 使用方法

### 命令行参数

```bash
HuffZip <command> <input> <output>
```

### 支持的命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `compress-file` | 压缩单个文件 | `HuffZip compress-file input.txt output.huff` |
| `compress-dir` | 压缩目录 | `HuffZip compress-dir mydir archive.huff` |
| `decompress` | 解压文件 | `HuffZip decompress archive.huff outputdir` |

### 使用示例

#### 1. 压缩单个文件

```bash
HuffZip compress-file input.txt compressed.huff
```

#### 2. 压缩目录

```bash
HuffZip compress-dir my_folder archive.huff
```

#### 3. 解压文件

```bash
HuffZip decompress archive.huff output_folder
```

## 项目结构

```
HuffZip/
├── CMakeLists.txt              # CMake 构建配置
├── include/                    # 头文件
│   ├── BitStream.hpp          # 位流操作类
│   ├── FileEntry.hpp          # 文件条目类
│   ├── HuffmanCompressor.hpp  # 压缩器主类
│   ├── HuffmanException.hpp   # 异常处理类
│   ├── HuffmanNode.hpp        # 哈夫曼树节点类
│   └── HuffmanTree.hpp        # 哈夫曼树类
├── src/                        # 源文件
│   ├── BitStream.cpp
│   ├── FileEntry.cpp
│   ├── HuffmanCompressor.cpp
│   ├── HuffmanException.cpp
│   ├── HuffmanNode.cpp
│   ├── HuffmanTree.cpp
│   └── main.cpp               # 主程序入口
└── test/                       # 测试文件
    ├── test_compression.cpp   # 压缩测试
    ├── test_decompression.cpp # 解压测试
    └── test_files/            # 测试数据
```

## 技术细节

### 哈夫曼编码

哈夫曼编码是一种无损数据压缩算法，通过为频繁出现的字符分配较短的编码，为不频繁的字符分配较长的编码，从而实现压缩效果。

### 核心组件

1. **HuffmanTree**：构建哈夫曼树并生成编码表
   - 使用优先队列（小顶堆）高效构建树
   - 支持树的序列化和反序列化

2. **BitStream**：位级读写操作
   - 支持按位读写，精确控制压缩数据
   - 自动缓冲管理，提高 I/O 效率

3. **HuffmanCompressor**：压缩/解压主逻辑
   - 文件和目录的递归处理
   - 频率统计和编码生成

### 压缩流程

1. 读取输入文件
2. 统计字符频率
3. 构建哈夫曼树
4. 生成编码表
5. 序列化树结构
6. 使用编码表压缩数据
7. 写入压缩文件

### 解压流程

1. 读取压缩文件
2. 反序列化哈夫曼树
3. 重建编码表
4. 逐位解码数据
5. 写入原始文件

## 示例输出

```
Testing compression...
Original file size: 1000 bytes
Unique characters: 15
Sample Huffman codes:
  'A': 0
  'B': 110
  'C': 1110
  'D': 1111
  'E': 10
Tree serialization size: 45 bytes
Compressed file size: 650 bytes
Compression ratio: 35.00%
Compression test passed!
```

## 许可证

本项目采用 MIT 许可证。

## 作者

Musubit

## 贡献

欢迎提交 Issue 和 Pull Request！
