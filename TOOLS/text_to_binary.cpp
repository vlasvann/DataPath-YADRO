#include <cstdint>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: text_to_binary <input.txt> <output.bin>\n";
    return 1;
  }

  std::ifstream input(argv[1]);
  std::ofstream output(argv[2], std::ios::binary | std::ios::trunc);

  if (!input.is_open() || !output.is_open()) {
    return 1;
  }

  std::int32_t value = 0;
  while (input >> value) {
    output.write(reinterpret_cast<const char *>(&value), sizeof(value));
    if (!output) {
      return 1;
    }
  }

  return 0;
}
