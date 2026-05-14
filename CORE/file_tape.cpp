#include "file_tape.h"

#include <stdexcept>
#include <thread>
#include <utility>

#include "constants.h"

std::unique_ptr<FileTape> FileTape::Create(std::filesystem::path path,
                                           TapeOpenMode mode,
                                           TapeDelays delays) {
  auto tape = std::unique_ptr<FileTape>(new FileTape(std::move(path), delays));

  if (!tape->Open(mode)) {
    return nullptr;
  }

  return tape;
}

FileTape::FileTape(std::filesystem::path path, TapeDelays delays)
    : path_(std::move(path)), delays_(delays) {}

bool FileTape::Open(TapeOpenMode mode) {
  std::ios::openmode openMode = std::ios::binary;

  switch (mode) {
  case TapeOpenMode::Read:
    openMode |= std::ios::in;
    break;
  case TapeOpenMode::Write:
    openMode |= std::ios::out | std::ios::trunc;
    break;
  case TapeOpenMode::ReadWrite:
    openMode |= std::ios::in | std::ios::out;
    break;
  }

  file_.open(path_, openMode);

  if (!file_.is_open()) {
    return false;
  }

  position_ = 0;
  readPosition_.reset();
  writePosition_.reset();

  return true;
}

bool FileTape::Read(std::int32_t &value) {
  Sleep(delays_.read);

  if (!readPosition_ || *readPosition_ != position_) {
    file_.clear();
    file_.seekg(ToByteOffset(position_), std::ios::beg);

    if (!file_) {
      throw std::runtime_error("Failed to seek read position in tape: " +
                               path_.string());
    }
  }

  file_.read(reinterpret_cast<char *>(&value), sizeof(value));

  if (file_.gcount() == 0 && file_.eof()) {
    readPosition_.reset();
    return false;
  }

  if (file_.gcount() != static_cast<std::streamsize>(sizeof(value))) {
    readPosition_.reset();
    throw std::runtime_error("Failed to read full int32 from tape: " +
                             path_.string());
  }

  readPosition_ = position_ + 1;
  return true;
}

void FileTape::Write(std::int32_t value) {
  Sleep(delays_.write);

  if (!writePosition_ || *writePosition_ != position_) {
    file_.clear();
    file_.seekp(ToByteOffset(position_), std::ios::beg);

    if (!file_) {
      throw std::runtime_error("Failed to seek write position in tape: " +
                               path_.string());
    }
  }

  file_.write(reinterpret_cast<const char *>(&value), sizeof(value));

  if (!file_) {
    writePosition_.reset();
    throw std::runtime_error("Failed to write int32 to tape: " +
                             path_.string());
  }

  writePosition_ = position_ + 1;
}

void FileTape::MoveLeft() {
  Sleep(delays_.move);

  if (position_ == 0) {
    throw std::runtime_error("Cannot move tape left before beginning: " +
                             path_.string());
  }

  --position_;
}

void FileTape::MoveRight() {
  Sleep(delays_.move);
  ++position_;
}

void FileTape::Rewind() {
  Sleep(delays_.rewind);
  position_ = 0;
}

void FileTape::Sleep(std::chrono::milliseconds delay) const {
  if (delay.count() > 0) {
    std::this_thread::sleep_for(delay);
  }
}

std::streamoff FileTape::ToByteOffset(std::size_t position) {
  return static_cast<std::streamoff>(
      position * common::constants::tape::IntSizeBytes);
}
