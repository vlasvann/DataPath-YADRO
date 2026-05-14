#include "run_reader.h"

#include <stdexcept>

RunReader::RunReader(const Run &run, TapeDelays delays)
    : run_(run), delays_(delays) {}

bool RunReader::Open() {
  tape_ = FileTape::Create(run_.path, FileTape::TapeOpenMode::Read, delays_);

  if (!tape_) {
    return false;
  }

  readCount_ = 0;
  hasValue_ = false;
  currentValue_ = 0;

  Advance();
  return true;
}

bool RunReader::HasValue() const { return hasValue_; }

std::int32_t RunReader::Value() const {
  if (!hasValue_) {
    throw std::runtime_error("RunReader has no current value");
  }

  return currentValue_;
}

void RunReader::Advance() {
  if (!tape_) {
    throw std::runtime_error("RunReader is not opened");
  }

  if (readCount_ >= run_.size) {
    hasValue_ = false;
    return;
  }

  if (!tape_->Read(currentValue_)) {
    hasValue_ = false;
    return;
  }

  tape_->MoveRight();

  ++readCount_;
  hasValue_ = true;
}
