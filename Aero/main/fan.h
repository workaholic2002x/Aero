/*
 * fan.h
 *
 *  Created on: Nov 3, 2017
 *      Author: cmaity
 */

#ifndef MAIN_FAN_H_
#define MAIN_FAN_H_

#include "driver/gpio.h"

#define FAN_OUTPUT GPIO_NUM_12

void init_fan();
void set_fan (int state);
int get_fan(void);

#endif /* MAIN_FAN_H_ */
