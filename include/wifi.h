/*
 * wifi.h
 *
 *  Created on: Jul 5, 2015
 *      Author: Darryl
 */

#ifndef INCLUDE_WIFI_H_
#define INCLUDE_WIFI_H_

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "user_config.h"
#include "ip_addr.h"
#include "espconn.h"

os_timer_t test_timer;

void ICACHE_FLASH_ATTR next_ap(void);
void ICACHE_FLASH_ATTR user_check_ip(void);
void ICACHE_FLASH_ATTR user_set_station_config(void (*gotIp_cb)(void), void (*wifi_cb)(System_Event_t *evt), char * SSID);


#endif /* INCLUDE_WIFI_H_ */
