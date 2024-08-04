//
// Created by msahn on 3/11/21.
//

#include <chrono>
#include <thread>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <numeric>
#include "packet_manager.h"

#define TX_PKT_MAX_LEN 250
#define RX_PKT_MAX_LEN 250

// Convenient Packet Macros
#define PKT_HEADER0         0
#define PKT_HEADER1         1
#define PKT_ID              2
#define PKT_LENGTH          3
#define PKT_INSTRUCTION     4
#define PKT_ERROR           4
#define PKT_PARAMETER0      5
#define PKT_BULK_N_MOTORS   5
#define PKT_BULK_N_STAT_RW  6
#define PKT_BULK_READ_REG_0 7

//#define MAX_TICKER_COUNT    400 // ret 2
//#define MAX_TICKER_COUNT    500 // ret 3

// Convenient Endian macros
#define GEN_MAKEWORD(a, b)  ((uint16_t)(((uint8_t)(((uint64_t)(a)) & 0xff)) | ((uint16_t)((uint8_t)(((uint64_t)(b)) & 0xff))) << 8))
#define GEN_MAKEDWORD(a, b) ((uint32_t)(((uint16_t)(((uint64_t)(a)) & 0xffff)) | ((uint32_t)((uint16_t)(((uint64_t)(b)) & 0xffff))) << 16))
#define GEN_LOWORD(l)       ((uint16_t)(((uint64_t)(l)) & 0xffff))
#define GEN_HIWORD(l)       ((uint16_t)((((uint64_t)(l)) >> 16) & 0xffff))
#define GEN_LOBYTE(w)       ((uint8_t)(((uint64_t)(w)) & 0xff))
#define GEN_HIBYTE(w)       ((uint8_t)((((uint64_t)(w)) >> 8) & 0xff))

using namespace bear;

PacketManager::PacketManager(const bool debug) : debug_mode_(debug) {}

void PacketManager::BuildPacket(uint8_t *packet) {
  uint8_t checksum = 0;
  uint8_t total_pkt_len = packet[PKT_LENGTH] + 4; // [HEADER0, HEADER1, ID, LENGTH] + ...
//   uint8_t write_pkt_len = 0;

  // Make packet header
  packet[PKT_HEADER0] = 0xFF;
  packet[PKT_HEADER1] = 0xFF;

  // Create checksum for the packet
  for (uint16_t idx = 2; idx < total_pkt_len - 1; idx++)
    checksum += packet[idx];
  packet[total_pkt_len - 1] = ~checksum;

//  std::cout << "Packet to be written: " << std::endl;
//  for (int a = 0; a < total_pkt_len; a++)
//    std::cout << int(packet[a]) << " ";
//  std::cout << std::endl;
}

int PacketManager::WritePacket(PortManager *port, uint8_t *packet) {
  uint8_t checksum = 0;
  uint8_t total_pkt_len = packet[PKT_LENGTH] + 4; // [HEADER0, HEADER1, ID, LENGTH] + ...
  uint8_t write_pkt_len = 0;

  // Check to see if the port is in use
  if (port->in_use_)
    return COMM_PORT_BUSY;
  port->in_use_ = true;

  // Check max packet length
  if (total_pkt_len > TX_PKT_MAX_LEN) {
    port->in_use_ = false;
    return COMM_TX_ERROR;
  }

  // Make packet header
  packet[PKT_HEADER0] = 0xFF;
  packet[PKT_HEADER1] = 0xFF;

  // Create checksum for the packet
  for (uint16_t idx = 2; idx < total_pkt_len - 1; idx++)
    checksum += packet[idx];
  packet[total_pkt_len - 1] = ~checksum;

//  std::cout << "Packet to be written: " << std::endl;
//  for (int a = 0; a < total_pkt_len; a++)
//    std::cout << int(packet[a]) << " ";
//  std::cout << std::endl;

//  uint8_t write_pkt_len = 0;
//  uint8_t total_pkt_len = packet[PKT_LENGTH] + 4;
//
//  // Check to see if the port is in use
//  if (port->in_use_)
//    return COMM_PORT_BUSY;
//  port->in_use_ = true;
//
//  // Check max packet length
//  if (total_pkt_len > TX_PKT_MAX_LEN) {
//    port->in_use_ = false;
//    return COMM_TX_ERROR;
//  }

  // Write the actual packet via PortManager
  port->ClearIOPort();
  write_pkt_len = port->WritePort(packet, total_pkt_len);
  if (total_pkt_len != write_pkt_len) // Write and written should be same
  {
    port->in_use_ = false;
    return COMM_TX_FAIL;
  }

  return COMM_SUCCESS;
}

