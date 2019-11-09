#include "wifi.h"


	void (*WifiCallback)(void) = NULL;


	void ICACHE_FLASH_ATTR
	next_ap(void){

    	os_timer_disarm(&test_timer);
    	wifi_set_event_handler_cb(NULL);
    	connect_to_aps();

    }

	/******************************************************************************
     * FunctionName : user_set_station_config
     * Description  : set the router info which ESP8266 station will connect to
     * Parameters   : none
     * Returns      : none
    *******************************************************************************/
    void ICACHE_FLASH_ATTR
	user_set_station_config(void (*gotIp_cb)(void), void (*wifi_cb)(System_Event_t *evt) , char * SSID)
    {
    	struct station_config st;
    	WifiCallback = gotIp_cb;
    	os_sprintf(st.ssid, "%s",SSID);
    	wifi_station_set_config_current(&st);
    	wifi_station_connect();


       //set a timer to check whether got ip from router succeed or not.
        os_timer_disarm(&test_timer);
        os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
        os_timer_arm(&test_timer, 1, 0);

    }

    /******************************************************************************
     * FunctionName : user_check_ip
     * Description  : check whether get ip addr or not
     * Parameters   : none
     * Returns      : none
    *******************************************************************************/
    void ICACHE_FLASH_ATTR
    user_check_ip(void )
    {
    	struct ip_info ipConfig;
    	static uint8_t count;
    	//disarm timer first
        os_timer_disarm(&test_timer);
    	switch(wifi_station_get_connect_status())
    	{
    		case STATION_GOT_IP:
    			wifi_get_ip_info(STATION_IF, &ipConfig);
    			if(ipConfig.ip.addr != 0) {
    			WifiCallback();
    			return;
    			}
    			break;
    		case STATION_WRONG_PASSWORD:

    			#ifdef PLATFORM_DEBUG
    			ets_uart_printf("WiFi connecting error, wrong password\r\n");
    			#endif

    			next_ap();
    			return;

    			break;
    		case STATION_NO_AP_FOUND:

    			#ifdef PLATFORM_DEBUG
    			ets_uart_printf("WiFi connecting error, ap not found\r\n");
    			#endif

    			next_ap();
    			return;

    			break;
    		case STATION_CONNECT_FAIL:

    			#ifdef PLATFORM_DEBUG
    			ets_uart_printf("WiFi connecting fail\r\n");
    			#endif

    			next_ap();
    			return;

    			break;
    		default:

    			#ifdef PLATFORM_DEBUG
    			ets_uart_printf("WiFi connecting...\r\n");

    			count++;
    			if (count > 12){
    				count = 0;
    				next_ap();
    				return;
    			}

    			#endif
    	}
                os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
                os_timer_arm(&test_timer, 1000, 0);
   }


