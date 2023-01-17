# Zagreus Chess Engine
Zagreus is a work in progress UCI Chess Engine. You can play against Zagreus on Lichess: https://lichess.org/@/Zagreus_Engine. You will also be able to find it's elo rating on there. At the time of writing this is around ~1750 rapid. 
Zagreus is a constantly changing engine with hopefully an increase in elo every update.

You can download binaries here: https://github.com/Dannyj1/ZagreusEngine/releases

This engine requires a GUI that supports the UCI (Universal Chess Interface) protocol to use it. I personally use [CuteChess](https://cutechess.com/).

# Features
- Bitboard board representation with Plain Magic Bitboards for sliding piece move generation
- Principal Variation Search with Alpha-Beta pruning
- Quiescence Search with delta pruning and SEE move ordering
- Move ordering using MVV/LVA, killer moves, history heuristic, countermove heuristic
- Transposition Table
- Tapered Evaluation consisting of material balance, mobility, very simple connectivity, king safety, piece square tables and more
- Null Move Pruning
- Late Move Reduction
- And more! This list is constantly growing and changing, but it is difficult to keep track of all features and changes.

# UCI Options
Zagreus Engine has the following UCI options that can be changed:

- `Move Overhead` - The amount of time that will be substracted from the internal timer for each move. This helps when using the engine over the internet, to prevent it from losing on time due to lag. The default is 0.
- `Hash` - The size of the transposition table in megabytes, rounded to the nearest power of 2. The default is 512MB.

# Build Instructions
To build Zagreus, you will need to use LLVM. On Windows, I use [LLVM MinGW](https://github.com/mstorsjo/llvm-mingw). On Linux I just use [LLVM](https://releases.llvm.org/download.html).
Zagreus uses CMake to build. On Windows you can use the [CMake-GUI](https://cmake.org/runningcmake/).  On Linux, use the following commands:

Clone the repository:
```bash
git clone https://github.com/Dannyj1/ZagreusEngine.git
```

Set the compiler to clang++:
```bash
export CC=/usr/bin/clang-15
export CXX=/usr/bin/clang++-15
```
Build:
```bash
cd <directory of Zagreus engine>
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

# Credits
Thanks to:
- [zd3nik](https://github.com/zd3nik) for the [Senjo UCI Adapter](https://github.com/zd3nik/SenjoUCIAdapter), which I slightly modified
- [AndyGrant](https://github.com/AndyGrant) for the easy to use and open source [OpenBench Testing Framework](https://github.com/AndyGrant/OpenBench), making testing my engine SO much easier and allowing you to distribute test over multiple devices (I use a private instance of OpenBench)
- [maksimKorzh](https://github.com/maksimKorzh) for the great [tutorials](https://www.youtube.com/channel/UCB9-prLkPwgvlKKqDgXhsMQ) and [engine](https://github.com/maksimKorzh/chess_programming)
- The [Chessprogramming wiki](https://www.chessprogramming.org/Main_Page) contributors for all the information about chess programming

# License
Copyright (C) 2023  Danny Jelsma

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
Please read the [LICENSE](https://github.com/Dannyj1/ZagreusEngine/blob/master/LICENSE) file or https://www.gnu.org/licenses/ for the full details.
