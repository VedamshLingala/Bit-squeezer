//
// Created by zawawy on 12/22/18.
//

#ifndef HUFFMANCODING_NODE_H
#define HUFFMANCODING_NODE_H

#include <cstdint>

class Node {
    int symbol;
    std::uint64_t frequency;
    int minimumSymbol;
    Node* left;
    Node* right;

public:
    Node(int symbol, std::uint64_t frequency);
    Node(Node* left, Node* right);

    std::uint64_t getFrequency() const;
    int getMinimumSymbol() const;
    int getSymbol() const;
    Node* getLeft() const;
    Node* getRight() const;
    bool isLeaf() const;
};

#endif
