/*
 * aqi.h
 *
 *  Created on: Jan 11, 2018
 *      Author: cmaity
 */

#ifndef MAIN_AQI_H_
#define MAIN_AQI_H_

#define AQI_LED GPIO_NUM_25
#define AQI_IN (ADC1_CHANNEL_0) //GPIO36

uint32_t Get_DustSensor();
void init_DustSensor();
uint32_t Get_DustSensorMin();
uint32_t Get_DustSensorMax();
void Set_DustSensorMinMax();


#endif /* MAIN_AQI_H_ */
