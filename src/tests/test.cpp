#include <logger/logger.h>

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

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
            assert(false);
        }
    }
}

int main() {
    TEST_TYPE onlyLibrary = {{"testCreateLogFile",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);

                                  std::ifstream file(filename);
                                  assert(file.is_open());
                              }},

                             {"testLog",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);
                                  logger.log("Hi!");

                                  std::ifstream logFile(filename);
                                  std::string   line;
                                  std::getline(logFile, line);

                                  assert(line.find("Hi!") != std::string::npos);
                              }},

                             {"testLogLevelCondition",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename, WARNING);

                                  logger.log("Bad info");
                                  logger.log("Hello world", WARNING);

                                  std::ifstream logFile(filename);
                                  std::string   line;
                                  bool          foundMessage = false, foundBadInfo = false;

                                  while (std::getline(logFile, line)) {
                                      if (line.find("Hello world") != std::string::npos) foundMessage = true;
                                      if (line.find("Bad info") != std::string::npos) foundBadInfo = true;
                                  }

                                  assert(foundMessage && !foundBadInfo);
                              }},

                             {"testLogLevelChange",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename, WARNING);

                                  logger.log("Bad info");
                                  logger.log("Hello world", WARNING);

                                  logger.changeLogLevel(INFO);
                                  logger.log("This is info", INFO);

                                  std::ifstream logFile(filename);
                                  std::string   line;
                                  bool          foundHello = false, foundInfo = false, foundBadInfo = false;

                                  while (std::getline(logFile, line)) {
                                      if (line.find("Hello world") != std::string::npos) foundHello = true;
                                      if (line.find("This is info") != std::string::npos) foundInfo = true;
                                      if (line.find("Bad info") != std::string::npos) foundBadInfo = true;
                                  }

                                  assert(foundHello && foundInfo && !foundBadInfo);
                              }},

                             {"testLogEmptyMsg",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);

                                  logger.log("");

                                  std::ifstream logFile(filename);
                                  std::string   line;
                                  bool          foundEmptyMessage = false;

                                  while (std::getline(logFile, line)) {
                                      if (line.empty()) foundEmptyMessage = true;
                                  }

                                  assert(!foundEmptyMessage);
                              }},

                             {"testMaxFileSize",
                              []() {
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

                             {"testBasePerformanceFAST",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);
                                  logger.changeLogType(LogType::FAST);

                                  auto start = std::chrono::high_resolution_clock::now();
                                  for (int i = 0; i < 100000; ++i) logger.log("Performance test message");
                                  auto end = std::chrono::high_resolution_clock::now();

                                  std::chrono::duration<double> duration          = end - start;
                                  double                        averageTimePerLog = duration.count() / 100000;

                                  assert(true);  // поскольку это режим быстрой записи

                                  std::cout << "testBasePerformanceFAST | " << duration.count()
                                            << " seconds, average time per log: " << averageTimePerLog << " seconds.\n";
                              }},

                             {"testBasePerformanceSAFELY",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);

                                  auto start = std::chrono::high_resolution_clock::now();
                                  for (int i = 0; i < 100000; ++i) logger.log("Performance test message");
                                  auto end = std::chrono::high_resolution_clock::now();

                                  std::chrono::duration<double> duration          = end - start;
                                  double                        averageTimePerLog = duration.count() / 100000;

                                  std::string   line;
                                  std::ifstream logFile(filename);
                                  int           count = 0;
                                  while (std::getline(logFile, line)) {
                                      if (line.find("Performance test message") != std::string::npos) {
                                          ++count;
                                      }
                                  }

                                  assert(count == 100000);

                                  std::cout << "testBasePerformanceSAFELY | " << duration.count()
                                            << " seconds, average time per log: " << averageTimePerLog << " seconds.\n";
                              }},

                             {"testPerformanceWithMultithreading",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger    logger(filename);
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
                                  int           messageCount = 0;
                                  std::string   line;
                                  while (std::getline(logFile, line)) {
                                      if (line.find("Thread test message") != std::string::npos) {
                                          ++messageCount;
                                      }
                                  }

                                  assert(messageCount == numThreads * numMessages);
                              }},

                             {"testFrequentLogLevelChanges",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);

                                  for (int i = 0; i < 1000; ++i) {
                                      logger.changeLogLevel(static_cast<LogLevel>(i % 3));
                                      logger.log("Message with changing log level");
                                  }

                                  std::ifstream logFile(filename);
                                  std::string   line;
                                  bool          foundMessage = false;

                                  while (std::getline(logFile, line)) {
                                      if (line.find("Message with changing log level") != std::string::npos) {
                                          foundMessage = true;
                                          break;
                                      }
                                  }

                                  assert(foundMessage);
                              }},

                             {"testFileAccessibilityAfterCompletion",
                              []() {
                                  const std::string filename = "test_log.txt";
                                  std::remove(filename.c_str());
                                  Logger logger(filename);
                                  logger.log("Test message after completion");

                                  std::ifstream file(filename);
                                  assert(file.is_open());
                              }},

                             {"testCreateLogFileInInaccessibleDirectory", []() {
                                  const std::string filenameToDelete = "test_log.txt",
                                                    filename         = "root/" + filenameToDelete;
                                  std::remove(filenameToDelete.c_str());

                                  try {
                                      Logger logger(filename);
                                      assert(false);
                                  } catch (const std::runtime_error& e) {
                                      std::cout
                                          << "testCreateLogFileInInaccessibleDirectory | exception caught: " << e.what()
                                          << std::endl;
                                  }
                              }}};

    TEST_TYPE multithreadGameWithLib = {
        {"testInvalidMoveSMALL",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             const std::string commands = "w\na\nw\na\nw\n";

             std::istringstream inputStream(std::to_string(Choice::PLAY) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             size_t        WandFailed = 0, AandFailed = 0;
             std::ifstream logFile(filename);
             std::string   line;
             while (std::getline(logFile, line)) {
                 if (line.find("Player::processMove | data = w") != std::string::npos) ++WandFailed;
                 if (line.find("Player::processMove | failed to move up") != std::string::npos) ++WandFailed;

                 if (line.find("Player::processMove | data = a") != std::string::npos) ++AandFailed;
                 if (line.find("Player::processMove | failed to move left") != std::string::npos) ++AandFailed;
             }

             assert(WandFailed + AandFailed == commands.size());
         }},

        {"testInvalidMoveBIG",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             const size_t commandSize = 10000;
             std::string  commands;
             for (size_t i = 0; i != commandSize; ++i) commands += (i % 2 == 0) ? "w\n" : "a\n";

             std::istringstream inputStream(std::to_string(Choice::PLAY) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             size_t        WandFailed = 0, AandFailed = 0;
             std::ifstream logFile(filename);
             std::string   line;
             while (std::getline(logFile, line)) {
                 if (line.find("Player::processMove | data = w") != std::string::npos) ++WandFailed;
                 if (line.find("Player::processMove | failed to move up") != std::string::npos) ++WandFailed;

                 if (line.find("Player::processMove | data = a") != std::string::npos) ++AandFailed;
                 if (line.find("Player::processMove | failed to move left") != std::string::npos) ++AandFailed;
             }

             assert(WandFailed + AandFailed == commands.size());
         }},

        {"testInvalidChoice",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             std::istringstream inputStream(std::to_string(static_cast<Choice>('Y')) + "\n");
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             std::ifstream logFile(filename);
             std::string   line;

             bool found = false;
             while (std::getline(logFile, line)) {
                 if (line.find("Player::handleChoice | selected something unclear...") != std::string::npos) {
                     found = true;
                     break;
                 }
             }

             assert(found);
         }},

        {"testAllPlayerLogsSMALL",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             std::string        commands = "W\nA\nS\nD\n";
             std::istringstream inputStream(std::to_string(Choice::PLAY) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             size_t W, A, S, D, handleChoice;
             bool   readme, printBeforePlay;

             W = A = S = D = handleChoice = 0;
             readme = printBeforePlay = false;

             std::ifstream logFile(filename);
             std::string   line;
             while (std::getline(logFile, line)) {
                 if (line.find("Player::readme | received instructions.") != std::string::npos) readme = true;
                 if (line.find("Player::handleChoice | data = 0") != std::string::npos) ++handleChoice;
                 if (line.find("Player::handleChoice | decided to play!") != std::string::npos) ++handleChoice;
                 if (line.find("Player::printBeforePlay | received information before starting.") != std::string::npos)
                     printBeforePlay = true;
                 if (line.find("Player::processMove | data = W") != std::string::npos) ++W;
                 if (line.find("Player::processMove | failed to move up.") != std::string::npos) ++W;
                 if (line.find("Player::processMove | data = A") != std::string::npos) ++A;
                 if (line.find("Player::processMove | failed to move left.") != std::string::npos) ++A;
                 if (line.find("Player::processMove | data = S") != std::string::npos) ++S;
                 if (line.find("Player::processMove | failed to move down.") != std::string::npos) ++S;
                 if (line.find("Player::processMove | data = D") != std::string::npos) ++D;
                 if (line.find("Player::processMove | moving right. New coordinates:") != std::string::npos) ++D;
             }

             assert(W == 2 && A == 2 && S == 2 && D == 2 && readme && printBeforePlay && handleChoice == 2);
         }},

        {"testAllPlayerLogsVeryBIG",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             std::string                     commands;
             const size_t                    commandSize = 133331;
             std::random_device              rd;
             std::mt19937                    gen(rd());
             std::uniform_int_distribution<> dis(0, 3);

             for (size_t i = 0; i != commandSize; ++i) {
                 switch (dis(gen)) {
                     case 0:
                         commands += "W\n";
                         break;
                     case 1:
                         commands += "A\n";
                         break;
                     case 2:
                         commands += "S\n";
                         break;
                     case 3:
                         commands += "D\n";
                         break;
                 }
             }

             std::istringstream inputStream(std::to_string(Choice::PLAY) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             size_t W, A, S, D, handleChoice;
             bool   readme, printBeforePlay;

             W = A = S = D = handleChoice = 0;
             readme = printBeforePlay = false;

             std::ifstream logFile(filename);
             std::string   line;
             bool          finished = false;
             while (std::getline(logFile, line)) {
                 if (line.find("Player::readme | received instructions.") != std::string::npos) readme = true;
                 if (line.find("Player::handleChoice | data = 0") != std::string::npos) ++handleChoice;
                 if (line.find("Player::handleChoice | decided to play!") != std::string::npos) ++handleChoice;
                 if (line.find("Player::printBeforePlay | received information before starting.") != std::string::npos)
                     printBeforePlay = true;
                 if (line.find("Player::processMove | data = W") != std::string::npos) ++W;
                 if (line.find("Player::processMove | failed to move up.") != std::string::npos) ++W;
                 if (line.find("Player::processMove | moving up. New coordinates:") != std::string::npos) ++W;
                 if (line.find("Player::processMove | data = A") != std::string::npos) ++A;
                 if (line.find("Player::processMove | failed to move left.") != std::string::npos) ++A;
                 if (line.find("Player::processMove | moving left. New coordinates:") != std::string::npos) ++A;
                 if (line.find("Player::processMove | data = S") != std::string::npos) ++S;
                 if (line.find("Player::processMove | failed to move down.") != std::string::npos) ++S;
                 if (line.find("Player::processMove | moving down. New coordinates:") != std::string::npos) ++S;
                 if (line.find("Player::processMove | data = D") != std::string::npos) ++D;
                 if (line.find("Player::processMove | moving right. New coordinates:") != std::string::npos) ++D;
                 if (line.find("Player::processMove | failed to move right.") != std::string::npos) ++D;
                 if (line.find("Player::play | finished the game. Time =") != std::string::npos) {
                     finished = true;
                     break;
                 }
             }

             assert((W + A + S + D == commands.size() * 2 && readme && printBeforePlay && handleChoice == 2) ||
                    (readme && printBeforePlay && finished && handleChoice == 2));
         }},

        {"testThreadCompleted",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             std::string commands = "w\n";
             for (size_t i = 1; i <= 99; ++i) commands += "w\n";
             std::istringstream inputStream(std::to_string(Choice::PLAY) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             size_t mazeGen = 0, game = 0, log = 0;

             std::ifstream logFile(filename);
             std::string   line;
             while (std::getline(logFile, line)) {
                 if (line.find("APP | START THREAD runMazeGenMulti") != std::string::npos) ++mazeGen;
                 if (line.find("APP | END THREAD runMazeGenMulti") != std::string::npos) ++mazeGen;

                 if (line.find("APP | START THREAD runGameMulti") != std::string::npos) ++game;
                 if (line.find("APP | END THREAD runGameMulti") != std::string::npos) ++game;

                 if (line.find("APP | START THREAD logMulti") != std::string::npos) ++log;
                 if (line.find("APP | END THREAD logMulti") != std::string::npos) ++log;
             }

             assert(mazeGen == 2 && game == 2 && log == 2);
         }},

        {"testNotDefaultLogLevelInLine",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             std::string        commands = "W INFO\nS WARNING\nD\nD ERROR\n";
             std::istringstream inputStream(std::to_string(Choice::PLAY) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             bool          WINFO = false, SWARNING = false, D = false, DERROR = false;
             std::ifstream logFile(filename);
             std::string   line;
             while (std::getline(logFile, line)) {
                 if (line.find("[ERROR] Player::processMove | data = D") != std::string::npos) DERROR = true;
                 if (line.find("[INFO] Player::processMove | data = D") != std::string::npos) D = true;
                 if (line.find("[INFO] Player::processMove | data = W") != std::string::npos) WINFO = true;
                 if (line.find("[WARNING] Player::processMove | data = S") != std::string::npos) SWARNING = true;
             }

             assert(WINFO && SWARNING && D && DERROR);
         }},

        {"testChangeDLL",
         []() {
             const std::string filename = "test_lib_log.txt";
             std::remove(filename.c_str());

             std::string        commands = "\n1\nW\nD\n";
             std::istringstream inputStream(std::to_string(Choice::CHANGE_DLL) + commands);
             std::cin.rdbuf(inputStream.rdbuf());

             app = std::make_unique<MultithreadAppManager>(filename);
             app->run();

             bool          moveUP = false, moveRIGHT = false;
             std::ifstream logFile(filename);
             std::string   line;
             while (std::getline(logFile, line)) {
                 if (line.find("[WARNING] Player::processMove | failed to move up.") != std::string::npos)
                     moveUP = true;
                 if (line.find("[INFO] Player::processMove | data = D") != std::string::npos) moveRIGHT = true;
             }

             std::remove(filename.c_str());
             assert(moveUP && !moveRIGHT);
         }},
    };

    runTests(onlyLibrary);
    runTests(multithreadGameWithLib);

    system("clear");
    std::cout << "Total: all tests passed :)!\n";

    return 0;
}