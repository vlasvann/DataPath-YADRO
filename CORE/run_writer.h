#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

#include "file_tape.h"
#include "run.h"
#include "tape_config.h"

class RunWriter {
public:
  RunWriter(std::filesystem::path path, TapeDelays delays = {});

  bool Open();

  void Write(std::int32_t value);

  Run Finish();

private:
  std::filesystem::path path_;
  TapeDelays delays_;

  std::unique_ptr<FileTape> tape_;
  std::size_t size_{0};
};