int PacketManager::ReadPacket(PortManager *port, uint8_t *packet) {
  int result{COMM_TX_FAIL};

  uint8_t checksum = 0;
  uint8_t rx_len = 0;
  uint8_t wait_len = 6; // [HEADER0, HEADER1, ID, LENGTH, ERROR, CHKSUM]

  const double MAX_TIMEOUT = 0.005;

  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

  while (true) {
    rx_len += port->ReadPort(&packet[rx_len], wait_len - rx_len);

    if (rx_len >= wait_len) {
      uint8_t idx = 0;

      // Identify packet header
      for (idx = 0; idx < (rx_len - 1); idx++) {
        if (packet[idx] == 0xFF && packet[idx + 1] == 0xFF)
          break;
      }

      if (idx == 0) // Immediately found (i.e. packet[0], packet[1] are 0xFF, 0xFF
      {
        if (packet[PKT_ID] > 0xFD ||                    // Unavailable ID (max is 0xFC)
            packet[PKT_LENGTH] > RX_PKT_MAX_LEN)        // Unavailable length
        {
          // Remove first byte in packets
          for (uint16_t s = 0; s < rx_len - 1; s++)
            packet[s] = packet[s + 1];
          rx_len -= 1;
          continue;
        }

        // Re-calculate the length of the reading packet if the amount of packets to wait for doesn't match
        if (wait_len != packet[PKT_LENGTH] + PKT_LENGTH + 1) {
          wait_len = packet[PKT_LENGTH] + PKT_LENGTH + 1;
          continue;
        }

        // Calculate checksum
        for (uint16_t jdx = 2; jdx < wait_len - 1; jdx++)
          checksum += packet[jdx];
        checksum = ~checksum;

        // Verify checksum
        if (packet[wait_len - 1] == checksum) {
          result = COMM_SUCCESS;
        } else {
          result = COMM_RX_CORRUPT;
        }
        break;
      } else {
        // Remove unnecessary packets
        for (uint16_t s = 0; s < rx_len - idx; s++)
          packet[s] = packet[idx + s];
        rx_len -= idx;
      }
    } else {
      double tm_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1e9;
      if ( tm_duration > MAX_TIMEOUT )
      {
        std::cerr << "[ CBEAR ] Couldn't return status packet. Timing out..." << tm_duration << std::endl;
        port->in_use_ = false;
        return COMM_RX_FAIL;
      }
    }
  }
  port->in_use_ = false;

//  std::cout << "Packet to be read: " << std::endl;
//  for (int a = 0; a < rx_len; a++)
//    std::cout << int(packet[a]) << " ";
//  std::cout << std::endl;

  return result;
}

