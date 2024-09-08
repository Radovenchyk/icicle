#include <iostream>
#include <cstring>
#include <chrono>
#include <string>
#include "hash/blake2/blake2s.h"
#include <chrono>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace icicle;

#define START_TIMER(timer) auto timer##_start = std::chrono::high_resolution_clock::now();
#define END_TIMER(timer, msg)                                                                                          \
  printf("%s: %.0f us\n", msg, FpMicroseconds(std::chrono::high_resolution_clock::now() - timer##_start).count());

void print_hash(uint8_t* hash, unsigned int len)
{
  printf("Hash Len: %d \n", len);
  printf("BLAKE2S hash:\n");
  for (unsigned int i = 0; i < len; i++) {
    printf("%02x", hash[i]);
  }
  printf("\n");
}

std::string byte_to_hex(uint8_t* data, unsigned int len)
{
  std::stringstream ss;
  for (unsigned int i = 0; i < len; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
  }
  return ss.str();
}

std::vector<std::pair<std::string, std::string>> load_csv(const char* filename)
{
  std::vector<std::pair<std::string, std::string>> data;
  std::ifstream file(filename);
  std::string line;

  while (std::getline(file, line)) {
    std::stringstream line_stream(line);
    std::string test_string, hash;

    if (std::getline(line_stream, test_string, ',') && std::getline(line_stream, hash)) {
      data.push_back({test_string, hash});
    }
  }

  return data;
}

int main(int argc, char** argv)
{
  using FpMilliseconds = std::chrono::duration<float, std::chrono::milliseconds::period>;
  using FpMicroseconds = std::chrono::duration<float, std::chrono::microseconds::period>;
  bool all_ok = true;
  const char* csv_filename = "tests/test_vectors_variable_length.csv"; // Replace with your actual CSV file name
  std::vector<std::pair<std::string, std::string>> test_data = load_csv(csv_filename);

  std::cout << "Loaded test data from CSV:" << std::endl;

  // Test parameters
  HashConfig config;

  // Perform the hashing
  for (size_t i = 0; i < test_data.size(); i++) {
    const std::string& input_str = test_data[i].first;
    const std::string& expected_hash = test_data[i].second;

    uint8_t* input = (uint8_t*)input_str.c_str();
    size_t inlen = input_str.size();

    Blake2s_cpu blake2s = Blake2s_cpu(inlen / sizeof(limb_t));

    unsigned int outlen = blake2s.m_total_output_limbs * sizeof(limb_t); // Output length in bytes (32)
    uint8_t* output = (uint8_t*)malloc(outlen);
    if (!output) {
      perror("Failed to allocate memory for output");
      return EXIT_FAILURE;
    }

    // Perform the hashing
    START_TIMER(blake_timer)
    blake2s.run_single_hash((limb_t*)input, (limb_t*)output, config);
    END_TIMER(blake_timer, "Blake Timer")

    // Convert the output to hex string
    std::string computed_hash = byte_to_hex(output, outlen);

    // Compare with the expected hash
    if (computed_hash == expected_hash) {
      std::cout << "Test " << i << " passed." << std::endl;
    } else {
      std::cout << "Test " << i << " failed." << std::endl;
      std::cout << "Expected: " << expected_hash << std::endl;
      std::cout << "Got:      " << computed_hash << std::endl;
      all_ok = false;
    }

    free(output);
  }
  if (all_ok) {
    printf("ok\n");
  } else {
    printf("error!\n");
  }
  return 0;
}