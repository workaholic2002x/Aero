/*
 * buzzer.h
 *
 *  Created on: Nov 5, 2017
 *      Author: cmaity
 */

#ifndef MAIN_BUZZER_H_
#define MAIN_BUZZER_H_

#define BUZZER_OUT 			GPIO_NUM_21
void init_BUZZER(void);
void Set_BUZZER(int,int);// DURATION:in milisec,BLINK_COUNT

//int Get_BUZZER(void);


#endif /* MAIN_BUZZER_H_ */
