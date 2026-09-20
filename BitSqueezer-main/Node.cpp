#include "Node.h"

Node::Node(int symbol, std::uint64_t frequency)
    : symbol(symbol), frequency(frequency), minimumSymbol(symbol), left(nullptr), right(nullptr) {}

Node::Node(Node* left, Node* right)
    : symbol(-1),
      frequency(left->getFrequency() + right->getFrequency()),
      minimumSymbol(left->getMinimumSymbol() < right->getMinimumSymbol()
                        ? left->getMinimumSymbol()
                        : right->getMinimumSymbol()),
      left(left),
      right(right) {}

std::uint64_t Node::getFrequency() const { return frequency; }
int Node::getMinimumSymbol() const { return minimumSymbol; }
int Node::getSymbol() const { return symbol; }
Node* Node::getLeft() const { return left; }
Node* Node::getRight() const { return right; }
bool Node::isLeaf() const { return left == nullptr && right == nullptr; }
