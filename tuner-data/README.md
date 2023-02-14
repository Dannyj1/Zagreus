## Zagreus Tuner Dataset
This directory contains the Zagreus Tuner Dataset. The dataset is a file in the [Extended Position Description (EDP)](https://www.chessprogramming.org/Extended_Position_Description) format that contains all positions that were used to tune the evaluation parameters of Zagreus. The dataset mainly consists of FEN strings from games played by top engines (mainly SF, Dragon, Lc0 and Fat Fritz) on [CCRL](https://ccrl.chessdom.com).
The actual end result of a game from white's perspective is represented with a "c9" comment. Example:
```  
rq3rk1/ppnb2bp/3p1np1/2pPp1B1/2P1P3/2N3P1/PP1Q2BP/R3NRK1 w - - c9 "1-0";
```

## License
Zagreus Tuner Dataset © 2023 by Danny Jelsma is licensed under CC BY-SA 4.0. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ or see the [LICENSE](https://github.com/Dannyj1/Zagreus/blob/master/tuner-data/LICENSE) file. This license applies all EDP (files ending in .edp) files in this directory. It does not apply to the source code of Zagreus, which is licensed under the [GNU Affero General Public License v3.0](https://github.com/Dannyj1/Zagreus/blob/master/LICENSE).