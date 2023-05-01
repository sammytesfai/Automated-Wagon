/*!
 * @file DFRobotIRPosition.cpp
 * @brief DFRobot's Positioning ir camera
 * @n CPP file for DFRobot's Positioning ir camera
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [Angelo](Angelo.qiao@dfrobot.com)
 * @version  V1.0
 * @date  2016-02-17
 */

#include "DFRobotIRPosition.h"

DFRobotIRPosition::DFRobotIRPosition()
{
  
}

DFRobotIRPosition::~DFRobotIRPosition()
{
  
}

void DFRobotIRPosition::writeTwoIICByte(uint8_t first, uint8_t second)
{
  Wire.beginTransmission(IRAddress);
  Wire.write(first);
  Wire.write(second);
  Wire.endTransmission();
}

void DFRobotIRPosition::begin()
{
  writeTwoIICByte(0x30,0x01);
  delay(10);
  writeTwoIICByte(0x30,0x08);
  delay(10);
  writeTwoIICByte(0x06,0x90);
  delay(10);
  writeTwoIICByte(0x08,0xC0);
  delay(10);
  writeTwoIICByte(0x1A,0x40);
  delay(10);
  writeTwoIICByte(0x33,0x33);
  delay(10);
  
  delay(100);
}

void DFRobotIRPosition::requestPosition()
{
  Wire.beginTransmission(IRAddress);
  Wire.write(0x36);
  Wire.endTransmission();
  Wire.requestFrom(IRAddress, 16);
}

bool DFRobotIRPosition::available()
{
  uint8_t i = 0, s = 0, data_buf[16];
  while(Wire.available() && i < 16)
  {
    data_buf[i] = Wire.read();
    i++;
  }
  if(i == 0) return false;

  positionX[0] = data_buf[1];
  positionY[0] = data_buf[2];
  s   = data_buf[3];
  positionX[0] += (s & 0x30) <<4;
  positionY[0] += (s & 0xC0) <<2;

  positionX[1] = data_buf[4];
  positionY[1] = data_buf[5];
  s   = data_buf[6];
  positionX[1] += (s & 0x30) <<4;
  positionY[1] += (s & 0xC0) <<2;

  positionX[2] = data_buf[7];
  positionY[2] = data_buf[8];
  s   = data_buf[9];
  positionX[2] += (s & 0x30) <<4;
  positionY[2] += (s & 0xC0) <<2;

  positionX[3] = data_buf[10];
  positionY[3] = data_buf[11];
  s   = data_buf[12];
  positionX[3] += (s & 0x30) <<4;
  positionY[3] += (s & 0xC0) <<2;
  return true;
}

int DFRobotIRPosition::readX(int index)
{
  return positionX[index];
}

int DFRobotIRPosition::readY(int index)
{
  return positionY[index];
}























