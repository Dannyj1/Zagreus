# Zagreus Engine
Zagreus is a work in progress UCI Chess Engine. You can play against Zagreus on Lichess: https://lichess.org/@/Zagreus_Engine. You will also be able to find it's elo rating on there. At the time of writing this is around ~1800 rapid. 
Zagreus is a constantly changing engine with hopefully an increase in elo every update.

You can download binaries here: https://github.com/Dannyj1/ZagreusEngine/releases

# Features
- Bitboard board representation with Plain Magic Bitboards for sliding piece move generation
- Principal Variation Search with Alpha-Beta pruning
- Quiescence Search with delta pruning and SEE move ordering
- Move ordering using MVV/LVA, killer moves, history heuristic, countermove heuristic
- Transposition Table
- Evaluation consisting of material balance, mobility, connectivity, king safety, piece square tables and more
- Null Move Pruning
- Late Move Reduction
- And more! This list is constantly growing and changing, but it is difficult to keep track of all features and changes.

# Credits
Thanks to:
- zd3nik for the [Senjo UCI Adapter](https://github.com/zd3nik/SenjoUCIAdapter) that I use a slightly modified version of
- The [Chessprogramming wiki](https://www.chessprogramming.org/Main_Page) contributors for all the knowledge


# License
This program is licensed under the GNU General Public License v3.0. For full details, please read the [LICENSE](https://github.com/Dannyj1/ZagreusEngine/blob/master/LICENSE) file.
