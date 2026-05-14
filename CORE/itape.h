#pragma once

#include <cstdint>

class ITape {
public:
  virtual ~ITape() = default;

  virtual bool Read(std::int32_t &value) = 0;
  virtual void Write(std::int32_t value) = 0;

  virtual void MoveLeft() = 0;
  virtual void MoveRight() = 0;
  virtual void Rewind() = 0;
};
