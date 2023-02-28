//
// Created by msahn on 3/11/21.
//

#include <string>
#include "bear_sdk.h"

// Configuration Registers
#define CR_ID                       0
#define CR_MODE                     1
#define CR_BAUDRATE                 2
#define CR_HOMING_OFFSET            3
#define CR_P_GAIN_ID                4
#define CR_I_GAIN_ID                5
#define CR_D_GAIN_ID                6
#define CR_P_GAIN_IQ                7
#define CR_I_GAIN_IQ                8
#define CR_D_GAIN_IQ                9
#define CR_P_GAIN_VEL               10
#define CR_I_GAIN_VEL               11
#define CR_D_GAIN_VEL               12
#define CR_P_GAIN_POS               13
#define CR_I_GAIN_POS               14
#define CR_D_GAIN_POS               15
#define CR_P_GAIN_FORCE             16
#define CR_I_GAIN_FORCE             17
#define CR_D_GAIN_FORCE             18
#define CR_LIMIT_ID_MAX             19
#define CR_LIMIT_IQ_MAX             20
#define CR_LIMIT_VEL_MAX            21
#define CR_LIMIT_POS_MIN            22
#define CR_LIMIT_POS_MAX            23
#define CR_MIN_VOLTAGE              24
#define CR_MAX_VOLTAGE              25
#define CR_WATCHDOG_TIMEOUT         26
#define CR_TEMP_LIMIT_LOW           27
#define CR_TEMP_LIMIT_HIGH          28

// Status Registers
#define SR_TORQUE_ENABLE            0
#define SR_HOMING_COMPLETE          1
#define SR_GOAL_ID                  2
#define SR_GOAL_IQ                  3
#define SR_GOAL_VELOCITY            4
#define SR_GOAL_POSITION            5
#define SR_PRESENT_ID               6
#define SR_PRESENT_IQ               7
#define SR_PRESENT_VELOCITY         8
#define SR_PRESENT_POSITION         9
#define SR_INPUT_VOLTAGE            10
#define SR_WINDING_TEMPERATURE      11
#define SR_POWERSTAGE_TEMPERATURE   12
#define SR_IC_TEMPERATURE           13
#define SR_ERROR_STATUS             14
#define SR_PACKED_IQ_TEMP           15

// Return Error Codes
#define RET_ERR                     999

using namespace bear;

BEAR::BEAR(const char *portName, int baudrate, const bool autoconnect, const bool debug)
    : portName_{portName},
      debug_mode_(debug),
      baudrate_{baudrate},
      packetManager_{bear::PacketManager(debug)},
      portManager_{bear::PortManager(portName_, baudrate, debug)} {
  if (autoconnect)
    Connect();
}

bool BEAR::Connect() {
  if (portManager_.OpenPort()) {
    if (debug_mode_)
    {
        printf("Success! Port opened!\n");
        printf(" - Device Name: %s\n", portName_);
        printf(" - Baudrate: %d\n\n", portManager_.GetBaudRate());
    }
    return true;
  } else {
    if (debug_mode_)
    {
        printf("Failed to open port! [%s]\n", portName_);
    }
  }
  return false;
}

void BEAR::Disconnect() {
    portManager_.ClosePort();
}

bool BEAR::IsConnected() {
    return portManager_.IsPortOpen();
}

uint8_t BEAR::GetErrorCode() {
  return bear_error;
}

int BEAR::ping(uint8_t mID) {
  return (packetManager_.ping(&portManager_, mID, &bear_error) == COMM_SUCCESS);
}

int BEAR::save(uint8_t mID) {
  return (packetManager_.save(&portManager_, mID, &bear_error) == COMM_SUCCESS);
}

/* ******************************************** *
 * Getters/Setters for configuration registers. *
 * ******************************************** */
uint32_t BEAR::GetID(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_ID, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetID(uint8_t mID, uint32_t val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_ID, val, &bear_error) == COMM_SUCCESS);
}

uint32_t BEAR::GetMode(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_MODE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetMode(uint8_t mID, uint32_t val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_MODE, val, &bear_error) == COMM_SUCCESS);
}

uint32_t BEAR::GetBaudrate(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_BAUDRATE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetBaudrate(uint8_t mID, uint32_t val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_BAUDRATE, val, &bear_error) == COMM_SUCCESS);
}

