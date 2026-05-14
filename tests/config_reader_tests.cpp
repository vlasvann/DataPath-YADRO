#include "config_reader.h"

#include <chrono>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace {

std::filesystem::path MakeTempPath(const std::string &name) {
  return std::filesystem::temp_directory_path() / name;
}

void RemoveIfExists(const std::filesystem::path &path) {
  std::error_code error;
  std::filesystem::remove(path, error);
}

} // namespace

TEST(ConfigReaderTests, ReadsValidConfig) {
  const auto path = MakeTempPath("tape_sort_config_valid.json");
  RemoveIfExists(path);

  {
    std::ofstream output(path);
    output << R"({
      "memory_limit_bytes": 1024,
      "temp_dir": "tmp_test",
      "delays": {
        "read_ms": 1,
        "write_ms": 2,
        "move_ms": 3,
        "rewind_ms": 4
      }
    })";
  }

  TapeConfig config;

  ASSERT_TRUE(ConfigReader::Read(path, config));

  EXPECT_EQ(config.memoryLimitBytes, 1024);
  EXPECT_EQ(config.tempDirectory, std::filesystem::path("tmp_test"));
  EXPECT_EQ(config.delays.read, std::chrono::milliseconds(1));
  EXPECT_EQ(config.delays.write, std::chrono::milliseconds(2));
  EXPECT_EQ(config.delays.move, std::chrono::milliseconds(3));
  EXPECT_EQ(config.delays.rewind, std::chrono::milliseconds(4));

  RemoveIfExists(path);
}

TEST(ConfigReaderTests, ReturnsFalseForMissingConfig) {
  const auto path = MakeTempPath("tape_sort_missing_config.json");
  RemoveIfExists(path);

  TapeConfig config;

  EXPECT_FALSE(ConfigReader::Read(path, config));
}

TEST(ConfigReaderTests, ReturnsFalseForInvalidConfig) {
  const auto path = MakeTempPath("tape_sort_invalid_config.json");
  RemoveIfExists(path);

  {
    std::ofstream output(path);
    output << R"({
      "temp_dir": "tmp_test"
    })";
  }

  TapeConfig config;

  EXPECT_FALSE(ConfigReader::Read(path, config));

  RemoveIfExists(path);
}