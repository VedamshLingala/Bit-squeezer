#include "Huffman.h"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <queue>

namespace {
constexpr char MAGIC[] = {'B', 'S', 'Q', '1'};

struct NodeComparator {
    bool operator()(const Node* left, const Node* right) const {
        if (left->getFrequency() != right->getFrequency())
            return left->getFrequency() > right->getFrequency();
        return left->getMinimumSymbol() > right->getMinimumSymbol();
    }
};

bool writeUint64(std::ofstream& output, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8)
        output.put(static_cast<char>((value >> shift) & 0xFF));
    return static_cast<bool>(output);
}

bool readUint64(std::ifstream& input, std::uint64_t& value) {
    value = 0;
    for (int shift = 56; shift >= 0; shift -= 8) {
        const int byte = input.get();
        if (byte == EOF) return false;
        value |= static_cast<std::uint64_t>(static_cast<unsigned char>(byte)) << shift;
    }
    return true;
}
}

Node* Huffman::buildTree(const std::array<std::uint64_t, 256>& counts) {
    std::priority_queue<Node*, std::vector<Node*>, NodeComparator> queue;
    for (int symbol = 0; symbol < 256; ++symbol)
        if (counts[symbol] > 0) queue.push(new Node(symbol, counts[symbol]));
    if (queue.empty()) return nullptr;
    while (queue.size() > 1) {
        Node* left = queue.top(); queue.pop();
        Node* right = queue.top(); queue.pop();
        queue.push(new Node(left, right));
    }
    return queue.top();
}

void Huffman::destroyTree(Node* root) {
    if (!root) return;
    destroyTree(root->getLeft());
    destroyTree(root->getRight());
    delete root;
}

void Huffman::encodeCharacters(const Node* root, const std::string& code,
                               std::array<std::string, 256>& codes) {
    if (!root) return;
    if (root->isLeaf()) {
        codes[root->getSymbol()] = code.empty() ? "0" : code;
        return;
    }
    encodeCharacters(root->getLeft(), code + '0', codes);
    encodeCharacters(root->getRight(), code + '1', codes);
}

bool Huffman::huffer(const std::unordered_map<char, std::uint64_t>& frequencyMap) {
    frequencies.fill(0);
    codeMap.fill("");
    originalSize = 0;
    for (const auto& item : frequencyMap) {
        const auto symbol = static_cast<unsigned char>(item.first);
        frequencies[symbol] = item.second;
        originalSize += item.second;
    }
    Node* root = buildTree(frequencies);
    encodeCharacters(root, "", codeMap);
    destroyTree(root);
    return true;
}

bool Huffman::compressToFile(const std::string& inputName, const std::string& outputName) const {
    std::ifstream input(inputName, std::ios::binary);
    std::ofstream output(outputName, std::ios::binary | std::ios::trunc);
    if (!input || !output) return false;

    output.write(MAGIC, sizeof(MAGIC));
    if (!writeUint64(output, originalSize)) return false;
    const auto symbolCount = static_cast<std::uint16_t>(std::count_if(
        frequencies.begin(), frequencies.end(), [](std::uint64_t count) { return count > 0; }));
    output.put(static_cast<char>(symbolCount >> 8));
    output.put(static_cast<char>(symbolCount & 0xFF));
    for (int symbol = 0; symbol < 256; ++symbol) {
        if (frequencies[symbol] == 0) continue;
        output.put(static_cast<char>(symbol));
        if (!writeUint64(output, frequencies[symbol])) return false;
    }

    unsigned char buffer = 0;
    int bitCount = 0;
    char character;
    while (input.get(character)) {
        const std::string& code = codeMap[static_cast<unsigned char>(character)];
        if (code.empty()) return false;
        for (char bit : code) {
            buffer = static_cast<unsigned char>((buffer << 1) | (bit == '1'));
            if (++bitCount == 8) {
                output.put(static_cast<char>(buffer));
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount != 0) output.put(static_cast<char>(buffer << (8 - bitCount)));
    return static_cast<bool>(output);
}

bool Huffman::deHuffer(const std::string& compressedName, const std::string& decompressedName) const {
    std::ifstream input(compressedName, std::ios::binary);
    std::ofstream output(decompressedName, std::ios::binary | std::ios::trunc);
    if (!input || !output) return false;

    char magic[sizeof(MAGIC)];
    input.read(magic, sizeof(magic));
    if (!input || !std::equal(std::begin(MAGIC), std::end(MAGIC), magic)) return false;
    std::uint64_t expectedSize = 0;
    if (!readUint64(input, expectedSize)) return false;
    const int high = input.get(), low = input.get();
    if (high == EOF || low == EOF) return false;
    const std::uint16_t symbolCount = static_cast<std::uint16_t>((high << 8) | low);
    std::array<std::uint64_t, 256> decodedCounts{};
    for (std::uint16_t i = 0; i < symbolCount; ++i) {
        const int symbol = input.get();
        std::uint64_t count = 0;
        if (symbol == EOF || !readUint64(input, count) || count == 0) return false;
        decodedCounts[static_cast<unsigned char>(symbol)] = count;
    }
    if (std::accumulate(decodedCounts.begin(), decodedCounts.end(), std::uint64_t{0}) != expectedSize)
        return false;
    if (expectedSize == 0) return true;

    Node* root = buildTree(decodedCounts);
    if (!root) return false;
    if (root->isLeaf()) {
        for (std::uint64_t i = 0; i < expectedSize; ++i) output.put(static_cast<char>(root->getSymbol()));
        destroyTree(root);
        return static_cast<bool>(output);
    }

    Node* current = root;
    std::uint64_t written = 0;
    int byte;
    while (written < expectedSize && (byte = input.get()) != EOF) {
        const unsigned char bits = static_cast<unsigned char>(byte);
        for (int bit = 7; bit >= 0 && written < expectedSize; --bit) {
            current = ((bits >> bit) & 1) ? current->getRight() : current->getLeft();
            if (!current) { destroyTree(root); return false; }
            if (current->isLeaf()) {
                output.put(static_cast<char>(current->getSymbol()));
                ++written;
                current = root;
            }
        }
    }
    const bool success = written == expectedSize && static_cast<bool>(output);
    destroyTree(root);
    return success;
}
