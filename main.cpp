#include <iostream>
#include "board.h"
#include "engine.h"
#include "senjo/UCIAdapter.h"
#include "senjo/Output.h"

int main() {
    try {
        Engine engine;
        senjo::UCIAdapter adapter(engine);

        std::string line;
        line.reserve(16384);

        while (std::getline(std::cin, line)) {
            senjo::Output() << "INFO: " << line;
            if (!adapter.doCommand(line)) {
                break;
            }
        }

        return 0;
    } catch (const std::exception &e) {
        senjo::Output() << "ERROR: " << e.what();
        return 1;
    }
}

void perft(int depth) {
    int nodes = 0;
    int captures = 0;
}