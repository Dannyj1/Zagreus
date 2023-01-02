//
// Created by Danny on 28-4-2022.
//

#pragma once

#include "bitboard.h"

namespace Zagreus {
    class TimeManager {
    public:
        std::chrono::time_point<std::chrono::high_resolution_clock> getEndTime(Bitboard &board, PieceColor movingColor);
    };

    static TimeManager timeManager{};
}
