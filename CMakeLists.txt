cmake_minimum_required(VERSION 3.12)
project(Zagreus)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wall -Wextra -Wuninitialized -g -gcodeview -Wl,--pdb=Zagreus.pdb")
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -fuse-ld=lld -pthread -flto=full -fprofile-generate -fprofile-instr-use=tuner.profdata")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -pthread -flto=full")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -march=native -Wall -Wextra -Wfatal-errors -ftrivial-auto-var-init=uninitialized -Wno-error=unused-command-line-argument -Wuninitialized -std=c++20 -g -gcodeview -Wl,--pdb=Zagreus.pdb")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -Wall -Wextra -Wuninitialized -Wfatal-errors -std=c++20 -pthread -flto -fprofile-generate -fprofile-instr-use=tuner.profdata")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -Wall -Wextra -Wuninitialized -Wfatal-errors -std=c++20 -pthread -flto")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -Wall -Wextra -Wuninitialized -Wfatal-errors -std=c++20 -pthread -flto -g -gcodeview -Wl,--pdb=Zagreus.pdb")

file(GLOB inc_zagreus "src/*.h" "src/*.cpp")
file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")

add_executable(Zagreus src/main.cpp ${inc_senjo} ${inc_zagreus})
