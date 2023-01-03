cmake_minimum_required(VERSION 3.12)
project(Zagreus)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wno-error=unused-command-line-argument -Wall -Wextra -Wuninitialized -g -Wl,--pdb=Zagreus.pdb")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wno-error=unused-command-line-argument -Wall -Wextra -Wuninitialized -fuse-ld=lld -pthread")

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Og -march=znver2 -Wall -Wextra -g -gcodeview -fno-omit-frame-pointer -ftrivial-auto-var-init=uninitialized -Wno-error=unused-command-line-argument -Wuninitialized -g -Wl,--pdb=Zagreus.pdb")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-error=unused-command-line-argument -fno-omit-frame-pointer -Ofast -march=znver2 -Wall -Wextra -g -gcodeview -Wuninitialized")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=znver2 -Wall -Wextra -Wuninitialized -fuse-ld=lld -pthread")

file(GLOB inc_zagreus "*.h" "*.cpp")
file(GLOB inc_zagreus_collections "collections/*.h" "collections/*.cpp")
file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")
find_package(Boost REQUIRED)
add_executable(Zagreus main.cpp ${inc_senjo} ${inc_zagreus} ${inc_zagreus_collections})
