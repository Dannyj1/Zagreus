cmake_minimum_required(VERSION 3.12)
project(Chess)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
add_executable(Chess main.cpp board.cpp board.h piece.cpp piece.h tile.cpp tile.h engine.cpp engine.h)
target_link_libraries(Chess PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/senjo_dbg.lib Boost::boost)