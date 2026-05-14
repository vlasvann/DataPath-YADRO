#pragma once

#include <chrono>
#include <cstddef>
#include <filesystem>

#include "constants.h"

struct TapeDelays {
  std::chrono::milliseconds read{0};
  std::chrono::milliseconds write{0};
  std::chrono::milliseconds move{0};
  std::chrono::milliseconds rewind{0};
};

struct TapeConfig {
  std::size_t memoryLimitBytes{0};
  std::filesystem::path tempDirectory{
      common::constants::sort::DefaultTempDirectory};
  TapeDelays delays;
};
