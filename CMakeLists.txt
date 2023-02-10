# Zagreus is a chess engine that supports the UCI protocol
# Copyright (C) 2023  Danny Jelsma
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.12)
project(Zagreus)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -Wall -Wextra -Wuninitialized -g -gcodeview -Wl,--pdb=Zagreus.pdb")
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -fuse-ld=lld -pthread -flto=full -fprofile-generate -fprofile-instr-use=tuner.profdata")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -std=c++20 -pthread -flto=full")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -march=native -Wall -Wextra -Wfatal-errors -ftrivial-auto-var-init=uninitialized -Wno-error=unused-command-line-argument -Wuninitialized -std=c++20 -g -gcodeview -Wl,--pdb=Zagreus.pdb")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -Wall -Wextra -Wuninitialized -Wfatal-errors -std=c++20 -pthread -flto -fprofile-generate -fprofile-instr-use=tuner.profdata")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=x86-64 -mlzcnt -Wall -Wextra -Wfatal-errors -Wuninitialized -fuse-ld=lld -std=c++20 -pthread -flto")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -Wall -Wextra -Wuninitialized -Wfatal-errors -std=c++20 -pthread -flto")

file(GLOB inc_zagreus "src/*.h" "src/*.cpp")
file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")

add_executable(Zagreus src/main.cpp ${inc_senjo} ${inc_zagreus})
