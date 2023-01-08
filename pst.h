//
// Created by Danny on 8-1-2023.
//

#pragma once

#include "types.h"

namespace Zagreus {
// PST tables from https://www.chessprogramming.org/Simplified_Evaluation_Function
    void initializePst();

    int getPstValue(PieceType piece, int square);
}