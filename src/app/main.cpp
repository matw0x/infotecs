#include <iostream>

#include "manager.h"

LogLevel convertToLogLevel(const std::string& logLevelString) {
    // так как работаем с перечислениями, а в аргументах строка, то переводим её
    // неизвестный уровень важности - выбрасываем исключение
    if (logLevelString == "INFO")
        return INFO;
    else if (logLevelString == "WARNING")
        return WARNING;
    else if (logLevelString == "ERROR")
        return ERROR;
    else
        throw std::invalid_argument("Invalid log level: " + logLevelString);
}

std::unique_ptr<MultithreadAppManager> app = nullptr;

int main(int argc, char* argv[]) {
    // работаем с параметрами командной строки
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file> <log_level>\n";
        return 1;
    }

    // в обертке инициализируем наше многопоточное приложение
    // передаем аргументы командной строки и запускаем
    try {
        app = std::make_unique<MultithreadAppManager>(argv[1], convertToLogLevel(argv[2]));
        app->run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}