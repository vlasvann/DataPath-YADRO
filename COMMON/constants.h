#pragma once

#include <cstddef>
#include <cstdint>

namespace common::constants::app {
inline constexpr int ExpectedArgumentCount = 4;
inline constexpr int InputPathArgumentIndex = 1;
inline constexpr int OutputPathArgumentIndex = 2;
inline constexpr int ConfigPathArgumentIndex = 3;
} // namespace common::constants::app

namespace common::constants::tape {
inline constexpr std::size_t IntSizeBytes = sizeof(std::int32_t);
} // namespace common::constants::tape

namespace common::constants::sort {
inline constexpr std::size_t MinMergeFanIn = 2;
inline constexpr std::size_t DefaultPageSizeBytes = 4096;
inline constexpr const char *DefaultTempDirectory = "tmp";
inline constexpr const char *TempRunPrefix = "run_";
inline constexpr const char *TempRunExtension = ".bin";
} // namespace common::constants::sort

namespace common::constants::config {
inline constexpr const char *MemoryLimitBytesKey = "memory_limit_bytes";
inline constexpr const char *TempDirectoryKey = "temp_dir";
inline constexpr const char *DelaysKey = "delays";
inline constexpr const char *ReadDelayMsKey = "read_ms";
inline constexpr const char *WriteDelayMsKey = "write_ms";
inline constexpr const char *MoveDelayMsKey = "move_ms";
inline constexpr const char *RewindDelayMsKey = "rewind_ms";
} // namespace common::constants::config
