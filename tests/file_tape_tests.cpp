#include "file_tape.h"

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

} // namespace

TEST(FileTapeTests, WriteAndReadSingleValue) {
  const auto path = MakeTempPath("file_tape_single_value.bin");
  RemoveIfExists(path);

  {
    auto tape = FileTape::Create(path, FileTape::TapeOpenMode::Write);
    ASSERT_NE(tape, nullptr);

    tape->Write(42);
  }

  {
    auto tape = FileTape::Create(path, FileTape::TapeOpenMode::Read);
    ASSERT_NE(tape, nullptr);

    std::int32_t value = 0;
    EXPECT_TRUE(tape->Read(value));
    EXPECT_EQ(value, 42);
  }

  RemoveIfExists(path);
}

TEST(FileTapeTests, ReadWriteDoesNotMovePositionAutomatically) {
  const auto path = MakeTempPath("file_tape_position.bin");
  RemoveIfExists(path);

  {
    auto tape = FileTape::Create(path, FileTape::TapeOpenMode::Write);
    ASSERT_NE(tape, nullptr);

    tape->Write(10);
    tape->MoveRight();

    tape->Write(20);
    tape->MoveRight();

    tape->Write(30);
  }

  {
    auto tape = FileTape::Create(path, FileTape::TapeOpenMode::Read);
    ASSERT_NE(tape, nullptr);

    std::int32_t value = 0;

    ASSERT_TRUE(tape->Read(value));
    EXPECT_EQ(value, 10);

    ASSERT_TRUE(tape->Read(value));
    EXPECT_EQ(value, 10);

    tape->MoveRight();

    ASSERT_TRUE(tape->Read(value));
    EXPECT_EQ(value, 20);
  }

  RemoveIfExists(path);
}

TEST(FileTapeTests, MoveLeftReturnsToPreviousCell) {
  const auto path = MakeTempPath("file_tape_move_left.bin");
  RemoveIfExists(path);

  {
    auto tape = FileTape::Create(path, FileTape::TapeOpenMode::Write);
    ASSERT_NE(tape, nullptr);

    tape->Write(1);
    tape->MoveRight();

    tape->Write(2);
    tape->MoveRight();

    tape->Write(3);
  }

  {
    auto tape = FileTape::Create(path, FileTape::TapeOpenMode::Read);
    ASSERT_NE(tape, nullptr);

    std::int32_t value = 0;

    tape->MoveRight();
    tape->MoveRight();

    ASSERT_TRUE(tape->Read(value));
    EXPECT_EQ(value, 3);

    tape->MoveLeft();

    ASSERT_TRUE(tape->Read(value));
    EXPECT_EQ(value, 2);
  }

  RemoveIfExists(path);
}