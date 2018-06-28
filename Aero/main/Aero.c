/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "math.h"
#include "esp_system.h"
#include "soc/rtc.h"

//#include "esp_deep_sleep.h"
//#include "esp32/ulp.h"
//#include "ulp_main.h"


#include "time.h"
#include "button.h"
#include "fan.h"
#include "Health.h"
#include "charging.h"
#include "ledout.h"

#include "sutil.h"
#include "buzzer.h"
#include "aqi.h"
#include "wifi.h"
//extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
//extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");




//gpio_num_t ULP_FAN_OUT=GPIO_NUM_15;

//gpio_num_t cpu_up_num = GPIO_NUM_25;
//gpio_num_t ulp_up_num = GPIO_NUM_4;

//gpio_num_t cpu_toggle_num = GPIO_NUM_26;
//gpio_num_t ulp_toggle_num = GPIO_NUM_27;

//RTC_DATA_ATTR static int cpu_toggle_counter;


#define BLINK_GPIO GPIO_NUM_2



bool isHall=false;
int Ctr=0;
//time_t Dnow;



static int GCtr=0;
static int GAlrmCtr=60000; //in mSec
static int LoopCtrDel=2000;		//in mSec
//static bool GAlrmFlg=false;
uint32_t chgStat=0;//= ChargeingStat();// Get_BatteryHealth();
uint32_t fanStat=0;
uint32_t batStat;

#define SHOW_ERROR Set_LED(1, 1,10)
#define SHOW_CHARGED Set_LED(7,1,0)

static int _HEALTH=0;
static int _isVin=0;
static int sleepFlg=0;
static int chargedonectr=0;
void indication_task1(void *pvParameter)
{

	int APctr=0;
	char _lssid[33];//="                              ";
	//int WifiFlg=0;
	uint32_t DSen_Min=0;
	uint32_t DSen_Max=999;
	uint32_t DSen=0;
	do
	{
		DSen=Get_DustSensor();
		vTaskDelay(LoopCtrDel/ portTICK_RATE_MS);
	}
	while(DSen==0);


	sprintf(_lssid,"AERO:PM2.5=%d",DSen);
			// strncpy((char *)_lssid, (char *)param->sta_ssid.ssid, param->sta_ssid.ssid_len);
			Set_AP(_lssid);
			printf("SSID=%s\n",_lssid);
			Set_DustSensorMinMax();
	while(1)
	{

		if(GCtr<5000)
			GCtr++;
		else
			GCtr=0;

			chgStat= ChargeingStat();// Get_BatteryHealth();
			fanStat=get_fan();
			batStat= Get_BatteryHealth();
			_isVin=isVIN();
			_HEALTH=Get_BatteryHealthInt();

			//printf("\n$$$$$$$$ Health=%d,%d\n",_HEALTH,chargedonectr);

			/* if(batStat>4000)
				 _HEALTH=7;
			    		else if(batStat>3500 && batStat<4000)
			    			_HEALTH=3;
					else //if(batStat>2500 && batStat<3500)
						_HEALTH=1;
					//else
					//	voltageInt=0;
			*/

			if(_isVin==0)
			{
				if(fanStat>0)
				{
					if(fanStat==1)
						Set_LED(1, 1,1);
					if(fanStat==2)
						Set_LED(3, 1,1);
					if(fanStat==3)
						Set_LED(7, 1,1);
				}
			}
			else
			{
			GCtr=0;
			if(chgStat==BAT_CHARGING)
			{

				if(_HEALTH==0)  // If no Input volatge detected
					_HEALTH=1;
				chargedonectr=0;
				Set_LED(_HEALTH, 1,2);
				//Set_BUZZER( 50,1);
			}
			if(chgStat==BAT_CHARGEDONE)
			{
				//printf("********* I am here 2 \n");

				if(chargedonectr<=10)
				chargedonectr++;

				if(_isVin) // Input Power available
					{
						if(_HEALTH==7)	//Battery charged
						{
							if(chargedonectr>=10)
							SHOW_CHARGED;		//dont Blink LED
							//else
							//Set_LED(_HEALTH, 1,3);
						}
						else
							SHOW_ERROR;			//Error
					}


			}
			if(chgStat==BAT_CHARGEERROR || chgStat==BAT_CHARGECHIPERROR)
			{

				//printf("********* I am here 3 \n");

				SHOW_ERROR;			//Error

			}

		}




		DSen=Get_DustSensor();
		DSen_Min=Get_DustSensorMin();
		DSen_Max=Get_DustSensorMax();
		printf("DS=%d:ctr=%d:Min=%d:Max=%d\n",DSen, APctr,DSen_Min,DSen_Max);
		APctr++;
		if(APctr>=15)
		{
			sprintf(_lssid,"AERO:PM2.5=%d\nMin=%d:Max=%d",DSen,DSen_Min,DSen_Max);
			// strncpy((char *)_lssid, (char *)param->sta_ssid.ssid, param->sta_ssid.ssid_len);
			Set_AP(_lssid);
			printf("SSID=%s\n",_lssid);
			APctr=0;

		}

			vTaskDelay(LoopCtrDel/ portTICK_RATE_MS);
	}
}


void app_main()
{

    printf("Hello Aero: FW VER:1.0. Dated: 10/01/2018\n");

     //Show health status


//xTaskCreate( vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle );

   // nvs_flash_init();
  //  xTaskCreate(&hello_task1, "hello", 2048, NULL, 5, NULL);
 //   init_DateTime();
    //time_t Dnow;
    //time(&Dnow);

    //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_2M);

//    get_Epoch(); // Resets tim
    init_wifi();

    init_LED();
    init_BUZZER();
    init_ChargModile();
    init_fan();
    init_button();
    init_DustSensor();

	_HEALTH=Get_BatteryHealthInt();
	Set_LED(_HEALTH, 5,0);
	Set_BUZZER(30,1);

   // esp_deep_sleep_start();


    xTaskCreate(&indication_task1, "indication_task1", 2048, NULL, 5, NULL);



}

