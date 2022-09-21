cmake_minimum_required(VERSION 3.12)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -O3 -march=native -Wall")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -march=native -Wall")

file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")
file(GLOB inc_hopscotch "tsl/*.h" "tsl/*.cpp")
find_package(Boost REQUIRED)
add_executable(Chess main.cpp ${inc_senjo} ${inc_hopscotch} bitboard.cpp bitboard.h move_gen.cpp move_gen.h engine.cpp engine.h search_mgr.cpp search_mgr.h time_mgr.cpp time_mgr.h tt.cpp tt.h)
