#include "wifi.h"
#include "term.h"
#include "myQueue.h"
#include "images.h"
#include "sender.h"

os_timer_t test;
char *test_string ="Wifi Messaging App functional test string";

queue* q;

typedef enum {
	WIFI_CONNECTING,
	WIFI_CONNECTING_ERROR,
	WIFI_CONNECTED,
	TCP_DISCONNECTED,
	TCP_CONNECTING,
	TCP_CONNECTING_ERROR,
	TCP_CONNECTED,
	TCP_SENDING_DATA_ERROR,
	TCP_SENT_DATA
} tConnState;

unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;



extern int ets_uart_printf(const char *fmt, ...);

struct espconn Conn;
esp_tcp ConnTcp;
os_timer_t scan_timer;

char str_buffer[100];

struct bss_info *bss_link;
uint8_t aps[50][32] = {0};
uint8_t apCount;

//struct bss_info * bss_link;
char info[150];
char tcpserverip[15];
ip_addr_t tcp_server_ip;

void wifiEventHandler(System_Event_t *evt){

	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			ets_uart_printf("connected to ssid %s, channel %d\n",
			evt->event_info.connected.ssid,
			evt->event_info.connected.channel);
			os_memset(str_buffer, 0 , os_strlen(str_buffer));
			os_sprintf(str_buffer,"connected to ssid %s \n channel %d\n", evt->event_info.connected.ssid, evt->event_info.connected.channel);
			OLED_Line(2,str_buffer, 1);


		break;

		case EVENT_STAMODE_DISCONNECTED:
			ets_uart_printf("disconnected from ssid %s, because %d\n",
					evt->event_info.disconnected.ssid,
					evt->event_info.disconnected.reason);
					next_ap();


		break;

		case EVENT_STAMODE_GOT_IP:
			ets_uart_printf("got ip: %d.%d.%d.%d", IP2STR(&(evt->event_info.got_ip.ip.addr)));

		break;

		default:
		break;
	}

}

static void ICACHE_FLASH_ATTR tcp_connectcb(void * arg);

void ICACHE_FLASH_ATTR
tcp_recon_cb(void *arg, sint8 err)
{
	struct espconn * pespconn = (struct espconn *) arg;
	espconn_connect(pespconn);
	ets_uart_printf("reconnect callback, error code %d !!! \r\n",err);

}

