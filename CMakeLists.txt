cmake_minimum_required(VERSION 3.12)
project(Zagreus)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wno-error=unused-command-line-argument -Wall -Wextra -Wuninitialized -g -gcodeview -flto -Wl,--pdb=Zagreus.pdb")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wno-error=unused-command-line-argument -Wall -Wextra -Wuninitialized -fuse-ld=lld -pthread -flto")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -march=native -Wall -Wextra -g -gcodeview -ftrivial-auto-var-init=uninitialized -Wno-error=unused-command-line-argument -Wuninitialized -pthread -std=c++20 -Wl,--pdb=Zagreus.pdb")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=x86-64 -mbmi -mlzcnt -Wall -Wextra -Wuninitialized -fuse-ld=lld -std=c++20 -pthread -flto")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -Wall -Wextra -Wuninitialized -fuse-ld=lld -std=c++20 -pthread -flto")

file(GLOB inc_zagreus "*.h" "*.cpp")
file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")
find_package(Boost REQUIRED)
add_executable(Zagreus main.cpp ${inc_senjo} ${inc_zagreus})