float BEAR::GetHomingOffset(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_HOMING_OFFSET, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetHomingOffset(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_HOMING_OFFSET, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetPGainId(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_P_GAIN_ID, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetPGainId(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_P_GAIN_ID, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetIGainId(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_I_GAIN_ID, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetIGainId(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_I_GAIN_ID, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetDGainId(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_D_GAIN_ID, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetDGainId(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_D_GAIN_ID, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetPGainIq(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_P_GAIN_IQ, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetPGainIq(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_P_GAIN_IQ, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetIGainIq(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_I_GAIN_IQ, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetIGainIq(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_I_GAIN_IQ, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetDGainIq(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_D_GAIN_IQ, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetDGainIq(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_D_GAIN_IQ, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetPGainVelocity(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_P_GAIN_VEL, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetPGainVelocity(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_P_GAIN_VEL, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetIGainVelocity(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_I_GAIN_VEL, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetIGainVelocity(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_I_GAIN_VEL, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetDGainVelocity(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_D_GAIN_VEL, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetDGainVelocity(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_D_GAIN_VEL, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetPGainPosition(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_P_GAIN_POS, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetPGainPosition(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_P_GAIN_POS, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetIGainPosition(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_I_GAIN_POS, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetIGainPosition(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_I_GAIN_POS, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetDGainPosition(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_D_GAIN_POS, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetDGainPosition(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_D_GAIN_POS, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetPGainDirectForce(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_P_GAIN_FORCE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetPGainDirectForce(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_P_GAIN_FORCE, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetIGainDirectForce(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_I_GAIN_FORCE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetIGainDirectForce(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_I_GAIN_FORCE, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetDGainDirectForce(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_D_GAIN_FORCE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetDGainDirectForce(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_D_GAIN_FORCE, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetLimitAccMax(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_LIMIT_ID_MAX, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetLimitAccMax(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_LIMIT_ID_MAX, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetLimitIMax(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_LIMIT_IQ_MAX, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetLimitIMax(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_LIMIT_IQ_MAX, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetVelocityMax(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_LIMIT_VEL_MAX, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetVelocityMax(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_LIMIT_VEL_MAX, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetLimitPositionMin(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_LIMIT_POS_MIN, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetLimitPositionMin(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_LIMIT_POS_MIN, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetLimitPositionMax(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_LIMIT_POS_MAX, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetLimitPositionMax(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_LIMIT_POS_MAX, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetMinVoltage(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_MIN_VOLTAGE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetMinVoltage(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_MIN_VOLTAGE, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetMaxVoltage(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_MAX_VOLTAGE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetMaxVoltage(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_MAX_VOLTAGE, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

uint32_t BEAR::GetWatchdogTimeout(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_WATCHDOG_TIMEOUT, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetWatchdogTimeout(uint8_t mID, uint32_t val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_WATCHDOG_TIMEOUT, val, &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetTemperatureLimitLow(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_TEMP_LIMIT_LOW, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetTemperatureLimitLow(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_TEMP_LIMIT_LOW, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetTemperatureLimitHigh(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadConfigRegister(&portManager_, mID, CR_TEMP_LIMIT_HIGH, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetTemperatureLimitHigh(uint8_t mID, float val) {
  return (packetManager_.WriteConfigRegister(&portManager_, mID, CR_TEMP_LIMIT_HIGH, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

/* ************************************* *
 * Getters/Setters for status registers. *
 * ************************************* */
uint32_t BEAR::GetTorqueEnable(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_TORQUE_ENABLE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetTorqueEnable(uint8_t mID, uint32_t val) {
  return (packetManager_.WriteStatusRegister(&portManager_, mID, SR_TORQUE_ENABLE, val, &bear_error) == COMM_SUCCESS);
}

float BEAR::GetGoalId(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_GOAL_ID, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetGoalId(uint8_t mID, float val) {
  return (packetManager_.WriteStatusRegister(&portManager_, mID, SR_GOAL_ID, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetGoalIq(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_GOAL_IQ, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetGoalIq(uint8_t mID, float val) {
  return (packetManager_.WriteStatusRegister(&portManager_, mID, SR_GOAL_IQ, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetGoalVelocity(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_GOAL_VELOCITY, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetGoalVelocity(uint8_t mID, float val) {
  return (packetManager_.WriteStatusRegister(&portManager_, mID, SR_GOAL_VELOCITY, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetGoalPosition(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_GOAL_POSITION, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

bool BEAR::SetGoalPosition(uint8_t mID, float val) {
  return (packetManager_.WriteStatusRegister(&portManager_, mID, SR_GOAL_POSITION, floatToUint32(val), &bear_error)
      == COMM_SUCCESS);
}

float BEAR::GetPresentId(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_PRESENT_ID, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetPresentIq(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_PRESENT_IQ, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetPresentVelocity(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_PRESENT_VELOCITY, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetPresentPosition(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_PRESENT_POSITION, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetInputVoltage(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_INPUT_VOLTAGE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetWindingTemperature(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_WINDING_TEMPERATURE, &retVal, &bear_error)
      != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetPowerstageTemperature(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_POWERSTAGE_TEMPERATURE, &retVal, &bear_error)
      != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

float BEAR::GetICTemperature(uint8_t mID) {
  float retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_IC_TEMPERATURE, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

uint32_t BEAR::GetErrorStatus(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_ERROR_STATUS, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

uint32_t BEAR::GetPackedIqAndTemps(uint8_t mID) {
  uint32_t retVal;
  if (packetManager_.ReadStatusRegister(&portManager_, mID, SR_PACKED_IQ_TEMP, &retVal, &bear_error) != COMM_SUCCESS)
    retVal = RET_ERR;
  return retVal;
}

std::vector<std::vector<float>> BEAR::BulkRead(std::vector<uint8_t> mIDs, std::vector<uint8_t> read_add) {
  std::vector<std::vector<float>> ret_vec;
  std::vector<uint8_t> empty_vec;
  std::vector<std::vector<uint32_t>> empty_vec_uint32;
  packetManager_.BulkCommunication(&portManager_, mIDs, read_add, empty_vec, empty_vec_uint32, ret_vec, &bear_error);
  return ret_vec;
}

bool BEAR::BulkWrite(std::vector<uint8_t> mIDs,
                     std::vector<uint8_t> write_add,
                     const std::vector<std::vector<float>> &data) {
  std::vector<std::vector<float>> empty_ret_vec;
  std::vector<uint8_t> empty_uint;
  std::vector<std::vector<uint32_t>> data_uint32;
  std::vector<uint32_t> data_uint32_i;
  for (auto const &outer : data) {
    for (auto const &inner : outer) {
      data_uint32_i.push_back(floatToUint32(inner));
    }
    data_uint32.push_back(data_uint32_i);
    data_uint32_i.erase(data_uint32_i.begin(), data_uint32_i.end());
  }
  if (packetManager_.BulkCommunication(&portManager_,
                                       mIDs,
                                       empty_uint,
                                       write_add,
                                       data_uint32,
                                       empty_ret_vec,
                                       &bear_error) == COMM_SUCCESS)
    return true;
  return false;
}

std::vector<std::vector<float>> BEAR::BulkReadWrite(std::vector<uint8_t> mIDs,
                                                    std::vector<uint8_t> read_add,
                                                    std::vector<uint8_t> write_add,
                                                    std::vector<std::vector<float>> data) {
  std::vector<std::vector<float>> ret_vec;

  std::vector<std::vector<uint32_t>> data_uint32;
  std::vector<uint32_t> data_uint32_i;
  for (auto const &outer : data) {
    for (auto const &inner : outer) {
      data_uint32_i.push_back(floatToUint32(inner));
    }
    data_uint32.push_back(data_uint32_i);
    data_uint32_i.erase(data_uint32_i.begin(), data_uint32_i.end());
  }

  packetManager_.BulkCommunication(&portManager_,
                                   mIDs,
                                   read_add,
                                   write_add,
                                   data_uint32,
                                   ret_vec,
                                   &bear_error);
  return ret_vec;
}

/* ****************** *
 * Utility functions. *
 * ****************** */
uint32_t BEAR::floatToUint32(float input) {
  static_assert(sizeof(float) == sizeof(uint32_t), "Float and uint32 are not the same size in your setup!");
  auto *pInt = reinterpret_cast<uint32_t *>(&input);
  return *pInt;
}