int PacketManager::ReadBulkPacket(PortManager *port, uint8_t num_motors, uint8_t *packet) {
  // Read bulk packet (manual for now) // TODO: Fix to adaptive
//   uint8_t checksum = 0;
  uint8_t rx_len = 0;
  uint8_t wait_len = 4; // [HEADER0, HEADER1, ID, LENGTH, ..., ...]

  const double MAX_TIMEOUT = 0.005;
  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

//   int ticker_initial = 0;
  while (true) {
    rx_len += port->ReadPort(&packet[rx_len], wait_len - rx_len);

    if (rx_len >= wait_len) {
      uint8_t idx = 0;

      // Identify packet header
      for (idx = 0; idx < (rx_len - 1); idx++) {
        if (packet[idx] == 0xFF && packet[idx + 1] == 0xFF)
          break;
      }

      if (idx == 0) {
        // Re-calculate the length of the reading packet if the amount of packets to wait for doesn't match
        if (wait_len != (packet[PKT_LENGTH] + PKT_LENGTH + 1)*num_motors){
          wait_len = (packet[PKT_LENGTH] + PKT_LENGTH + 1)*num_motors;
          continue;
        }
        break;
      } else {
        for (uint16_t s = 0; s < rx_len - idx; s++)
          packet[s] = packet[idx + s];
        rx_len -= idx;
      }
    }
    else
    {
      double tm_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1e9;
      if ( tm_duration > MAX_TIMEOUT )
      {
        std::cerr << "[ CBEAR ] Couldn't return status packet. Timing out..." << tm_duration << std::endl;
        port->in_use_ = false;
        return COMM_TX_FAIL;
      }
    }

//    ++ticker_initial;
//    if (ticker_initial > MAX_TICKER_COUNT) {
//      port->in_use_ = false;
////      std::cout << "Fail and leaving for now!" << std::endl;
//      return COMM_RX_FAIL;
//    }
  }

//  uint8_t len_single_pkt = packet[PKT_LENGTH];
//  uint8_t len_total_pkt =
//      PKT_LENGTH + 1 + len_single_pkt + (4 + len_single_pkt) * (num_motors - 1); // remaining incoming packets
//
//  int ticker_remaining = 0;
//  while (true) {
//    rx_len += port->ReadPort(&packet[rx_len], len_total_pkt - rx_len);
//    if (rx_len >= len_total_pkt)
//      break;
//    ++ticker_remaining;
//    if (ticker_remaining > MAX_TICKER_COUNT) {
//      port->in_use_ = false;
//      return COMM_RX_FAIL;
//    }
//  }

  port->in_use_ = false;
  return COMM_SUCCESS;
}

int PacketManager::wrPacket(PortManager *port, uint8_t *wpacket, uint8_t *rpacket, uint8_t *error) {
  int result{COMM_TX_FAIL};

  // Write packet
//  BuildPacket(wpacket); // TODO: Make this modular
  result = WritePacket(port, wpacket);
  if (result != COMM_SUCCESS)
    return result;

  // Read packet
  do {
    result = ReadPacket(port, rpacket);
  } while (result == COMM_SUCCESS && wpacket[PKT_ID] != rpacket[PKT_ID]);

  if (result == COMM_SUCCESS && wpacket[PKT_ID] == rpacket[PKT_ID]) {
    if (error != 0)
      *error = (uint8_t) rpacket[PKT_ERROR];
  }

  return result;
}

int PacketManager::wrBulkPacket(PortManager *port, uint8_t *wpacket, uint8_t *rpacket, uint8_t *error) {
  int result{COMM_TX_FAIL};

  // Write bulk packet
//  BuildPacket(wpacket); // TODO: Make this modular
  result = WritePacket(port, wpacket);
  if (result != COMM_SUCCESS)
    return result;

  // TODO: Include timeout?
  do {
    result = ReadPacket(port, rpacket);
  } while (result == COMM_SUCCESS && wpacket[PKT_ID] != rpacket[PKT_ID]);

  if (result == COMM_SUCCESS && wpacket[PKT_ID] == rpacket[PKT_ID]) {
    if (error != 0)
      *error = (uint8_t) rpacket[PKT_ERROR];
  }

  return result;
}

