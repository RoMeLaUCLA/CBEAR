//
// Created by msahn on 3/11/21.
//
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

#include <iostream>
#include <chrono>

#include "bear_sdk.h"
#include "bear_macro.h"

// Settings
#define DEVICENAME "/dev/ttyUSB0"

int main(int argc, char *argv[]) {
  std::cout << "RoMeLa CBEAR sample code." << std::endl;

  char *dev_name = (char *) DEVICENAME;

  // Initialize BEAR instance
  bear::BEAR bear_handle = bear::BEAR(dev_name, 8000000);

  // Sample command
  float ret_val{0.0};
  ret_val = bear_handle.GetPresentPosition(2);

  // Sample bulk read write
  std::vector<uint8_t> mIDs{1, 2};
  std::vector<uint8_t> write_add{bear_macro::GOAL_VELOCITY, bear_macro::GOAL_POSITION};
  std::vector<uint8_t> read_add{bear_macro::PRESENT_POSITION, bear_macro::PRESENT_VELOCITY, bear_macro::PRESENT_IQ};
  std::vector<std::vector<float>> data{{0.0, 0.0},
                                       {0.0, 0.0}};
  std::vector<std::vector<float>> ret_vec_rw;
  ret_vec_rw = bear_handle.BulkReadWrite(mIDs, read_add, write_add, data);

}
