#pragma once

#include <memory>

#include "itape.h"
#include "tape_config.h"

class App {
public:
  bool Init(int argc, char *argv[]);
  int Run();

private:
  bool initialized_{false};

  TapeConfig config_;

  std::unique_ptr<ITape> inputTape_;
  std::unique_ptr<ITape> outputTape_;
};