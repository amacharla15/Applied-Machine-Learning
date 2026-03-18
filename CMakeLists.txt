cmake_minimum_required(VERSION 3.10)
project(parallel_bpe_tokenizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(parallel_bpe_tokenizer
    src/main.cpp
    src/tokenizer_assets.cpp
    src/bpe.cpp
)

target_include_directories(parallel_bpe_tokenizer PRIVATE include)