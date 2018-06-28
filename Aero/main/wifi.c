/*
 * wifi.c
 *
 *  Created on: Jan 11, 2018
 *      Author: cmaity
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wps.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"


/* WiFi http server code start*/

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/portmacro.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "tcpip_adapter.h"
static EventGroupHandle_t event_group;

//const int WIFI_CONNECTED_BIT = BIT0;
const int STA_CONNECTED_BIT = BIT0;
//const int STA_DISCONNECTED_BIT = BIT1;

#include "lwip/err.h"
#include "string.h"
#include "cJSON.h"

#include "mdns.h"

#define LED_BUILTIN 16
#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))
char* json_unformatted;

const static char http_html_hdr[] =
    "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_index_hml[] = "<!DOCTYPE html>"
      "<html>\n"
      "<head>\n"
      "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
      "  <style type=\"text/css\">\n"
      "    html, body, iframe { margin: 0; padding: 0; height: 100%; }\n"
      "    iframe { display: block; width: 100%; border: none; }\n"
      "  </style>\n"
      "<title>HELLO AERO</title>\n"
      "</head>\n"
      "<body>\n"
      "<h1>Hello, from Aero!</h1>\n"
      "</body>\n"
      "</html>\n";


#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"


static void
http_server_netconn_serve(struct netconn *conn)
{
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  /* Read the data from the port, blocking if nothing yet there.
   We assume the request (the part we care about) is in one netbuf */
  err = netconn_recv(conn, &inbuf);

  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);

    /* Is this an HTTP GET command? (only check the first 5 chars, since
    there are other formats for GET, and we're keeping it very simple )*/
    if (buflen>=5 &&
        buf[0]=='G' &&
        buf[1]=='E' &&
        buf[2]=='T' &&
        buf[3]==' ' &&
        buf[4]=='/' ) {
          printf("%c\n", buf[5]);
      /* Send the HTML header
             * subtract 1 from the size, since we dont send the \0 in the string
             * NETCONN_NOCOPY: our data is const static, so no need to copy it
       */

          /*
          gpio_pad_select_gpio(DIODE_PIN);
       //Set the GPIO as a push/pull output
       gpio_set_direction(DIODE_PIN, GPIO_MODE_OUTPUT);
       if(buf[5]=='h'){
         gpio_set_level(DIODE_PIN,1);
       }
       if(buf[5]=='l'){
         gpio_set_level(DIODE_PIN,0);
       }
       */
      netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

      /* Send our HTML page */
      netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
    }

  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);

  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}

static void http_server(void *pvParameters)
{
  struct netconn *conn, *newconn;
  err_t err;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, NULL, 80);
  netconn_listen(conn);
  do {
     err = netconn_accept(conn, &newconn);
     if (err == ERR_OK) {
       http_server_netconn_serve(newconn);
       netconn_delete(newconn);
     }
     vTaskDelay(1); //allows task to be pre-empted
   } while(err == ERR_OK);
   netconn_close(conn);
   netconn_delete(conn);
}




/* HTTP webserver code ends here*/





static esp_err_t esp32_wifi_eventHandler(void *ctx, system_event_t *event) {
	// Your event handling code here...
	 switch(event->event_id) {
	 printf("?\n");
	    case SYSTEM_EVENT_AP_START:
			printf("Access point started\n");
			break;

		case SYSTEM_EVENT_AP_STACONNECTED:
			printf("Station Connected\n");
			xEventGroupSetBits(event_group, STA_CONNECTED_BIT);
			xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
			break;

		case SYSTEM_EVENT_AP_STADISCONNECTED:
			printf("Station Disconnected\n");
			//xEventGroupSetBits(event_group, STA_DISCONNECTED_BIT);
			xEventGroupClearBits(event_group, STA_CONNECTED_BIT);
			break;

		default:
	        break;
	    }

	return ESP_OK;
}

wifi_config_t apConfig_New ;
void access_point_task() {
	nvs_flash_init();
	event_group = xEventGroupCreate();
	tcpip_adapter_init();

	// stop DHCP server
		ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
		// assign a static IP to the network interface
		tcpip_adapter_ip_info_t info;
	    memset(&info, 0, sizeof(info));
		IP4_ADDR(&info.ip, 192, 168, 10, 1);
	    IP4_ADDR(&info.gw, 192, 168, 10, 1);
	    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
		ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));

		// start the DHCP server
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));


	ESP_ERROR_CHECK( esp_event_loop_init(esp32_wifi_eventHandler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
	wifi_config_t apConfig = {
	   .ap = {
	      .ssid="AERO\nPM2.5=000",
	      .ssid_len=0,
	      .password="",
	      .channel=0,
	      .authmode=WIFI_AUTH_OPEN,
	      .ssid_hidden=0,
	      .max_connection=4,
	      .beacon_interval=100
	   }
	};
	apConfig_New=apConfig;

	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &apConfig) );
	ESP_ERROR_CHECK( esp_wifi_start() );

	xEventGroupWaitBits(event_group, STA_CONNECTED_BIT , pdFALSE, pdTRUE, portMAX_DELAY);


	// run the mDNS daemon
		mdns_server_t* mDNS = NULL;
		ESP_ERROR_CHECK(mdns_init(TCPIP_ADAPTER_IF_AP, &mDNS));
		ESP_ERROR_CHECK(mdns_set_hostname(mDNS, "aero"));
		ESP_ERROR_CHECK(mdns_set_instance(mDNS, "Basic HTTP Server"));
	printf("mDNS started\n");



	vTaskDelete(NULL);
	// xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
}

void wifi_task(void *pvParameter)
{
	access_point_task();
}

int init_wifi()
{
	 xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, NULL);

	 return 0;
}

void Set_AP( char * _ssid)
{
	//return;

	strncpy((char *)apConfig_New.ap.ssid,_ssid, strlen(_ssid));
	apConfig_New.ap.ssid[strlen(_ssid)] = '\0';
/*
	wifi_config_t apConfig = {
		   .ap = {
			  .ssid=&dd,
		      .ssid_len=0,
		      .password="",
		      .channel=0,
		      .authmode=WIFI_AUTH_OPEN,
		      .ssid_hidden=0,
		      .max_connection=4,
		      .beacon_interval=100
		   }
		};
	*/
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &apConfig_New) );
}
