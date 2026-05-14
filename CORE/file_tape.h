#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>

#include "itape.h"
#include "tape_config.h"

class FileTape final : public ITape {
public:
  enum class TapeOpenMode { Read, Write, ReadWrite };

  static std::unique_ptr<FileTape>
  Create(std::filesystem::path path, TapeOpenMode mode, TapeDelays delays = {});

  bool Read(std::int32_t &value) override;
  void Write(std::int32_t value) override;

  void MoveLeft() override;
  void MoveRight() override;
  void Rewind() override;

private:
  FileTape(std::filesystem::path path, TapeDelays delays);

  bool Open(TapeOpenMode mode);
  void Sleep(std::chrono::milliseconds delay) const;

  static std::streamoff ToByteOffset(std::size_t position);

private:
  std::filesystem::path path_;
  std::fstream file_;
  TapeDelays delays_;

  std::size_t position_{0};

  std::optional<std::size_t> readPosition_;
  std::optional<std::size_t> writePosition_;
};
