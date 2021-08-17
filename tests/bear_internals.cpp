//
// Created by msahn on 3/11/21.
//
#include "doctest.h"
#include "port_manager.h"
#include "packet_manager.h"

#include <iostream>

// create dummy port
bear::PortManager d_portManager = bear::PortManager("dev/dummy", 1);

TEST_CASE("check port manager use guard")
{
  CHECK(d_portManager.in_use_ == false);
  d_portManager.in_use_ = true;
  CHECK(d_portManager.in_use_ == true);
}

// create dummy packet manager
bear::PacketManager d_packetManager = bear::PacketManager();

// write packet
TEST_CASE("testing buildPacket constructing correct packets")
{
  d_portManager.in_use_ = false;
  uint8_t pkt_tx[6]{};
  d_packetManager.BuildPacket(pkt_tx);

  CHECK(pkt_tx[0] == 255);
  CHECK(pkt_tx[1] == 255);
  CHECK(pkt_tx[2] == 0);
  CHECK(pkt_tx[3] == 255);
}

TEST_CASE("ping write packet test")
{
  d_portManager.in_use_ = false;
  uint8_t pkt_tx[6]{};
  pkt_tx[2] = 1;
  pkt_tx[3] = 2;
  pkt_tx[4] = INST_PING;
  d_packetManager.BuildPacket(pkt_tx);

  CHECK(pkt_tx[0] == 255);
  CHECK(pkt_tx[1] == 255);
  CHECK(pkt_tx[2] == 1);
  CHECK(pkt_tx[3] == 2);
  CHECK(pkt_tx[4] == 1);
  CHECK(pkt_tx[5] == 251);
}