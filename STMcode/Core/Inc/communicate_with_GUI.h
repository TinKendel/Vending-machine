/*
 * communicate_with_GUI.h
 *
 *  Created on: Jun 27, 2024
 *      Author: ana.zebic
 */

#ifndef INC_COMMUNICATE_WITH_GUI_H_
#define INC_COMMUNICATE_WITH_GUI_H_

#include "Data.h"

void serialize_Automat(uint8_t*, uint8_t);
void send_Serialized_Automat(uint8_t*, uint8_t);
void receive_Data(int8_t*, uint8_t);

#endif /* INC_COMMUNICATE_WITH_GUI_H_ */
