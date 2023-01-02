cmake_minimum_required(VERSION 3.12)
project(Zagreus)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wno-error=unused-command-line-argument -g -Wl,--pdb=Zagreus.pdb")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wno-error=unused-command-line-argument")

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Og -march=znver2 -Wall -g -gcodeview -fuse-ld=lld -g -Wl,--pdb=Zagreus.pdb -Wno-error=unused-command-line-argument")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-error=unused-command-line-argument -Ofast -march=znver2 -Wall -g -gcodeview")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=znver2 -Wall")

file(GLOB inc_zagreus "*.h" "*.cpp")
file(GLOB inc_zagreus_collections "collections/*.h" "collections/*.cpp")
file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")
file(GLOB inc_hopscotch "tsl/*.h" "tsl/*.cpp")
find_package(Boost REQUIRED)
add_executable(Zagreus main.cpp ${inc_senjo} ${inc_hopscotch} ${inc_zagreus} ${inc_zagreus_collections})
