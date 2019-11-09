#ifndef _SENDER_H
#define _SENDER_H

#define MAX_MESSAGES 25
#define MAX_SENDERS	 5
#define MAX_MESSAGE_LENGTH 100
//#define debug

#include "c_types.h"
#include "osapi.h"
#include "mem.h"

typedef struct _sender{

	char *name;
	char *message[MAX_MESSAGES];
	char *date[MAX_MESSAGES];
	int   pos;
	void (*init)(struct _sender*,char* name);
	void (*addMessage)(struct _sender*, char*, char*);

}sender;

typedef struct _senders{
	int pos;
	sender*  msender[MAX_SENDERS];
	char*	 (*addNewSender)(struct _senders*, char*);
	void     (*addMessage)(struct _senders*,char*, char*, char*);
	char*    (*getDate)(struct _senders*, char*, int );
	char*    (*getBody)(struct _senders*, char*, int );
	sender*  (*create)(char *);
	void     (*destroy)(sender*);


}senders;

senders* create_senders();
void	 destroy_senders(senders* nsenders);
char*    senders_addNewSender(senders* nsenders, char* name);
void     senders_addMessage(senders* nsenders, char* name, char* message, char* date);

sender*  create_sender(char * name);
void	 destroy_sender(sender* msender);
void  	 sender_init(sender* msender, char* name);
void 	 sender_addMessage(sender* msender, char* message, char* date);



#endif
