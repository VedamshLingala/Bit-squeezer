//
// Created by zawawy on 12/22/18.
//
#include <array>
#include <cstdint>
#include <unordered_map>
#include <string>
#include "Node.h"

#ifndef HUFFMANCODING_HUFFER_H
#define HUFFMANCODING_HUFFER_H


class Huffman {
public :
    bool huffer(const std::unordered_map<char, std::uint64_t>& frequencyMap);
    bool deHuffer(const std::string& compressedFileName, const std::string& decompressedFileName) const;
    bool compressToFile(const std::string& inputFileName, const std::string& outputFileName) const;

private:
    std::array<std::uint64_t, 256> frequencies{};
    std::array<std::string, 256> codeMap{};
    std::uint64_t originalSize = 0;

    static Node* buildTree(const std::array<std::uint64_t, 256>& frequencies);
    static void destroyTree(Node* root);
    static void encodeCharacters(const Node* root, const std::string& code,
                                 std::array<std::string, 256>& codes);
};



#endif //HUFFMANCODING_HUFFER_H