LOCAL void ICACHE_FLASH_ATTR user_dns_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
struct espconn *pespconn = (struct espconn *)arg;
ets_uart_printf("user_esp_platform_dns_found %d.%d.%d.%d\n",*((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1),*((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));
tcp_server_ip.addr = ipaddr->addr;
           os_memcpy(pespconn->proto.tcp->remote_ip, &ipaddr->addr, 4); // remote ip of tcp server which get by dns

           espconn_connect(pespconn);
}

static void ICACHE_FLASH_ATTR init_done(void){

		Conn.proto.tcp = &ConnTcp;
		Conn.type = ESPCONN_TCP;
		Conn.state = ESPCONN_NONE;
		wifi_set_event_handler_cb(wifiEventHandler);
		Conn.proto.tcp->local_port = espconn_port();
		Conn.proto.tcp->remote_port = 80;
		espconn_regist_connectcb(&Conn, tcp_connectcb);
		espconn_regist_reconcb(&Conn, tcp_recon_cb);
		espconn_gethostbyname(&Conn, "www.google.com", &tcp_server_ip, user_dns_found);

	//sint8 espcon_status= espconn_connect(&Conn);
	sint8 espcon_status = 0;

	 switch(espcon_status)
	{
		case ESPCONN_OK:
			ets_uart_printf("TCP created.\r\n");
			break;
		case ESPCONN_RTE:
			ets_uart_printf("Error connection, routing problem.\r\n");
			break;
		case ESPCONN_TIMEOUT:
			ets_uart_printf("Error connection, timeout.\r\n");
			break;
		default:
			ets_uart_printf("Connection error: %d\r\n", espcon_status);
	}
}





void ICACHE_FLASH_ATTR connect_to_aps(){

	static uint8_t i;

	 if ( i < apCount ){

		ets_uart_printf("ssid: %s \n", aps[i]);
		user_set_station_config(init_done, wifiEventHandler, aps[i]);
        i++;
	}
	else{

	ets_uart_printf("set wifi station scan \n");
	os_timer_arm(&scan_timer, timer_time, 0);
	wifi_set_event_handler_cb(NULL);
	i=0;
	}

}


static void ICACHE_FLASH_ATTR scanCb(void* arg, STATUS status){

	apCount = 0;
	ets_uart_printf("status: %d\n",status);

	if (status == OK) {

		bss_link = (struct bss_info *)arg;
		bss_link = bss_link->next.stqe_next;//ignore first

		int i;
		bool ssid_added = false;

		ets_uart_printf("access points: ");
		while(bss_link != NULL){

			ssid_added = false;
			if (bss_link->authmode == AUTH_OPEN){


				for (i = 0; i < apCount; i++){


					if (os_strcmp(bss_link->ssid, aps[i]) == 0){
						ssid_added = true;
						break;
					}

				}

				if (ssid_added == false){
					os_sprintf(aps[apCount],"%s",bss_link->ssid);
					ets_uart_printf(" %s ,",aps[apCount]);

					apCount++;
				}

		    }
			bss_link = bss_link->next.stqe_next;

		}
		ets_uart_printf("\n");
		ets_uart_printf("the number of access points: %d \n", apCount);

	}
	next_ap();
}

void ICACHE_FLASH_ATTR scan(void)
{
ets_uart_printf("scanning \n");
wifi_station_scan(NULL, &scanCb);
}

void ICACHE_FLASH_ATTR tcp_recvcb(void *arg, char *pdata, unsigned short len){

ets_uart_printf("received shit: %d \n",len);
ets_uart_printf("%s \n", pdata);
//term.print(pdata);

}

static void ICACHE_FLASH_ATTR tcp_sentcb(void *arg){
ets_uart_printf("sent");

}

void ICACHE_FLASH_ATTR tcp_disconcb(void *arg){
ets_uart_printf("hey discon");
}

static void ICACHE_FLASH_ATTR tcp_connectcb(void * arg)
{
	 char* TLSHEAD = "GET /1/classes/Message HTTP/1.1\r\nHost: %d.%d.%d.%d\r\n"
				"X-Parse-Application-Id: AsCIOVSlwmT8KBcHJ3zbqgeRTwx8xOk4sQ9wbEJa\r\n"
				"X-Parse-REST-API-Key: PC7BAmNl4dCVfe1dELpxB5DYOziAZKUNGvxVadPq\r\n\r\n";

 struct espconn *pespconn = (struct espconn*)arg ;
 espconn_regist_recvcb(pespconn, tcp_recvcb);
 espconn_regist_disconcb(pespconn, tcp_disconcb);
 espconn_regist_sentcb(pespconn, tcp_sentcb);
 espconn_sent(pespconn,TLSHEAD,os_strlen(TLSHEAD));

 //OLED_DrawBMP(20,20,57,57,myImage);
 ets_uart_printf("ok its connected");

}

void ICACHE_FLASH_ATTR tcp_reconnectcb(void *arg){
	struct espconn *pespconn = (struct espconn*) arg;
}





void user_rf_pre_init(void)
{
}


void initDone(void * parg){

	static int i = 0;
	static int step = 0;
	char * string = (char*) parg;

	static char array[21] ="                     ";
	int j;

	int length = os_strlen(string);

	ets_uart_printf("%d\n", i);

	if (i <= length - 1)
		term_scroll_char(*(string + i));
	else
		term_scroll_char(' ');

	if (i == length + 20)
		i = 0;
	else i++;

}



void scroll(void)
{
    os_timer_disarm(&test);
    os_timer_setfn(&test,(os_timer_func_t *) initDone, test_string);
    os_timer_arm(&test, 150, 1);

}

void init(void){
	scroll();
}

void uartCb(void *parg){

	char* string = (char *) parg;
	//ets_uart_printf("it works : %s \n ", string);
}

//Init function
void ICACHE_FLASH_ATTR user_init()
{
  	uart_init(BIT_RATE_115200, BIT_RATE_115200);
  	i2c_init();
  	OLED_Init();
	os_delay_us(1000);
	OLED_Print(48, 0, "Titus App", 1);

	uartCbp = uartCb;

	term_init();
	OLED_DrawBMP(20,20,57,57,myImage);


    wifi_set_opmode((wifi_get_opmode()|STATION_MODE)&USE_WIFI_MODE);

    system_init_done_cb(init);


}


