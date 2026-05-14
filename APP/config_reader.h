#pragma once

#include <filesystem>

#include "tape_config.h"

class ConfigReader {
public:
  static bool Read(const std::filesystem::path &path, TapeConfig &config);
};
