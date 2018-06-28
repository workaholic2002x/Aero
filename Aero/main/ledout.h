/*
 * ledout.h
 *
 *  Created on: Nov 5, 2017
 *      Author: cmaity
 */

#ifndef MAIN_LEDOUT_H_
#define MAIN_LEDOUT_H_

#define LED1_OUT 			GPIO_NUM_2
#define LED2_OUT 			GPIO_NUM_19
#define LED3_OUT 			GPIO_NUM_18

void init_LED(void);

void Set_LED(int, int,int);// LED_NO:0-7,DURATION:0-10 sec,isBLINK:true/false, BLINK_FREQ:1-10(1=100mS)
int Get_LED(void);

#endif /* MAIN_LEDOUT_H_ */
