//
// Created by msahn on 3/11/21.
//

#ifndef CBEAR_INCLUDE_PACKET_MANAGER_H_
#define CBEAR_INCLUDE_PACKET_MANAGER_H_

#include <string>
#include <vector>
#include "port_manager.h"

// Communication results
#define COMM_SUCCESS 0
#define COMM_PORT_BUSY -1000
#define COMM_TX_FAIL -1001
#define COMM_RX_FAIL -1002
#define COMM_RX_CORRUPT -1003

#define MAX_COMM_ATTEMPT 5

#define INST_PING 0x01
#define INST_READ_STAT 0x02
#define INST_WRITE_STAT 0x03
#define INST_READ_CONFIG 0x04
#define INST_WRITE_CONFIG 0x05
#define INST_SAVE_CONFIG 0x06
#define INST_BULK_COMM 0x12
#define COMM_TX_ERROR -2000

namespace bear {
class PacketManager {
 private:
  static PacketManager *unique_instance_;
  const bool debug_mode_;
 public:
  PacketManager(const bool debug=true);

  /*! \brief Get PacketManager instance.
     *
     * Get PacketManager instance.
     */
  static PacketManager *GetPacketManager() { return unique_instance_; }

  /*! \brief Destruct the PacketManager.
   *
   * Destruct the PacketManager.
   */
  virtual ~PacketManager() {}

  /*! \brief Builds the packet that will be written, including the checksum.
   *
   * @param packet Packet to be transmitted.
   */
  void BuildPacket(uint8_t *packet);

  /*! \brief Write instruction packet via the PortManager port.
     *
     * After clearing the port via PortManager::clearPort(), the packet is transmitted
     * via PortManager::writePort().
     * @param port PortManager instance.
     * @param packet Packet to be transmitted.
     * @return Result of writing the packet.
     */
  int WritePacket(PortManager *port, uint8_t *packet);

  /*! \brief Receive the packet from the PortManager port.
   *
   * Receive the packet from the PortManager port.
   */
  int ReadPacket(PortManager *port, uint8_t *packet);

  int ReadBulkPacket(PortManager *port, uint8_t num_motors, uint8_t *packet);

  /*! \brief Write and then read the packet from PortManager.
   *
   * Write and then read the packet from PortManager using a combination of
   * PacketManager::writePacket() and PacketManager::readPacket().
   */
  int wrPacket(PortManager *port, uint8_t *wpacket, uint8_t *rpacket, uint8_t *error);

  int wrBulkPacket(PortManager *port, uint8_t *wpacket, uint8_t *rpacket, uint8_t *error);

  /*! \brief Ping BEAR.
   *
   * Ping BEAR.
   */
  int ping(PortManager *port, uint8_t id, uint8_t *error);

  /*!
   *
   * @param port PortManager instance
   * @param id Motor ID
   * @param error
   * @return
   */
  int save(PortManager *port, uint8_t id, uint8_t *error);

  /*! \brief Write data to a register.
     *
     * Write data to a register.
     *
     * @param port PortManager instance
     * @param id Motor ID
     * @param address Register address
     * @param length Length of data (not the entire packet)
     * @param data Data to write to register
     * @param sc Type of register "s" for status, "c" for config
     * @return
     */
  int WriteRegisterTX(PortManager *port,
                      uint8_t id,
                      uint16_t address,
                      uint16_t length,
                      uint8_t *data,
                      const std::string &sc = "s");

  int WriteRegisterTXRX(PortManager *port,
                        uint8_t id,
                        uint16_t address,
                        uint16_t length,
                        uint8_t *data,
                        uint8_t *error = 0,
                        const std::string &sc = "s");

  int WriteStatusRegister(PortManager *port, uint8_t id, uint16_t address, uint32_t data, uint8_t *error);

  int WriteConfigRegister(PortManager *port, uint8_t id, uint16_t address, uint32_t data, uint8_t *error);

  /*! \brief Initially create the container for status/config packet and transmitted with PacketManager::writePacket().
   *
   * Initially create the container for status/config packet and transmitted with PacketManager::writePacket().
   */
  int ReadRegisterTX(PortManager *port, uint8_t id, uint16_t address, uint16_t length, const std::string &sc = "s");

  /*! \brief Receive the packet and parse the data in the packet.
   *
   * Receive the packet and parse the data in the packet.
   */
  int ReadRegisterRX(PortManager *port, uint8_t id, uint16_t length, uint8_t *data, uint8_t *error = 0);

  /*! \brief Transmits the instruction packet and reads the data from the received packet.
   *
   * Transmits the instruction packet and reads the data from the received packet.
   */
  int ReadRegisterTXRX(PortManager *port, uint8_t id, uint16_t address, uint16_t length, uint8_t *data,
                       uint8_t *error = 0,
                       const std::string &sc = "s");

  int ReadStatusRegister(PortManager *port, uint8_t id, uint16_t address, uint32_t *data, uint8_t *error = 0);

  int ReadStatusRegister(PortManager *port, uint8_t id, uint16_t address, float *data, uint8_t *error = 0);

  int ReadConfigRegister(PortManager *port, uint8_t id, uint16_t address, uint32_t *data, uint8_t *error = 0);

  int ReadConfigRegister(PortManager *port, uint8_t id, uint16_t address, float *data, uint8_t *error = 0);

  int ReadBulkData(PortManager *port);

  /*! \brief Writing to / reading from multiple status registers of one or more BAER actuators in a single communication frame.
   *
   * @param port PortManager
   * @param mIDs Vector of motor IDs
   * @param addr_read Vector of registers to read from
   * @param addr_write Vector of registers to write to
   * @param data_write Vector of data to write
   * @param ret_vec Vector of values to be returned through
   * @return
   */
  int BulkCommunication(PortManager *port,
                        std::vector<uint8_t> mIDs,
                        std::vector<uint8_t> addr_read,
                        std::vector<uint8_t> addr_write,
                        std::vector<std::vector<uint32_t>> data_write,
                        std::vector<std::vector<float>> &ret_vec,
                        uint8_t *error);

  float HexToFloat32(uint8_t *val);

  /*! \brief Generate a packet specific for bulk read and write.
   *
   * @param wpacket Packet that will be built to write
   * @param mIDs Vector of motor IDs
   * @param pkt_len Length of the packet
   * @param num_motors Number of motors
   * @param num_total_regs Total number of registers to read/write
   * @param addr_read Vector of addresses to read from
   * @param addr_write Vector of addresses to write to
   * @param data Vector of data to write
   * @param checksum Checksum
   */
  void GenerateBulkPacket(uint8_t *wpacket,
                          std::vector<uint8_t> &mIDs,
                          uint8_t &pkt_len,
                          uint8_t &num_motors,
                          uint8_t &num_total_regs,
                          std::vector<uint8_t> &addr_read,
                          std::vector<uint8_t> &addr_write,
                          std::vector<std::vector<uint8_t>> &data,
                          uint8_t checksum);

  uint8_t GenerateChecksum(uint8_t mID, uint8_t pkt_len, uint8_t instruction, const std::vector<uint8_t>& addr_vec);
}; // class PacketManager
} // namespace bear

#endif //CBEAR_INCLUDE_PACKET_MANAGER_H_
