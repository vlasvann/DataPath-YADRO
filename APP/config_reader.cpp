#include "config_reader.h"

#include <chrono>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include "constants.h"

namespace {

using json = nlohmann::json;

namespace config_constants = common::constants::config;
namespace sort_constants = common::constants::sort;

} // namespace

bool ConfigReader::Read(const std::filesystem::path &path, TapeConfig &config) {
  std::ifstream file(path);

  if (!file.is_open()) {
    return false;
  }

  try {
    json data;
    file >> data;

    config.memoryLimitBytes =
        data.at(config_constants::MemoryLimitBytesKey).get<std::size_t>();

    config.tempDirectory =
        data.value(config_constants::TempDirectoryKey,
                   std::string{sort_constants::DefaultTempDirectory});

    const json delays = data.value(config_constants::DelaysKey, json::object());

    config.delays.read = std::chrono::milliseconds(
        delays.value(config_constants::ReadDelayMsKey, 0));

    config.delays.write = std::chrono::milliseconds(
        delays.value(config_constants::WriteDelayMsKey, 0));

    config.delays.move = std::chrono::milliseconds(
        delays.value(config_constants::MoveDelayMsKey, 0));

    config.delays.rewind = std::chrono::milliseconds(
        delays.value(config_constants::RewindDelayMsKey, 0));

    return config.memoryLimitBytes > 0;
  } catch (...) {
    return false;
  }
}