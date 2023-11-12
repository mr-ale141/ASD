cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(b-tree)

add_executable(
    00 
    main.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/headers/sha512.hh
    # ${CMAKE_CURRENT_SOURCE_DIR}/libs/headers/b_tree.hpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/libs/b_tree.cpp
)

target_compile_features(00 PUBLIC cxx_std_17)