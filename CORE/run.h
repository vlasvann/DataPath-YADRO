#pragma once

#include <cstddef>
#include <filesystem>

struct Run {
  std::filesystem::path path;
  std::size_t size{0};
};