int PacketManager::ping(PortManager *port, uint8_t id, uint8_t *error) {
  // Status: Done

  int result{COMM_TX_FAIL};

  // Create packet containers
  uint8_t pkt_tx[6]{};
  uint8_t pkt_rx[RX_PKT_MAX_LEN]{};

  pkt_tx[PKT_ID] = id;
  pkt_tx[PKT_LENGTH] = 2; // No data packets for ping
  pkt_tx[PKT_INSTRUCTION] = INST_PING;

  result = wrPacket(port, pkt_tx, pkt_rx, error);

  return result;
}

int PacketManager::save(bear::PortManager *port, uint8_t id, uint8_t *error) {
  // Status: Done
  (void) error;
  int result(COMM_TX_FAIL);

  // Create packet containers
  uint8_t pkt_tx[6]{};
//   uint8_t pkt_rx[RX_PKT_MAX_LEN]{};

  pkt_tx[PKT_ID] = id;
  pkt_tx[PKT_LENGTH] = 2; // No data packets for save
  pkt_tx[PKT_INSTRUCTION] = INST_SAVE_CONFIG;

  result = WritePacket(port, pkt_tx);
  port->in_use_ = false;

  return result;
}

/****************************************/
/***** INSTRUCTION PACKET STRUCTURE *****/
/*
 * [0xFF, 0xFF, MOTOR_ID, LENGTH, INSTRUCTION, DATA0, DATA1, ..., DATAN-1, CHKSUM]
 */
/****************************************/
int PacketManager::WriteRegisterTX(bear::PortManager *port, uint8_t id, uint16_t address, uint16_t length,
                                   uint8_t *data, const std::string &sc) {
  // Status: Testing

  int result{COMM_TX_FAIL};

  uint8_t
      *pkt_tx = (uint8_t *) malloc(length + 6); // [0xFF, 0xFF, ID, LENGTH, INST, ..., CHKSUM] is the 6 default bytes

  pkt_tx[PKT_ID] = id;
  pkt_tx[PKT_LENGTH] = length + 3; // [INST, PACKET_LENGTH, CHKSUM], where 3 is [INST, MOTOR_ID, CHKSUM]
  if (sc == "c")
    pkt_tx[PKT_INSTRUCTION] = INST_WRITE_CONFIG;
  else if (sc == "s")
    pkt_tx[PKT_INSTRUCTION] = INST_WRITE_STAT;
  pkt_tx[PKT_PARAMETER0] = (uint8_t) address;

  for (uint16_t s = 0; s < length; s++)
    pkt_tx[PKT_PARAMETER0 + s + 1] = data[s];

  result = WritePacket(port, pkt_tx);
  port->in_use_ = false;

  free(pkt_tx);

  return result;
}

int PacketManager::WriteRegisterTXRX(bear::PortManager *port, uint8_t id, uint16_t address, uint16_t length,
                                     uint8_t *data, uint8_t *error, const std::string &sc) {
  // Status: Testing

  int result{COMM_TX_FAIL};

  uint8_t *pkt_tx = (uint8_t *) malloc(length + 6);
  uint8_t pkt_rx[6]{};

  pkt_tx[PKT_ID] = id;
  pkt_tx[PKT_LENGTH] = length + 3;
  if (sc == "c")
    pkt_tx[PKT_INSTRUCTION] = INST_WRITE_CONFIG;
  else if (sc == "s")
    pkt_tx[PKT_INSTRUCTION] = INST_WRITE_STAT;
  pkt_tx[PKT_PARAMETER0] = (uint8_t) address;

  for (uint16_t s = 0; s < length; s++)
    pkt_tx[PKT_PARAMETER0 + s + 1] = data[s];

  result = wrPacket(port, pkt_tx, pkt_rx, error);

  free(pkt_tx);

  return result;
}

