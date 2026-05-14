#include "app.h"

#include "config_reader.h"
#include "file_tape.h"
#include "tape_sorter.h"

#include <exception>
#include <filesystem>
#include <iostream>

bool App::Init(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: tape_sort <input.bin> <output.bin> <config.json>\n";
    return false;
  }

  try {
    const std::filesystem::path inputPath{argv[1]};
    const std::filesystem::path outputPath{argv[2]};
    const std::filesystem::path configPath{argv[3]};

    if (!ConfigReader::Read(configPath, config_)) {
      std::cerr << "Failed to read config: " << configPath << '\n';
      return false;
    }

    inputTape_ = FileTape::Create(inputPath, FileTape::TapeOpenMode::Read,
                                  config_.delays);

    if (!inputTape_) {
      std::cerr << "Failed to open input tape: " << inputPath << '\n';
      return false;
    }

    outputTape_ = FileTape::Create(outputPath, FileTape::TapeOpenMode::Write,
                                   config_.delays);

    if (!outputTape_) {
      std::cerr << "Failed to open output tape: " << outputPath << '\n';
      return false;
    }

    initialized_ = true;
    return true;
  } catch (const std::exception &ex) {
    std::cerr << "Initialization error: " << ex.what() << '\n';
    return false;
  }
}

int App::Run() {
  if (!initialized_ || !inputTape_ || !outputTape_) {
    std::cerr << "Application is not initialized\n";
    return 1;
  }

  try {
    TapeSorter sorter(config_.memoryLimitBytes, config_.tempDirectory,
                      config_.delays);

    sorter.Sort(*inputTape_, *outputTape_);

    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "Runtime error: " << ex.what() << '\n';
    return 1;
  }
}