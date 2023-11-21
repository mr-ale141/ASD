cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(b-tree)

add_executable(
        bTree
        main.cpp
        libs/headers/bTree.hpp
)

target_compile_features(bTree PUBLIC cxx_std_17)