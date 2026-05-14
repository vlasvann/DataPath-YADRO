#include "run_writer.h"

#include <stdexcept>
#include <utility>

RunWriter::RunWriter(std::filesystem::path path, TapeDelays delays)
    : path_(std::move(path)), delays_(delays) {}

bool RunWriter::Open() {
  tape_ = FileTape::Create(path_, FileTape::TapeOpenMode::Write, delays_);

  if (!tape_) {
    return false;
  }

  size_ = 0;
  return true;
}

void RunWriter::Write(std::int32_t value) {
  if (!tape_) {
    throw std::runtime_error("RunWriter is not opened");
  }

  tape_->Write(value);
  tape_->MoveRight();

  ++size_;
}

Run RunWriter::Finish() { return Run{path_, size_}; }
