cmake_minimum_required(VERSION 3.10)
project(BitSqueezer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(BitSqueezer
    main.cpp
    FrequencyCounter.cpp
    Huffman.cpp
    Node.cpp)
