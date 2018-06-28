/*
 * button.h
 *
 *  Created on: Nov 3, 2017
 *      Author: cmaity
 */

#ifndef MAIN_BUTTON_H_
#define MAIN_BUTTON_H_

#include "driver/gpio.h"

#define SW1_INPUT GPIO_NUM_0	//23
#define SW2_INPUT GPIO_NUM_22
#define GPIO_INPUT_PIN_SEL  ((1<<SW1_INPUT) | (1<<SW2_INPUT))

void init_button(void);



#endif /* MAIN_BUTTON_H_ */
