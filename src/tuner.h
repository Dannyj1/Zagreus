/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2026  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#ifdef ZAGREUS_TUNER
#include <string>
#include <vector>

#include "constants.h"
#include "eval.h"
#include "types.h"

namespace Zagreus {
struct TraceCoefficient {
    int midgameIndex;
    int endgameIndex;
    int16_t whiteCount;
    int16_t blackCount;
};

struct TunePosition {
    std::vector<TraceCoefficient> coefficients;
    double result = 0.0;
    int phase = 0;
};

extern std::vector<double> weights;
extern std::vector<double> baseWeights;
extern int pstWeightStart;
extern int mobilityWeightStart;
extern int doubledPawnWeightStart;
extern double K;

void initializeWeights();
std::vector<TraceCoefficient> createCoefficients(const EvalTrace& trace);
double sigmoid(double x);
double evaluateFromCoefficients(const TunePosition& position);
double calculateError(const std::vector<TunePosition>& positions);
std::vector<double> calculateGradients(const std::vector<TunePosition>& positions);
void exportTunedValues(const std::string& outputPath, int finalEpoch, double trainingError, double validationError,
                       double testError);

void startTuning(std::string filePath);
}  // namespace Zagreus
#endif
