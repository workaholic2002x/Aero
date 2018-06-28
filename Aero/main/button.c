/*
 * button.c
 *
 *  Created on: Nov 3, 2017
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
//#include "esp_deep_sleep.h"
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

#include "button.h"
#include "fan.h"
#include "Health.h"
#include "xtensa/core-macros.h"
#include "sutil.h"
#include "ledout.h"
#include "buzzer.h"
#include "charging.h"

#define ESP_INTR_FLAG_DEFAULT 0
static xQueueHandle gpio_evt_queue = NULL;
TaskHandle_t * const BUTTONTask;

static int SW1_STAT=0;
static int SW2_STAT=0;

//static int SINGLE_CLICK=0;
//static int DOUBLE_CLICK=0;
//static int LONG_CLICK=0;
TimerHandle_t tm;
TimerHandle_t tmsleep;
int Time_Interval=0;

static int TimerCount=0;
static int Click_Count=0;
static long _ticksec=0;
bool TimerOnFlg=false;

int CLICK_TYPE=0;


void FucntionCall(int CLICK_T)
{

	if(CLICK_T==2 || CLICK_T==5) // Double Click or long click
	{
		int _st=get_fan();
		if(_st==0)
		{
			Set_LED(1, 1,2);
			Set_BUZZER(1000,1);
			set_fan(1); //Start fan
		}
		else
		{
			Set_LED(1, 1,2);
			Set_BUZZER(1000,1);
			set_fan(0);	//Stop Fan
		}
	}


	if(CLICK_T==1) //single click
	{
		int _st=get_fan();
		if(_st>0) //is fan ON
		{
			_st++;
			if (_st>3)
				_st=1;
			set_fan(_st);
			Set_BUZZER(100,_st);

			//Set_LED(1, 1,_st);
			printf("******FAN Speed Changed to: %d\n", _st);
			//change fan speed
			//indicate fan speed
		}
		else //fan off
		{
			uint32_t bathealth= Get_BatteryHealth();
			uint32_t chstat= ChargeingStat();

			int _HEALTH=Get_BatteryHealthInt();
				Set_LED(_HEALTH, 2,0);
				if(_HEALTH==7)
				Set_BUZZER(50,3);
				if(_HEALTH==3)
				Set_BUZZER(50,2);
				if(_HEALTH==1)
				Set_BUZZER(50,1);
				if(_HEALTH==0)
				Set_BUZZER(50,5);


				printf("******Battery health : %d, %dV\n", bathealth, _HEALTH);

		}
	}

}

void Reset_Params()
{
	TimerOnFlg=false;
	xTimerStop(tm,0);
	//vTaskDelete( BUTTONTask );
	//xTimerDelete(tm,0);
	//xTimerDelete(tmsleep,0);

	  //xTimerStop(tm,0);
	  //xTimerStop(tmsleep,0);

	TimerCount=0;
	Click_Count=0;
}




void vCallbackSleep(TimerHandle_t xTimer)
{


	//Set_LED(1, 3,10);
	 vTaskDelay(2000/portTICK_RATE_MS);

	 if((get_fan()==0 || Get_BatteryHealthInt()==1) && (isVIN()==0))
	 {
	/*	 	Set_BUZZER(1000,1);
			printf("\n Entering Sleep\n");
			esp_deep_sleep_start();
	 */
	 }
	 else
	 {
			printf("\n Sleep discarded. Timer Reset.\n");
		 xTimerReset(tmsleep,0);
	 }

//	  Reset_Params();
	 //esp_light_sleep_start();

}
void vCallbackFunction( TimerHandle_t xTimer ) {

	if(Click_Count>0)
	{
	TimerOnFlg=false;
	xTimerStop(tm,0);

	}
	else // Click_Count=0,Long Click
	{
		if(TimerCount<10)
		TimerCount++;
		else
		{
			TimerOnFlg=false;
			xTimerStop(tm,0);
		}
	}

	//tasks
	CLICK_TYPE=0;
	if(TimerCount==0)
	{
		if(Click_Count>2)
		{
			FucntionCall(3);
			//   printf("\n.....Multiple Click\n");
			//single click
		}
		else if(Click_Count==2)
		{
			FucntionCall(2);
			// printf("\n.....Double Click Click\n");
			//double click
		}
		else if(Click_Count==1)
		{
			FucntionCall(1);
			// printf("\n.....Single Click\n");
		}
		else //no click
			return;

		Click_Count=0;
	}
	else
	{
		if(TimerCount>=2 && TimerCount<=10 && Click_Count>0) // more than 1 sec
		{
			FucntionCall(5);
		 //printf("\n.....Long Click\n");
		 TimerCount=0;
		 Click_Count=0;
		}
		else //ignore long Click
		{
			if(Click_Count>1)
			{
			printf("\n.....Click Ignored %d, %d\n",TimerCount,Click_Count);
			TimerCount=0;
				Click_Count=0;
			}
			else
				printf("\n.....Undefined Click %d, %d\n",TimerCount,Click_Count);


		}

	}


//	if(TimeCount>1000)
//		TimeCount=1000;
 //   printf("in timer xx=%d\n", TimeCount++);
    fflush(stdout);
}

