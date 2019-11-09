#include "sender.h"


senders* ICACHE_FLASH_ATTR create_senders(){
	senders* nsenders = (senders*) os_zalloc( sizeof(senders) );
	nsenders->pos=0;
	nsenders->create = create_sender;
	nsenders->destroy= destroy_sender;
	nsenders->addNewSender = senders_addNewSender;
	nsenders->addMessage   = senders_addMessage;
	return nsenders;
}

void ICACHE_FLASH_ATTR destroy_senders(senders* nsenders){
	int i;

	#ifdef  debug
	ets_uart_printf("heap before sender deleted :%d \n", system_get_free_heap_size() );
	#endif

	for( i = 0; i < nsenders->pos; i++){
		nsenders->destroy(nsenders->msender[i]);

	#ifdef  debug
	ets_uart_printf("heap after mem delete arrays :%d \n", system_get_free_heap_size() );
	#endif

	}

	os_free(nsenders);

	#ifdef  debug
	ets_uart_printf("heap after sender deleted :%d \n", system_get_free_heap_size() );
	#endif
}

char* ICACHE_FLASH_ATTR senders_addNewSender(senders* nsenders, char * name){

	int i;
	ets_uart_printf("name in addNew %s \n", name);
	if (nsenders->pos == 0){

		nsenders->msender[0] = nsenders->create(name);

		nsenders->pos++;

		return "";
	}

	for (i = 0; i < nsenders->pos; i++){

		if(os_strcmp(nsenders->msender[i]->name, name) == 0){
			ets_uart_printf("sender already added");
			return "";
		}
	}

	if (nsenders->pos < MAX_SENDERS){

		nsenders->msender[nsenders->pos] = nsenders->create(name);
		nsenders->pos++;
	}
	else
		ets_uart_printf("Max Senders");

	return "";

}

void ICACHE_FLASH_ATTR senders_addMessage(senders* nsenders, char* name, char* message, char* date){

	int i;
	bool added = false;

	for (i = 0; i < nsenders->pos; i++){

		if(os_strcmp(nsenders->msender[i]->name, name) == 0){
			ets_uart_printf("good, message is added ");
			added = true;
			break;
		}

	}

	if (added == false)
		return;

	nsenders->msender[i]->addMessage(nsenders->msender[i], message, date);

}



//-----------------------------------------------------------------------------//

sender* ICACHE_FLASH_ATTR create_sender(char* name){

	sender* msender = (sender*) os_zalloc( sizeof(sender) );
	msender->init=  sender_init;
	msender->addMessage = sender_addMessage;
	msender->init(msender, name);
	return msender;
}

void ICACHE_FLASH_ATTR destroy_sender(sender* msender){
	int i;

	#ifdef  debug
	ets_uart_printf("heap before sender deleted :%d \n", system_get_free_heap_size() );
	#endif
	os_free(msender->name);

	for( i = 0; i < msender->pos; i++){
		os_free(msender->message[i]);
		os_free(msender->date[i]);

	#ifdef  debug
	ets_uart_printf("heap after mem delete arrays :%d \n", system_get_free_heap_size() );
	#endif

	}

	os_free(msender);

	#ifdef  debug
	ets_uart_printf("heap after sender deleted :%d \n", system_get_free_heap_size() );
	#endif
}

void ICACHE_FLASH_ATTR sender_init(sender* msender, char* name){


	msender->pos= 0;

	msender->name = (char*) os_zalloc(os_strlen(name));

	os_sprintf(msender->name, "%s", name);

}


void ICACHE_FLASH_ATTR sender_addMessage(sender* msender, char* message, char* date){

	int pos = msender->pos;

	if (pos == MAX_MESSAGES){
		ets_uart_printf("reached message limit");
		return;
	}

	msender->message[pos] = (char*) os_zalloc(os_strlen(message));
	os_sprintf(msender->message[pos], "%s", message);

	msender->date[pos] = (char*) os_zalloc(os_strlen(date));
	os_sprintf(msender->date[pos], "%s", date);

	msender->pos++;


}

