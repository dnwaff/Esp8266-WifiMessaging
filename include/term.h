#ifndef _TERM_H
#define _TERM_H

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"

typedef void(*termFunc)(char * string);

void term_init(void);
void term_print(char * string);
void term_print_textMessage(char* from, char* time, char * message);
void term_print_connected(char* ssid, char* ip);

struct _term{
	int xpos, ypos;
	termFunc print;
	//void (*print_connected)(char* ssid, char* ip);
	//void (*print_textMessage)(char* from, char* time, char * message);

} term;


#endif
