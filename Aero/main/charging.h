/*
 * charging.h
 *
 *  Created on: Nov 4, 2017
 *      Author: cmaity
 */

#ifndef MAIN_CHARGING_H_
#define MAIN_CHARGING_H_

#define POWER_IN 			GPIO_NUM_34
#define POWER_OUT 			GPIO_NUM_35
#define CHARGING_STAT 		GPIO_NUM_33
#define CHARGINGSTDBY_STAT 	GPIO_NUM_27

typedef enum {
    BAT_NOTCHARGING = 0,    // Not Charging, No Input Power
    BAT_CHARGING = 1,     	//Charging
    BAT_CHARGEDONE=2,     	//Charging Complete
    BAT_CHARGEERROR = 3,    	//Charging Error
	BAT_CHARGECHIPERROR = 4    	//ERROR in CHARGING IC
} ChargingStatus;

void init_ChargModile(void);
int ChargeingStat(void);
int isVIN(void);
int isVOUT(void);

#endif /* MAIN_CHARGING_H_ */
