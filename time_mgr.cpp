//
// Created by Danny on 28-4-2022.
//

#include <chrono>
#include "time_mgr.h"

namespace Chess {
    std::chrono::time_point<std::chrono::system_clock> TimeManager::getEndTime(Board* board) {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

        now += std::chrono::seconds(10);

        return now;
    }
}
