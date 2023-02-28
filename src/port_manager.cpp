#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <linux/serial.h>

#include "port_manager.h"

using namespace bear;

PortManager::PortManager(const char *port_name, const int baudrate, const bool debug)
    : socket_fd{-1},
      baudrate_{baudrate},
      debug_mode_(debug),
      packet_start_time{0.0},
      packet_timeout{0.0},
      tx_time_per_byte{0.0} {
  in_use_ = false;
  SetPortName(port_name);
}

bool PortManager::OpenPort() {
  return SetBaudRate(baudrate_);
}

void PortManager::ClosePort() {
  if (socket_fd != -1)
    close(socket_fd);
  socket_fd = -1;
}

bool PortManager::IsPortOpen()
{
    return socket_fd != -1;
}

void PortManager::ClearPort() {
  tcflush(socket_fd, TCIFLUSH);
}

void PortManager::ClearIOPort() {
  tcflush(socket_fd, TCIOFLUSH);
}

void PortManager::SetPortName(const char *port_name) {
  strcpy(port_name_, port_name);
}

char *PortManager::GetPortName() {
  return port_name_;
}

bool PortManager::SetBaudRate(const int baudrate) {
  int baud = GetIoctlBaud(baudrate);

  ClosePort();

  baudrate_ = baudrate;

  if (baud <= 0) // If PortManager::GetIoctlBaud switch case does not exist
  {
    SetupPort(B38400);
    return SetCustomBaudrate(baudrate);
  } else {
    return SetupPort(baud);
  }
}

int PortManager::GetBaudRate() {
  return baudrate_;
}

int PortManager::GetBytesAvailable() {
  int bytes_available;
  ioctl(socket_fd, FIONREAD, &bytes_available); // TODO: Check with select(2)
  return bytes_available;
}

int PortManager::ReadPort(uint8_t *packet, int length) {
  return read(socket_fd, packet, length);
}

int PortManager::WritePort(uint8_t *packet, int length) {
  return write(socket_fd, packet, length); // Returns the number of bytes written
}

bool PortManager::SetupPort(const int cflag_baud) {
  struct termios tioNew;

  socket_fd = open(port_name_, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (socket_fd < 0) {
    if (debug_mode_)
        printf("[ CBEAR ] <PortManager::SetupPort> Error opening serial port!\n");
    return false;
  }
  bzero(&tioNew, sizeof(tioNew)); // Clear the struct for new port settings

  tioNew.c_cflag = cflag_baud | CS8 | CLOCAL | CREAD;
  tioNew.c_iflag = IGNPAR;
  tioNew.c_oflag = 0;
  tioNew.c_lflag = 0;
  tioNew.c_cc[VTIME] = 0;
  tioNew.c_cc[VMIN] = 0;

  tcflush(socket_fd, TCIFLUSH);
  tcsetattr(socket_fd, TCSANOW, &tioNew);

  tx_time_per_byte = (1000.0 / (double) baudrate_) * 10.0;

  return true;
}

bool PortManager::SetCustomBaudrate(int speed) {
  struct serial_struct ss;
  if (ioctl(socket_fd, TIOCGSERIAL, &ss) != 0) {
    if (debug_mode_)
        printf("[ CBEAR ] <PortManager::SetCustomBaudrate> TIOCGSERIAL failed!\n");
    return false;
  }

  ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
  ss.custom_divisor = (ss.baud_base + (speed / 2)) / speed;
  int closest_speed = ss.baud_base / ss.custom_divisor;

  if (closest_speed < speed * 98 / 100 || closest_speed > speed * 102 / 100) {
    if (debug_mode_)
        printf("[ CBEAR ] <PortManager::SetCustomBaudrate> Cannot set speed to %d, closest is %d.\n", speed, closest_speed);
    return false;
  }

  if (ioctl(socket_fd, TIOCSSERIAL, &ss) < 0) {
    if (debug_mode_)
        printf("[ CBEAR ] <PortManager::SetCustomBaudrate> TIOCSSERIAL failed.\n");
  }

  tx_time_per_byte = (1000.0 / (double) speed) * 10.0;
  return true;
}

int PortManager::GetIoctlBaud(int baudrate) {
  switch (baudrate) {
    case 3000000:return B3000000;
    case 4000000:return B4000000;
    default:return -1;
  }
}
