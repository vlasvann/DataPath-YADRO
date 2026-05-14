#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "file_tape.h"
#include "run.h"
#include "tape_config.h"

class RunReader {
public:
  RunReader(const Run &run, TapeDelays delays = {});

  bool Open();

  bool HasValue() const;
  std::int32_t Value() const;

  void Advance();

private:
  Run run_;
  TapeDelays delays_;

  std::unique_ptr<FileTape> tape_;

  std::size_t readCount_{0};
  bool hasValue_{false};
  std::int32_t currentValue_{0};
};