int PacketManager::WriteStatusRegister(bear::PortManager *port, uint8_t id, uint16_t address, uint32_t data,
                                       uint8_t *error) {
  /* Status: WIP
   *
   * - [ ] Create little-endian creating functionality
   */
//    if (address < 3)
(void) error;
  uint8_t data_packed[4] = {GEN_LOBYTE(GEN_LOWORD(data)), GEN_HIBYTE(GEN_LOWORD(data)), GEN_LOBYTE(GEN_HIWORD(data)),
                            GEN_HIBYTE(GEN_HIWORD(data))}; // TODO: Create little-endian creating functionality

//    else
//        uint8_t data_packed[4]
//    uint8_t data_packed[4];
//    data_packed[0] = (data >> 0) & 0xFF;
//    data_packed[1] = (data >> 8) & 0xFF;
//    data_packed[2] = (data >> 16) & 0xFF;
//    data_packed[3] = (data >> 24) & 0xFF;

//  return WriteRegisterTXRX(port, id, address, 4, data_packed, error, "s");
  return WriteRegisterTX(port, id, address, 4, data_packed, "s");
}

int PacketManager::WriteConfigRegister(bear::PortManager *port, uint8_t id, uint16_t address, uint32_t data,
                                       uint8_t *error) {
  /* Status: WIP
   *
   * - [ ] Create little-endian creating functionality
   */
  (void) error;
  uint8_t data_packed[4];
  data_packed[0] = (data >> 0) & 0xFF;
  data_packed[1] = (data >> 8) & 0xFF;
  data_packed[2] = (data >> 16) & 0xFF;
  data_packed[3] = (data >> 24) & 0xFF;

//    int result = WriteRegisterTXRX(port, id, address, 4, data_packed, error, "c");
  return WriteRegisterTX(port, id, address, 4, data_packed, "c");
}

int PacketManager::ReadRegisterTX(PortManager *port, uint8_t id, uint16_t address, uint16_t length,
                                  const std::string &sc) {
  // Status: Testing
  int result = COMM_TX_FAIL;

  uint8_t pkt_tx[11]{};

  pkt_tx[PKT_ID] = id;
  pkt_tx[PKT_LENGTH] = 4;
  if (sc == "c")
    pkt_tx[PKT_INSTRUCTION] = INST_READ_CONFIG;
  else if (sc == "s")
    pkt_tx[PKT_INSTRUCTION] = INST_READ_STAT;
  pkt_tx[PKT_PARAMETER0 + 0] = (uint8_t) address;
  pkt_tx[PKT_PARAMETER0 + 1] = (uint8_t) length;

  result = WritePacket(port, pkt_tx);

  // TODO: Set packet timeout

  return result;
}

int PacketManager::ReadRegisterRX(PortManager *port, uint8_t id, uint16_t length, uint8_t *data, uint8_t *error) {
  // Status: Testing

  int result = COMM_RX_FAIL;
  uint8_t *pkt_rx = (uint8_t *) malloc(RX_PKT_MAX_LEN);

  do {
    result = ReadPacket(port, pkt_rx);
  } while (result == COMM_SUCCESS && pkt_rx[PKT_ID] != id);

  if (result == COMM_SUCCESS && pkt_rx[PKT_ID] == id) {
    if (error != 0) {
      *error = (uint8_t) pkt_rx[PKT_ERROR];
    }
    for (uint16_t s = 0; s < length; s++) {
      data[s] = pkt_rx[PKT_PARAMETER0 + s];
    }
  }

  free(pkt_rx);

  return result;
}

int PacketManager::ReadRegisterTXRX(PortManager *port, uint8_t id, uint16_t address, uint16_t length, uint8_t *data,
                                    uint8_t *error, const std::string &sc) {
  // Status: Testing

  int result = COMM_TX_FAIL;

//  uint8_t pkt_tx[7]{};
  uint8_t *pkt_tx = (uint8_t *) malloc(length + 6);
  uint8_t *pkt_rx = (uint8_t *) malloc(RX_PKT_MAX_LEN);

  pkt_tx[PKT_ID] = id;
  pkt_tx[PKT_LENGTH] = (uint8_t) length;
  if (sc == "c")
    pkt_tx[PKT_INSTRUCTION] = INST_READ_CONFIG;
  else if (sc == "s")
    pkt_tx[PKT_INSTRUCTION] = INST_READ_STAT;
  pkt_tx[PKT_PARAMETER0 + 0] = (uint8_t) address;
//  pkt_tx[PKT_PARAMETER0 + 1] = *data;
//    pkt_tx[PKT_PARAMETER0+1] = (uint8_t) length;

  result = wrPacket(port, pkt_tx, pkt_rx, error);
  if (result == COMM_SUCCESS) {
    if (*error != 128) {
      *error = (uint8_t) pkt_rx[PKT_ERROR];
    }
    for (uint16_t s = 0; s < length + 1; s++) {
      data[s] = pkt_rx[PKT_PARAMETER0 + s];
    }
  }

  free(pkt_tx);
  free(pkt_rx);

  return result;
}

