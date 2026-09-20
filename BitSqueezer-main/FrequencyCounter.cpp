#include "FrequencyCounter.h"

#include <fstream>

bool FrequencyCounter::readFile(const std::string& fileName) {
    char character;
    std::ifstream inputFile(fileName, std::ios::binary);
    if (!inputFile)
        return false;

    while(inputFile.get(character))
        frequencyMap[character]++;
    return inputFile.eof();
}

const std::unordered_map<char, std::uint64_t>& FrequencyCounter::getFrequencyMap() const {
    return frequencyMap;
}
