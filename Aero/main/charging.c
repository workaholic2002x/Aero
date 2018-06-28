/*
 * charging.c
 *
 *  Created on: Nov 4, 2017
 *      Author: cmaity
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


//#include <stdio.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

//#include "driver/gpio.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
#include "soc/soc_ulp.h"
//#include "ulp_main.h"


#include "soc/rtc_io_reg.h"
#include "soc/soc.h"


#include <stdio.h>
#include "esp_deep_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "soc/soc.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
//#include "ulp_main.h"

#include "freertos/timers.h"
#include "freertos/task.h"

#include "charging.h"

#define POWER_INPUT_SEL  ((1<<POWER_IN))
#define CHRGSTAT_INPUT_SEL  (1<<CHARGING_STAT)| (1<<CHARGINGSTDBY_STAT))

#define ESP_INTR_FLAG_DEFAULT 0
static xQueueHandle charhing_evt_queue = NULL;



int isVOUT()
{
if(gpio_get_level(POWER_OUT))
	return 1;
else
	return 0;
}

int isVIN()
{
if(gpio_get_level(POWER_IN))
	return 1;
else
	return 0;
}

int ChargeingStat()
{
	int status=0;
	if(isVIN()==1)
	{
	if((gpio_get_level(CHARGING_STAT)==0) && (gpio_get_level(CHARGINGSTDBY_STAT)==1))
		status=BAT_CHARGING;		//charging
	else if((gpio_get_level(CHARGING_STAT)==1) && (gpio_get_level(CHARGINGSTDBY_STAT)==0))
		status=BAT_CHARGEDONE; //Charge Completed
	else
		status=BAT_CHARGEERROR; // Charging error (VIN too low or no battery)
	}
	else
	{
		if((gpio_get_level(CHARGING_STAT)==1) && (gpio_get_level(CHARGINGSTDBY_STAT)==1))
			status=BAT_NOTCHARGING; // Not Charging, No Input Power
		else
			status= BAT_CHARGECHIPERROR; // Charging CHIP Error
	}
	return status;
}

static void Charging_task(void* arg)
{
	/*
    uint32_t io_num;
    int STAT=0;
    for(;;) {
        if(xQueueReceive(charhing_evt_queue, &io_num, portMAX_DELAY))
        {
        }
    }
    */
}


void init_ChargModile()
{
/*
	gpio_config_t PIN_io_conf, CHRGSTAT_io_conf;

	// 5VIN Power IN detection ; Rising edge, Input_pin mask,input_Pin,Pull_up disable
	PIN_io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
	PIN_io_conf.pin_bit_mask = POWER_INPUT_SEL;
	PIN_io_conf.mode = GPIO_MODE_INPUT;
	PIN_io_conf.pull_up_en = 0;
	 gpio_config(&PIN_io_conf);

	 // CHARGING STAT detection ; falling edge, Input_pin mask,input_Pin,Pull_up enable
	 CHRGSTAT_io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
	 CHRGSTAT_io_conf.pin_bit_mask = CHRGSTAT_INPUT_SEL;
	 CHRGSTAT_io_conf.mode = GPIO_MODE_INPUT;
	 CHRGSTAT_io_conf.pull_up_en = 1;
	 gpio_config(&CHRGSTAT_io_conf);



	    //change gpio intrrupt type for one pin
	    gpio_set_intr_type(POWER_IN, GPIO_INTR_POSEDGE);
	    gpio_set_intr_type(CHARGING_STAT, GPIO_INTR_NEGEDGE);
	    gpio_set_intr_type(CHARGINGSTDBY_STAT, GPIO_INTR_NEGEDGE);

	    //create a queue to handle gpio event from isr
	        charging_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	        //start gpio task
	        xTaskCreate(Charging_task, "Charging_task", 2048, NULL, 10, NULL);


	        //install gpio isr service
	            gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	            gpio_isr_handler_add(POWER_IN, PIN_isr_handler, (void*) POWER_IN);
	            gpio_isr_handler_add(CHARGING_STAT, CHG_isr_handler, (void*) CHARGING_STAT);
	            gpio_isr_handler_add(CHARGINGSTDBY_STAT, STDBY_isr_handler, (void*) CHARGING_STAT);


	        //  tm = xTimerCreate("Charging_timer", 1000/portTICK_PERIOD_MS, pdTRUE, (void*)1, vCallbackFunction);
	*/


		//5VIN config
	  	gpio_pad_select_gpio(POWER_IN);
	    gpio_set_direction(POWER_IN, GPIO_MODE_INPUT);
	    gpio_pullup_dis(POWER_IN);
	    gpio_pulldown_en(POWER_IN);

	    //5Vout config
	    gpio_pad_select_gpio(POWER_OUT);
	    gpio_set_direction(POWER_OUT, GPIO_MODE_INPUT);
	    gpio_pullup_dis(POWER_OUT);
	    gpio_pulldown_en(POWER_OUT);


	    //CHARGING_STAT config
	    gpio_pad_select_gpio(CHARGING_STAT);
	    gpio_set_direction(CHARGING_STAT, GPIO_MODE_INPUT);
	    gpio_pulldown_dis(CHARGING_STAT);
	    gpio_pullup_en(CHARGING_STAT);

	     //CHARGING_STAT config
	     gpio_pad_select_gpio(CHARGINGSTDBY_STAT);
	     gpio_set_direction(CHARGINGSTDBY_STAT, GPIO_MODE_INPUT);
	     gpio_pulldown_dis(CHARGINGSTDBY_STAT);
	     gpio_pullup_en(CHARGINGSTDBY_STAT);

	    // xTaskCreate(&Charging_task, "Charging_task", 2048,NULL,10,NULL );


}