int PacketManager::ReadStatusRegister(bear::PortManager *port, uint8_t id, uint16_t address, uint32_t *data,
                                      uint8_t *error) {
  uint8_t data_packed[4]{};

  int result = PacketManager::ReadRegisterTXRX(port, id, address, 3, data_packed, error, "s");

  if (result == COMM_SUCCESS)
    *data = data_packed[0];

  return result;
}

int PacketManager::ReadStatusRegister(bear::PortManager *port, uint8_t id, uint16_t address, float *data,
                                      uint8_t *error) {
  uint8_t data_packed[4]{};

  int result = PacketManager::ReadRegisterTXRX(port, id, address, 3, data_packed, error, "s");

  if (result == COMM_SUCCESS)
    *data = *(float *) &data_packed;

  return result;
}

int PacketManager::ReadConfigRegister(bear::PortManager *port, uint8_t id, uint16_t address, uint32_t *data,
                                      uint8_t *error) {
  uint8_t data_packed[4]{};

  int result = PacketManager::ReadRegisterTXRX(port, id, address, 3, data_packed, error, "c");

  if (result == COMM_SUCCESS)
    *data = data_packed[0];

  return result;
}

int PacketManager::ReadConfigRegister(bear::PortManager *port, uint8_t id, uint16_t address, float *data,
                                      uint8_t *error) {
  uint8_t data_packed[4]{};

  int result = PacketManager::ReadRegisterTXRX(port, id, address, 3, data_packed, error, "c");

  if (result == COMM_SUCCESS)
    *data = *(float *) &data_packed;

  return result;
}

