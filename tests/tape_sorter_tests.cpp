#include "file_tape.h"
#include "tape_sorter.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

namespace {

std::filesystem::path MakeTempPath(const std::string &name) {
  return std::filesystem::temp_directory_path() / name;
}

void RemoveIfExists(const std::filesystem::path &path) {
  std::error_code error;
  std::filesystem::remove(path, error);
}

void WriteBinaryFile(const std::filesystem::path &path,
                     const std::vector<std::int32_t> &values) {
  std::ofstream output(path, std::ios::binary | std::ios::trunc);

  for (std::int32_t value : values) {
    output.write(reinterpret_cast<const char *>(&value), sizeof(value));
  }
}

std::vector<std::int32_t> ReadBinaryFile(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);

  std::vector<std::int32_t> values;
  std::int32_t value = 0;

  while (input.read(reinterpret_cast<char *>(&value), sizeof(value))) {
    values.push_back(value);
  }

  return values;
}

} // namespace

TEST(TapeSorterTests, SortsSmallInput) {
  const auto inputPath = MakeTempPath("tape_sort_input_small.bin");
  const auto outputPath = MakeTempPath("tape_sort_output_small.bin");
  const auto tempDir = MakeTempPath("tape_sort_tmp_small");

  RemoveIfExists(inputPath);
  RemoveIfExists(outputPath);
  std::filesystem::remove_all(tempDir);

  WriteBinaryFile(inputPath, {7, 3, 9, 1, 5, 8, 2, 4});

  {
    auto inputTape = FileTape::Create(inputPath, FileTape::TapeOpenMode::Read);
    auto outputTape =
        FileTape::Create(outputPath, FileTape::TapeOpenMode::Write);

    ASSERT_NE(inputTape, nullptr);
    ASSERT_NE(outputTape, nullptr);

    TapeSorter sorter(16, tempDir);
    sorter.Sort(*inputTape, *outputTape);
  }

  const auto result = ReadBinaryFile(outputPath);
  const std::vector<std::int32_t> expected{1, 2, 3, 4, 5, 7, 8, 9};

  EXPECT_EQ(result, expected);

  RemoveIfExists(inputPath);
  RemoveIfExists(outputPath);
  std::filesystem::remove_all(tempDir);
}

TEST(TapeSorterTests, SortsDuplicatesAndNegativeValues) {
  const auto inputPath = MakeTempPath("tape_sort_input_duplicates.bin");
  const auto outputPath = MakeTempPath("tape_sort_output_duplicates.bin");
  const auto tempDir = MakeTempPath("tape_sort_tmp_duplicates");

  RemoveIfExists(inputPath);
  RemoveIfExists(outputPath);
  std::filesystem::remove_all(tempDir);

  WriteBinaryFile(inputPath, {5, -1, 3, 5, 0, -1, 10, 3});

  {
    auto inputTape = FileTape::Create(inputPath, FileTape::TapeOpenMode::Read);
    auto outputTape =
        FileTape::Create(outputPath, FileTape::TapeOpenMode::Write);

    ASSERT_NE(inputTape, nullptr);
    ASSERT_NE(outputTape, nullptr);

    TapeSorter sorter(12, tempDir);
    sorter.Sort(*inputTape, *outputTape);
  }

  const auto result = ReadBinaryFile(outputPath);
  const std::vector<std::int32_t> expected{-1, -1, 0, 3, 3, 5, 5, 10};

  EXPECT_EQ(result, expected);

  RemoveIfExists(inputPath);
  RemoveIfExists(outputPath);
  std::filesystem::remove_all(tempDir);
}

TEST(TapeSorterTests, EmptyInputProducesEmptyOutput) {
  const auto inputPath = MakeTempPath("tape_sort_input_empty.bin");
  const auto outputPath = MakeTempPath("tape_sort_output_empty.bin");
  const auto tempDir = MakeTempPath("tape_sort_tmp_empty");

  RemoveIfExists(inputPath);
  RemoveIfExists(outputPath);
  std::filesystem::remove_all(tempDir);

  WriteBinaryFile(inputPath, {});

  {
    auto inputTape = FileTape::Create(inputPath, FileTape::TapeOpenMode::Read);
    auto outputTape =
        FileTape::Create(outputPath, FileTape::TapeOpenMode::Write);

    ASSERT_NE(inputTape, nullptr);
    ASSERT_NE(outputTape, nullptr);

    TapeSorter sorter(16, tempDir);
    sorter.Sort(*inputTape, *outputTape);
  }

  const auto result = ReadBinaryFile(outputPath);

  EXPECT_TRUE(result.empty());

  RemoveIfExists(inputPath);
  RemoveIfExists(outputPath);
  std::filesystem::remove_all(tempDir);
}