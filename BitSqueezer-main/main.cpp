#include <chrono>
#include <fstream>
#include <iostream>

#include "FrequencyCounter.h"
#include "Huffman.h"

namespace {
std::uint64_t fileSize(const std::string& filename) {
    std::ifstream input(filename, std::ios::binary | std::ios::ate);
    return input ? static_cast<std::uint64_t>(input.tellg()) : 0;
}

void printUsage(const char* program) {
    std::cerr << "Usage:\n  " << program << " compress <input> <output.bsq>\n  "
              << program << " decompress <input.bsq> <output>\n";
}
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string mode = argv[1];
    const std::string input = argv[2];
    const std::string output = argv[3];
    const auto start = std::chrono::steady_clock::now();
    bool success = false;

    if (mode == "compress") {
        FrequencyCounter counter;
        if (!counter.readFile(input)) {
            std::cerr << "Unable to read input file: " << input << '\n';
            return 1;
        }
        Huffman huffman;
        huffman.huffer(counter.getFrequencyMap());
        success = huffman.compressToFile(input, output);
    } else if (mode == "decompress") {
        Huffman huffman;
        success = huffman.deHuffer(input, output);
    } else {
        printUsage(argv[0]);
        return 1;
    }

    if (!success) {
        std::cerr << "Operation failed. The input may be unreadable or malformed.\n";
        return 1;
    }

    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    std::cout << "Completed in " << elapsed << " seconds\n"
              << "Input size: " << fileSize(input) << " bytes\n"
              << "Output size: " << fileSize(output) << " bytes\n";
    if (mode == "compress" && fileSize(input) != 0)
        std::cout << "Compressed/original ratio: "
                  << static_cast<double>(fileSize(output)) / fileSize(input) << '\n';
    return 0;
}
