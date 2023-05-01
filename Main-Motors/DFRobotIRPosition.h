/*!
 * @file DFRobotIRPosition.h
 * @brief DFRobot's Positioning ir camera
 * @n Header file for DFRobot's Positioning ir camera
 *
 * @copyright	[DFRobot](http://www.dfrobot.com), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [Angelo](Angelo.qiao@dfrobot.com)
 * @version  V1.0
 * @date  2016-02-01
 */

#include "Arduino.h"
#include "Wire.h"

#ifndef DFRobotIRPosition_cpp
  #define DFRobotIRPosition_cpp

class DFRobotIRPosition {

  const int IRAddress = 0xB0 >> 1; ///< IIC address of the sensor
  
  int positionX[4];   ///< position x.
  int positionY[4];   ///< position y.
  
  void writeTwoIICByte(uint8_t first, uint8_t second);

public:

  /*!
   *  @brief Constructor
   */
  DFRobotIRPosition();
  
  /*!
   *  @brief Destructor
   */
  ~DFRobotIRPosition();
  
  /*!
   *  @brief initialize the sensor.
   */
  void begin();
  
  /*!
   *  @brief request the position, IIC will block the progress until all the data is transmitted.
   */
  void requestPosition();
  
  /*!
   *  @brief After requesting the position, and the data read from the sensor is ready, True will be returned.
   *
   *  @return Whether data reading is ready.
   *  @retval true Is ready
   *  @retval false Is not ready
   */
  bool available();

  /*!
   *  @brief get the X position of the point, 1023 means empty.
   *
   *  @param index The index of the 4 light objects ranging from 0 to 3,
   *
   *  @return The X position corresponing to the index.
   */
  int readX(int index);
  
  /*!
   *  @brief get the Y position of the point, 1023 means empty.
   *
   *  @param index The index of the 4 light objects ranging from 0 to 3,
   *
   *  @return The Y position corresponing to the index.
   */
  int readY(int index);
};


#endif