int PacketManager::BulkCommunication(PortManager *port,
                                     std::vector<uint8_t> mIDs,
                                     std::vector<uint8_t> addr_read,
                                     std::vector<uint8_t> addr_write,
                                     std::vector<std::vector<uint32_t>> data_write,
                                     std::vector<std::vector<float>> &ret_vec,
                                     uint8_t *error) {
  (void) error;
  int result{COMM_TX_FAIL};

  uint8_t checksum = 0;

  uint8_t num_motors = mIDs.size();
  uint8_t num_read_regs = addr_read.size();
  uint8_t num_write_regs = addr_write.size();
  uint8_t num_total_regs = num_write_regs | num_read_regs << 4;

  uint8_t pkt_length = 3 + num_read_regs + num_write_regs + num_motors + 4 * num_motors * num_write_regs + 1;

  std::vector<std::vector<uint8_t>> data_write_hex_all;
  std::vector<uint8_t> data_write_hex_single;
  for (uint8_t odx = 0; odx < data_write.size(); ++odx) {
    for (uint8_t idx = 0; idx < data_write[odx].size(); ++idx) {
      uint8_t data_i[4] = {GEN_LOBYTE(GEN_LOWORD(data_write[odx][idx])),
                           GEN_HIBYTE(GEN_LOWORD(data_write[odx][idx])),
                           GEN_LOBYTE(GEN_HIWORD(data_write[odx][idx])),
                           GEN_HIBYTE(GEN_HIWORD(data_write[odx][idx]))};

      data_write_hex_single.push_back(data_i[0]);
      data_write_hex_single.push_back(data_i[1]);
      data_write_hex_single.push_back(data_i[2]);
      data_write_hex_single.push_back(data_i[3]);
    }
    data_write_hex_all.push_back(data_write_hex_single);
    data_write_hex_single.erase(data_write_hex_single.begin(), data_write_hex_single.end());
  }

  // create packet containers
  uint8_t *pkt_tx = (uint8_t *) malloc(pkt_length + 4);

  uint8_t sum_addr_read = std::accumulate(addr_read.begin(), addr_read.end(), 0);
  uint8_t sum_addr_write = std::accumulate(addr_write.begin(), addr_write.end(), 0);
//  uint8_t sum_data = std::accumulate(data_write_hex.begin(), data_write_hex.end(), 0);
  uint8_t sum_data =
      std::accumulate(data_write_hex_all.cbegin(), data_write_hex_all.cend(), 0, [](auto lhs, const auto &rhs) {
        return std::accumulate(rhs.cbegin(), rhs.cend(), lhs);
      });
  uint8_t sum_mIDs = std::accumulate(mIDs.begin(), mIDs.end(), 0);

  // Check if there are data to write
  if (num_write_regs == 0) {
    std::vector<uint8_t> data_pkt{num_motors, num_total_regs, sum_addr_read, sum_mIDs};
    checksum = GenerateChecksum(0xFE, pkt_length, INST_BULK_COMM, data_pkt);
    GenerateBulkPacket(pkt_tx,
                       mIDs,
                       pkt_length,
                       num_motors,
                       num_total_regs,
                       addr_read,
                       addr_write,
                       data_write_hex_all,
                       checksum);
  } else {
    std::vector<uint8_t> data_pkt{num_motors, num_total_regs, sum_addr_read, sum_addr_write, sum_data, sum_mIDs};
    checksum = GenerateChecksum(0xFE, pkt_length, INST_BULK_COMM, data_pkt);
    GenerateBulkPacket(pkt_tx,
                       mIDs,
                       pkt_length,
                       num_motors,
                       num_total_regs,
                       addr_read,
                       addr_write,
                       data_write_hex_all,
                       checksum);
  }

//  // Print write packet
//  std::cout << "TX Packet: " << std::endl;
//  for (int a = 0; a < pkt_length + 4; a++)
//    std::cout << int(pkt_tx[a]) << " ";
//  std::cout << std::endl;

  // Check if there are data to read after writing the packet
  if (num_read_regs == 0) {
    result = WritePacket(port, pkt_tx);
    port->in_use_ = false;
    free(pkt_tx);
    return result;
  } else { // custom R/W
    uint8_t *pkt_rx;
    int mdx = 0;
    while (mdx < MAX_COMM_ATTEMPT) {
      port->ClearPort();
//      port->ClearIOPort();
      pkt_rx = (uint8_t *) malloc(RX_PKT_MAX_LEN);
      result = WritePacket(port, pkt_tx);
      std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
      result = ReadBulkPacket(port, num_motors, pkt_rx);
      if (result == COMM_SUCCESS)
        break;
      free(pkt_rx);
      ++mdx;
      if (debug_mode_)
        std::cout << "[ ! ] Trying again!" << std::endl;
    }
    if (mdx == MAX_COMM_ATTEMPT) {
      result = COMM_RX_FAIL;
      free(pkt_tx);
      return result;
    }

    uint8_t len_ret_pkt = pkt_rx[PKT_LENGTH] + 4;

//    // Print return packet
//    std::cout << "RX Packet: " << std::endl;
//    for (int a = 0; a < len_ret_pkt * num_motors; ++a)
//      std::cout << int(pkt_rx[a]) << " ";
//    std::cout << std::endl;

    std::vector<float> ret_single;
    *error = 128;
    for (uint8_t ii = 0; ii < num_motors; ii++) {
      uint8_t m_offset = ii * len_ret_pkt;

      static constexpr int warning_mask = 1 | 2;
      static constexpr int error_mask = 255 ^ warning_mask;

      ret_single.push_back(pkt_rx[m_offset + PKT_ID]);
      uint8_t err_single = pkt_rx[m_offset + PKT_ERROR];
      if (err_single != 128) { // if not normal, cut the list short
        // ret_single.push_back(err_single);
        *error = err_single;
        if (debug_mode_)
            std::cout << "Error for motor " << int(pkt_rx[m_offset + PKT_ID]) << " in the return packet: " << int(err_single-128) << std::endl;
        // continue;
      }

      for (uint8_t jj = 0; jj < num_read_regs; jj++) {
        uint8_t data_raw[4]{};

        for (uint16_t s = 0; s < 4; s++) {
          data_raw[s] = pkt_rx[m_offset + PKT_PARAMETER0 + 4 * jj + s];
        }

        float *data;
        data = (float *) &data_raw;
        ret_single.push_back(*data);
      }
      ret_single.push_back(err_single);

      ret_vec.push_back(ret_single);
      ret_single.erase(ret_single.begin(), ret_single.end());
    }

    free(pkt_tx);
    free(pkt_rx);
    return result;
  }
}

