#include "term.h"
#include "myQueue.h"


// bounds are within 3 and 8 in the y.  1 char length in the x directions

queue* q;

void term_init(void){
	term.xpos = 0;
	term.ypos = 3;
	term.print = term_print;
	q = create_queue(21);
}

void term_scroll(char * string)
{



int strlen = os_strlen(string);
char array[21] ="";
int i;


do
{

	for(i = 21; i >= 1; i--){

		array[i] = array[i-1];
		}

	array[0] = string[strlen];

	strlen = strlen - 1;

	OLED_Print(0,2,array,1);


} while(strlen > -1);

}

void term_scroll_char(char p, bool clear){


	static char array[21] ="";

	push(q,p);

	ets_uart_printf("%s\n", array);

	OLED_Print(0,2,q->buffer,1);

}


void term_print(char* string){
	static int xpos;
	int len = os_strlen(string);
	ets_uart_printf("string length: %d ", len);
	int rem;
	char * short_str ="" ;

	while (len > 0)
	{
		os_memset(short_str, 0 , 21);
	    os_memcpy(short_str, string, 19);

		OLED_Print(term.xpos, term.ypos,short_str,1);


		if (len > 19)
        {

			term.ypos = term.ypos + 1;
			term.xpos = 0;

			if (term.ypos >= 8 )
				term.ypos = 3;

			rem = 19;
        }
		else
		{
			 rem = len;
			 term.xpos = rem + 2;
		}
        len = len - rem;

		string = string + rem ;
		//ets_uart_printf("%s \n", short_str);
		ets_uart_printf(" xpos : %d \n", term.xpos);
	}



}
