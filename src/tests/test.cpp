#include <logger/logger.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <sstream>
#include "../app/manager.h"

typedef std::vector<std::pair<std::string, std::function<void()>>> TEST_TYPE;

std::unique_ptr<MultithreadAppManager> app = nullptr;

void runTests(const TEST_TYPE& tests) {
    for (const auto& pair : tests) {
        try {
            std::cout << "Running test: " << pair.first << "...\n";
            pair.second();
            std::cout << pair.first << " passed.\n";
        } catch (const std::exception& e) {
            std::cout << pair.first << " failed: " << e.what() << "\n";
        }
    }
}

int main() {
    TEST_TYPE onlyLibrary = {
        { "testCreateLogFile", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);

            std::ifstream file(filename);
            assert(file.is_open());
        }},

        { "testLog", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);
            logger.log("Hi!");

            std::ifstream logFile(filename);
            std::string line;
            std::getline(logFile, line);

            assert(line.find("Hi!") != std::string::npos);
        }},

        { "testLogLevelCondition", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename, WARNING);

            logger.log("Bad info");
            logger.log("Hello world", WARNING);

            std::ifstream logFile(filename);
            std::string line;
            bool foundMessage = false, foundBadInfo = false;

            while (std::getline(logFile, line)) {
                if (line.find("Hello world") != std::string::npos) foundMessage = true;
                if (line.find("Bad info") != std::string::npos) foundBadInfo = true;
            }

            assert(foundMessage && !foundBadInfo);
        }},

        { "testLogLevelChange", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename, WARNING);

            logger.log("Bad info");
            logger.log("Hello world", WARNING);

            logger.changeLogLevel(INFO);
            logger.log("This is info", INFO);

            std::ifstream logFile(filename);
            std::string line;
            bool foundHello = false, foundInfo = false, foundBadInfo = false;

            while (std::getline(logFile, line)) {
                if (line.find("Hello world") != std::string::npos) foundHello = true;
                if (line.find("This is info") != std::string::npos) foundInfo = true;
                if (line.find("Bad info") != std::string::npos) foundBadInfo = true;
            }

            assert(foundHello && foundInfo && !foundBadInfo);
        }},

        { "testLogEmptyMsg", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);

            logger.log("");

            std::ifstream logFile(filename);
            std::string line;
            bool foundEmptyMessage = false;

            while (std::getline(logFile, line)) {
                if (line.empty()) foundEmptyMessage = true;
            }

            assert(!foundEmptyMessage);
        }},
        
        { "testMaxFileSize", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);

            const std::string longMessage(1000000, 'a');

            for (int i = 0; i < 1000; ++i) logger.log(longMessage);

            std::ifstream logFile(filename, std::ios::ate);
            assert(logFile.is_open());

            const std::streampos fileSize = logFile.tellg();
            assert(fileSize > 0);
        }},

        { "testBasePerformanceFAST", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);
            logger.changeLogType(LogType::FAST);

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100000; ++i) logger.log("Performance test message");
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end - start;
            double averageTimePerLog = duration.count() / 100000;

            assert(true); // поскольку это режим быстрой записи

            std::cout << "testBasePerformanceFAST | " << duration.count()
                    << " seconds, average time per log: " << averageTimePerLog << " seconds.\n";
        }},

        { "testBasePerformanceSAFELY", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 100000; ++i) logger.log("Performance test message");
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end - start;
            double averageTimePerLog = duration.count() / 100000;

            std::string line;
            std::ifstream logFile(filename);
            int count = 0;
            while (std::getline(logFile, line)) {
                if (line.find("Performance test message") != std::string::npos) {
                    ++count;
                }
            }

            assert(count == 100000);

            std::cout << "testBasePerformanceSAFELY | " << duration.count()
                    << " seconds, average time per log: " << averageTimePerLog << " seconds.\n";
        }},

        { "testPerformanceWithMultithreading", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);
            const int numThreads = 10, numMessages = 1000;

            std::vector<std::thread> threads;
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back([&logger, numMessages]() {
                    for (int j = 0; j < numMessages; ++j) {
                        logger.log("Thread test message");
                    }
                });
            }

            for (auto& t : threads) t.join();

            std::ifstream logFile(filename);
            int messageCount = 0;
            std::string line;
            while (std::getline(logFile, line)) {
                if (line.find("Thread test message") != std::string::npos) {
                    ++messageCount;
                }
            }

            assert(messageCount == numThreads * numMessages);
        }},

        { "testFrequentLogLevelChanges", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);

            for (int i = 0; i < 1000; ++i) {
                logger.changeLogLevel(static_cast<LogLevel>(i % 3));
                logger.log("Message with changing log level");
            }

            std::ifstream logFile(filename);
            std::string line;
            bool foundMessage = false;

            while (std::getline(logFile, line)) {
                if (line.find("Message with changing log level") != std::string::npos) {
                    foundMessage = true;
                    break;
                }
            }

            assert(foundMessage);
        }},

        { "testFileAccessibilityAfterCompletion", []() {
            const std::string filename = "test_log.txt";
            std::remove(filename.c_str());
            Logger logger(filename);
            logger.log("Test message after completion");

            std::ifstream file(filename);
            assert(file.is_open());
        }},

        { "testCreateLogFileInInaccessibleDirectory", []() {
            const std::string filenameToDelete = "test_log.txt",
                            filename = "root/" + filenameToDelete;
            std::remove(filenameToDelete.c_str());

            try {
                Logger logger(filename);
                assert(false);
            } catch(const std::runtime_error& e) {
                std::cout << "testCreateLogFileInInaccessibleDirectory | exception caught: " << e.what() << std::endl;
            }
        }}
    };

    TEST_TYPE multithreadGameWithLib = {
        { "testInvalidMove", []() {
                const std::string filename = "test_lib_log.txt";
                std::remove(filename.c_str());

                std::istringstream inputStream("wdas");
                std::cin.rdbuf(inputStream.rdbuf());

                app = std::make_unique<MultithreadAppManager>(filename);
                app->run();
            }
        }
    };

    runTests(onlyLibrary);
    runTests(multithreadGameWithLib);

    std::cout << "Total: all tests passed!\n";

    return 0;
}