void PacketManager::GenerateBulkPacket(uint8_t *wpacket,
                                       std::vector<uint8_t> &mIDs,
                                       uint8_t &pkt_len,
                                       uint8_t &num_motors,
                                       uint8_t &num_total_regs,
                                       std::vector<uint8_t> &addr_read,
                                       std::vector<uint8_t> &addr_write,
                                       std::vector<std::vector<uint8_t>> &data,
                                       uint8_t checksum) {
  wpacket[PKT_HEADER0] = 0xFF;
  wpacket[PKT_HEADER1] = 0xFF;
  wpacket[PKT_ID] = 0xFE;
  wpacket[PKT_LENGTH] = pkt_len;
  wpacket[PKT_INSTRUCTION] = INST_BULK_COMM;
  wpacket[PKT_BULK_N_MOTORS] = num_motors;
  wpacket[PKT_BULK_N_STAT_RW] = num_total_regs;
  wpacket[4 + pkt_len - 1] = checksum;

  int idx = 0;
  for (auto const &it : addr_read) {
    wpacket[PKT_BULK_READ_REG_0 + idx] = it;
    idx++;
  }

  if (!addr_write.empty()) { // when there is something to write
    int pkt_write_0 = PKT_BULK_READ_REG_0 + addr_read.size();
    int jdx = 0;
    for (auto const &it : addr_write) {
      wpacket[pkt_write_0 + jdx] = it;
      jdx++;
    }

    int pkt_motdata_0 = pkt_write_0 + addr_write.size();
    for (uint8_t kdx = 0; kdx < mIDs.size(); ++kdx) {
      uint8_t kkdx = kdx * (data[kdx].size() + 1);
      wpacket[pkt_motdata_0 + kkdx] = mIDs[kdx];

      for (uint8_t ddx = 0; ddx < data[kdx].size(); ++ddx) {
        wpacket[pkt_motdata_0 + kkdx + ddx + 1] = data[kdx][ddx];
      }
    }
  } else {
    int pkt_mot_0 = PKT_BULK_READ_REG_0 + addr_read.size();
    int mdx = 0;
    for (auto const &it : mIDs) {
      wpacket[pkt_mot_0 + mdx] = it;
      mdx++;
    }
  }
}

uint8_t PacketManager::GenerateChecksum(uint8_t mID,
                                        uint8_t pkt_len,
                                        uint8_t instruction,
                                        const std::vector<uint8_t> &addr_vec) {
  uint8_t checksum = 0;
  checksum = mID + pkt_len + instruction;
  for (auto const &adx : addr_vec) {
    checksum += adx;
  }
//  checksum = mID + pkt_len + instruction + std::accumulate(addr_vec.begin(), addr_vec.end(), 0); // TODO: Compare timing
  return ~checksum;
}