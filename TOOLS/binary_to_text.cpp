#include <cstdint>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: binary_to_text <input.bin> <output.txt>\n";
    return 1;
  }

  std::ifstream input(argv[1], std::ios::binary);
  std::ofstream output(argv[2]);

  if (!input.is_open() || !output.is_open()) {
    return 1;
  }

  std::int32_t value = 0;
  while (input.read(reinterpret_cast<char *>(&value), sizeof(value))) {
    output << value << '\n';
  }

  if (!input.eof()) {
    return 1;
  }

  return 0;
}
