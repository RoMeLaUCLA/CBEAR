//
// Created by msahn on 3/11/21.
//

#ifndef CBEAR_INCLUDE_PORT_MANAGER_H_
#define CBEAR_INCLUDE_PORT_MANAGER_H_

#include <cstdint>

namespace bear {

class PortManager {
 public:
  bool in_use_; // Boolean to show if the port is in use or not.

//    static const int DEFAULT_BAUDRATE = 57600; //< Default baudrate.
  static const int DEFAULT_BAUDRATE = 8000000; //< Default baudrate.

  /*! \brief Initialize the PortManager.
   *
   * Initialize the PortManager.
   */
  PortManager(const char *port_name, const int baudrate);

  /*! \brief Destruct the PortManager.
   *
   * Destruct the PortManager.
   */
  virtual ~PortManager() {
    ClosePort();
  }

  /*! \brief Open the port and return communication results.
   *
   * Open the port and return communication results.
   */
  bool OpenPort();

  /*! \brief Close the port.
   *
   * Close the port
   */
  void ClosePort();

  /*! \brief Clear the port.
   *
   * Clear the port
   */
  void ClearPort();

  /*! \brief Clear input and output port.
   *
   * Clear input and output port.
   */
  void ClearIOPort();

  /*! \brief Set port name.
   *
   * Set port name.
   */
  void SetPortName(const char *port_name);

  /*! \brief Get port name.
   *
   * Get port nam.e
   */
  char *GetPortName();

  /*! \brief Set the baud rate of the port.
   *
   * Set the baudrate of the port.
   */
  bool SetBaudRate(const int baudrate);

  /*! \brief Get baud rate that the port is set at.
   *
   * Get baud rate that the port is set at.
   */
  int GetBaudRate();

  /*! \brief Get available bytes ready to be read from the port.
   *
   * Get available bytes ready to be read from the port.
   *
   * @return The length of the bytes ready to be read from the buffer.
   */
  int GetBytesAvailable();

  /*! \brief Read bytes from the buffer.
   *
   * Read bytes from the buffer.
   *
   * @return Length of bytes read.
   */
  int ReadPort(uint8_t *packet, int length);

  /*! \brief Write bytes to port
   *
   * Write bytes to port.
   *
   * @return Length of bytes written.
   */
  int WritePort(uint8_t *packet, int length);

 private:
  int socket_fd;
  int baudrate_;
  char port_name_[100];

  double packet_start_time;
  double packet_timeout;
  double tx_time_per_byte;

  bool SetupPort(const int cflag_baud);
  bool SetCustomBaudrate(int speed);
  int GetIoctlBaud(const int baudrate);

  double GetCurrentTime();
  double GetTimeSinceStart();
}; // class PortManager

} // namespace bear

#endif //CBEAR_INCLUDE_PORT_MANAGER_H_
