#include "tape_sorter.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>

#include "constants.h"
#include "run_reader.h"
#include "run_writer.h"

namespace {

struct HeapItem {
  std::int32_t value;
  std::size_t readerIndex;
};

struct HeapItemCompare {
  bool operator()(const HeapItem &lhs, const HeapItem &rhs) const {
    return lhs.value > rhs.value;
  }
};

} // namespace

TapeSorter::TapeSorter(std::size_t memoryLimitBytes,
                       std::filesystem::path tempDirectory, TapeDelays delays)
    : memoryLimitBytes_(memoryLimitBytes),
      tempDirectory_(std::move(tempDirectory)), delays_(delays) {
  if (memoryLimitBytes_ < common::constants::tape::IntSizeBytes) {
    throw std::runtime_error("Memory limit is too small");
  }

  std::filesystem::create_directories(tempDirectory_);
}

void TapeSorter::Sort(ITape &inputTape, ITape &outputTape) {
  std::vector<Run> runs = CreateInitialRuns(inputTape);

  if (runs.empty()) {
    return;
  }

  while (runs.size() > 1) {
    runs = MergeIteration(runs);
  }

  CopyRunToOutput(runs.front(), outputTape);
}

std::size_t TapeSorter::ItemsPerBlock() const {
  return memoryLimitBytes_ / common::constants::tape::IntSizeBytes;
}

std::size_t TapeSorter::MergeFanIn() const {
  const std::size_t bufferCount =
      memoryLimitBytes_ / common::constants::sort::DefaultPageSizeBytes;

  if (bufferCount <= 1) {
    return common::constants::sort::MinMergeFanIn;
  }

  return std::max(common::constants::sort::MinMergeFanIn, bufferCount - 1);
}

std::vector<Run> TapeSorter::CreateInitialRuns(ITape &inputTape) {
  std::vector<Run> runs;
  std::vector<std::int32_t> buffer;
  buffer.reserve(ItemsPerBlock());

  std::int32_t value = 0;

  while (inputTape.Read(value)) {
    buffer.push_back(value);
    inputTape.MoveRight();

    if (buffer.size() == ItemsPerBlock()) {
      std::sort(buffer.begin(), buffer.end());

      RunWriter writer(MakeTempRunPath(), delays_);

      if (!writer.Open()) {
        throw std::runtime_error("Failed to create temp run");
      }

      for (std::int32_t item : buffer) {
        writer.Write(item);
      }

      runs.push_back(writer.Finish());
      buffer.clear();
    }
  }

  if (!buffer.empty()) {
    std::sort(buffer.begin(), buffer.end());

    RunWriter writer(MakeTempRunPath(), delays_);

    if (!writer.Open()) {
      throw std::runtime_error("Failed to create temp run");
    }

    for (std::int32_t item : buffer) {
      writer.Write(item);
    }

    runs.push_back(writer.Finish());
  }

  return runs;
}

std::vector<Run> TapeSorter::MergeIteration(const std::vector<Run> &runs) {
  std::vector<Run> mergedRuns;

  const std::size_t fanIn = MergeFanIn();

  for (std::size_t i = 0; i < runs.size(); i += fanIn) {
    const std::size_t groupEnd = std::min(i + fanIn, runs.size());

    std::vector<Run> group;
    group.reserve(groupEnd - i);

    for (std::size_t j = i; j < groupEnd; ++j) {
      group.push_back(runs[j]);
    }

    mergedRuns.push_back(MergeGroup(group));
  }

  return mergedRuns;
}

Run TapeSorter::MergeGroup(const std::vector<Run> &runs) {
  if (runs.empty()) {
    throw std::runtime_error("Cannot merge empty run group");
  }

  if (runs.size() == 1) {
    return runs.front();
  }

  std::vector<RunReader> readers;
  readers.reserve(runs.size());

  for (const Run &run : runs) {
    readers.emplace_back(run, delays_);

    if (!readers.back().Open()) {
      throw std::runtime_error("Failed to open run for reading");
    }
  }

  RunWriter writer(MakeTempRunPath(), delays_);

  if (!writer.Open()) {
    throw std::runtime_error("Failed to create merged run");
  }

  std::priority_queue<HeapItem, std::vector<HeapItem>, HeapItemCompare> heap;

  for (std::size_t i = 0; i < readers.size(); ++i) {
    if (readers[i].HasValue()) {
      heap.push(HeapItem{readers[i].Value(), i});
    }
  }

  while (!heap.empty()) {
    HeapItem item = heap.top();
    heap.pop();

    writer.Write(item.value);

    RunReader &reader = readers[item.readerIndex];
    reader.Advance();

    if (reader.HasValue()) {
      heap.push(HeapItem{reader.Value(), item.readerIndex});
    }
  }

  return writer.Finish();
}

void TapeSorter::CopyRunToOutput(const Run &run, ITape &outputTape) {
  RunReader reader(run, delays_);

  if (!reader.Open()) {
    throw std::runtime_error("Failed to open final run");
  }

  while (reader.HasValue()) {
    outputTape.Write(reader.Value());
    outputTape.MoveRight();

    reader.Advance();
  }
}

std::filesystem::path TapeSorter::MakeTempRunPath() {
  const std::string filename = std::string{common::constants::sort::TempRunPrefix} +
                               std::to_string(tempRunCounter_++) +
                               common::constants::sort::TempRunExtension;

  return tempDirectory_ / filename;
}
