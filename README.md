[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](http://www.repostatus.org/badges/latest/active.svg)](http://www.repostatus.org/#active)
[![license](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/aminsung/CBEAR2/blob/master/LICENSE)

# CBEAR
CBEAR is a C++ SDK to communicate with the BEAR actuators developed in RoMeLa.

## Author
Name: Min Sung Ahn
Email: aminsung@ucla.edu

## Notes
It is advised to use the Boosted USB-to-RS485 device for maximum speed. Use at own risk.

## Installation Procedures
1. Modify the permissions of your computer such that CBEAR can access the serial port.
```bash
sudo chown -R your_username /usr/local
sudo usermod -a -G dialout your_username
```

2. Download the code.
```bash
git clone https://github.com/RoMeLaUCLA/CBEAR.git
```

3. Include the project into your code.

## Example
```c++
#include "bear_sdk.h"
#include "bear_macro.h"

// initialize BEAR instance
bear::BEAR bear_handle = bear::BEAR("/dev/ttyUSB0", 8000000);

// get present position
float ret_val{0.0};
ret_val = bear_handle.GetPresentPosition(1);

// bulk read/write
std::vector<uint8_t> mIDs{1,2};
std::vector<uint8_t> write_add{bear_macro::GOAL_VELOCITY, bear_macro::GOAL_POSITION};
std::vector<uint8_t> read_add{bear_macro::PRESENT_POSITION, bear_macro::PRESENT_VELOCITY};
std::vector<std::vector<float>> data{{0.0, 0.0}, {0.0, 0.0}};
std::vector<std::vector<float>> ret_vec_rw;
ret_vec_rw = bear_handle.BulkReadWrite(mIDs, read_add, write_add, data);
```