static void IRAM_ATTR SW1_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    SW1_STAT=gpio_get_level(SW1_INPUT);
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void IRAM_ATTR SW2_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    SW2_STAT=gpio_get_level(SW2_INPUT);
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

/*
void Button_Press(int HoldTime)
{

	//uint32_t ccount = XTHAL_GET_CCOUNT();
	int _st=get_fan();
	if(HoldTime>0)
	{
		if(HoldTime<10)
		{
			printf("***************FAN previous Status %d\n", _st);
			if(_st==0)
				set_fan(1); //Start fan
			else
				set_fan(0);	//Stop Fan
		}
		else //debug mode
		{
		}
	}
	else //HoldTime==0
	{
		if(_st>0) //Fan already ON; Change speed
		{
			_st++;
			if (_st>3)
				_st=1;
			set_fan(_st);
			printf("******FAN Speed Changed to: %d\n", _st);
		}
		else//Fan OFF
		{
			uint32_t bathealth= Get_BatteryHealth();
			printf("******Battery health : %d V\n", bathealth);
			uint32_t Fanhealth= Get_FilterHealth();
			printf("******Filter health : %u mV\n", Fanhealth);
			//Show Battery Status
		}
	}

}
*/

static void Button_task(void* arg)
{
    uint32_t io_num;
    int STAT=0;

    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
        	xTimerReset(tmsleep,0);
        	STAT=gpio_get_level(io_num);
        	if(io_num==SW1_INPUT)
        	{

        		if(SW1_STAT==0)
        		{
        			if(STAT==0) 	//Switch is just Pressed
        			{
        				SW1_STAT=1;
        				_ticksec=get_Epoch();
        				if(TimerOnFlg==false)
        				{
        				xTimerStart(tm,0);
        				TimerOnFlg=true;
        				}
        			}
        		}
        		else //SW1_STAT==1; Switch was pressed
        		{
        			if(STAT==1)		//Switch is released
        			{
        				SW1_STAT=2;	// Single click completed
        				//xTimerStop(tm,0);
        			//Button_Press(get_Epoch()-_ticksec);
        				Click_Count++;
        			SW1_STAT=0;
        			//TimeCount=0;
        			}
        		}





        	}

        //	printf("GPIO[%d] intr, val: %d\n", io_num, STAT);
        }
    }
}

void wakeup_enable()
{
	  //const int ext_wakeup_pin_1 = 25;
	  //  const uint64_t ext_wakeup_pin_1_mask = 1ULL << SW1_INPUT;

	  // esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
	    esp_sleep_enable_ext0_wakeup(SW1_INPUT, 0);

}

void init_button()
{

	gpio_config_t io_conf;

	 //interrupt of rising edge
	 io_conf.intr_type = GPIO_PIN_INTR_ANYEDGE;//GPIO_PIN_INTR_POSEDGE;
	 //bit mask of the pins, use GPIO4/5 here
	 io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	 //set as input mode
	 io_conf.mode = GPIO_MODE_INPUT;
	 //enable pull-up mode
	 io_conf.pull_up_en = 1;
	 gpio_config(&io_conf);


	    //change gpio intrrupt type for one pin
	    gpio_set_intr_type(SW1_INPUT, GPIO_INTR_ANYEDGE);
	    gpio_set_intr_type(SW2_INPUT, GPIO_INTR_ANYEDGE);

	    //create a queue to handle gpio event from isr
	        gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	        //start gpio task
	        xTaskCreate(Button_task, "Switch_task", 2048, NULL, 10,BUTTONTask);


	        //install gpio isr service
	            gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	            //hook isr handler for SW1
	            gpio_isr_handler_add(SW1_INPUT, SW1_isr_handler, (void*) SW1_INPUT);
	            //hook isr handler for SW2
	            gpio_isr_handler_add(SW2_INPUT, SW2_isr_handler, (void*) SW2_INPUT);


	            wakeup_enable();
	            tm = xTimerCreate("button_timer", 500/portTICK_PERIOD_MS, pdTRUE, (void*)1, vCallbackFunction);

	            tmsleep = xTimerCreate("sleep_timer", 30000/portTICK_PERIOD_MS, pdTRUE, (void*)1, vCallbackSleep);
	            xTimerStart(tmsleep,0);
}




