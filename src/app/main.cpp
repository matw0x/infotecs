#include "manager.h"
#include <iostream>

LogLevel convertToLogLevel(const std::string& logLevelString) {
    if (logLevelString == "INFO") return INFO;
    else if (logLevelString == "WARNING") return WARNING;
    else if (logLevelString == "ERROR") return ERROR;
    else return static_cast<LogLevel>(-1);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file> <log_level>\n";
        return 1;
    }

    try {
        app = MultithreadAppManager(argv[1], convertToLogLevel(argv[2]));
        app.run();
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // pPlayer->readme();
    // short input; std::cin >> input;

    // if (!pPlayer->handleChoice(input)) {
    //     //
    // }

    return 0;
}