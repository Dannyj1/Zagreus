cmake_minimum_required(VERSION 3.12)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -O3")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0")

file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")
find_package(Boost REQUIRED)
add_executable(Chess main.cpp board.cpp board.h piece.cpp piece.h tile.cpp tile.h engine.cpp engine.h ${inc_senjo} search.cpp search.h)
