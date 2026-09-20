/*
*
* FrequencyCounter module is to read input file and count frequency for each character.
*
*/
#include <cstdint>
#include <string>
#include <unordered_map>
#ifndef FREQUENCY_COUNTER_H_
#define FREQUENCY_COUNTER_H_



class FrequencyCounter {
    std::unordered_map<char, std::uint64_t> frequencyMap;

public:
    const std::unordered_map<char, std::uint64_t>& getFrequencyMap() const;
    bool readFile(const std::string& fileName);


};


#endif  /*FREQUENCY_COUNTER_H_*/
