#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

#include "itape.h"
#include "run.h"
#include "tape_config.h"

class TapeSorter {
public:
  TapeSorter(std::size_t memoryLimitBytes, std::filesystem::path tempDirectory,
             TapeDelays delays = {});

  void Sort(ITape &inputTape, ITape &outputTape);

private:
  std::vector<Run> CreateInitialRuns(ITape &inputTape);
  std::vector<Run> MergeIteration(const std::vector<Run> &runs);
  Run MergeGroup(const std::vector<Run> &runs);

  void CopyRunToOutput(const Run &run, ITape &outputTape);

  std::filesystem::path MakeTempRunPath();

  std::size_t ItemsPerBlock() const;
  std::size_t MergeFanIn() const;

private:
  std::size_t memoryLimitBytes_{0};
  std::filesystem::path tempDirectory_;
  TapeDelays delays_;

  std::size_t tempRunCounter_{0};
};
