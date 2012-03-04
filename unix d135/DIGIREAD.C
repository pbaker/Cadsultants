#include "stdio.h"
#include "string.h"


extern void far digitizer_out(int);
extern int far read_digitizer_buffer(void);


char far the_digitizer_buffer[27];
char far previous_buffer_values[27] = "                  ";





void far read_digitizer(x,y,buttons)
int far *x,far *y,far *buttons;
{
int i,ch,j;
char far *token_ptr;
int digi_x,digi_y;
int temp_buttons;
char com_error_flag;



/*
	digitizer_out(17);

	ch = ' ';

	for(i = 0 ; ch != 10 ; i++)
	{
		ch = read_digitizer_buffer();

		while(ch == -1)
		{
			ch = read_digitizer_buffer();
		}
	}


	printf("DIGIREAD.C   After throwing junk away\n");

*/


#ifdef  rkerrkrlelerll

printf("DIGIREAD.C  BEFORE clearing out Buffer\n");

/* clear out digi buffer */
	ch = read_digitizer_buffer();
	while(ch != -1)
	{
		printf("%x = %c , ",ch,ch);
		ch = read_digitizer_buffer();
	}


printf("DIGIREAD.C  After clearing out Buffer\n");



	digitizer_out('T');
#endif



try_again:

/* assume device has been paused by interrupt driver (XOFF)
   read buffer until empty
*/
	while(read_digitizer_buffer() != -1)
		;

/* turn on device to receive "fresh" data */

	digitizer_out(17);


/* throw away data up to & including 1st line_feed */

	ch = ' ';

	for(i = 0 ; ch != 10 ; i++)
	{
		ch = read_digitizer_buffer();

		while(ch == -1)
		{
			ch = read_digitizer_buffer();
		}
	}


/* read data up to & including next line feed */

	ch = ' ';

	for(i = 0 ; ch != 10 ; i++)
	{
		ch = read_digitizer_buffer();

		while(ch == -1)
		{
			ch = read_digitizer_buffer();
		}

		the_digitizer_buffer[i] = ch & 0x7f;
	}


	the_digitizer_buffer[i] = '\0';

/* check data validity */

	for(j = 0 ; the_digitizer_buffer[j] != '\0' ; j++)
	{
		ch = the_digitizer_buffer[j];

		if(((ch < '0') || (ch > '9')) && (ch != ',') && (ch != 10) &&
			(ch  != 13))
		{
			printf("BOGUS DIGI BUFFER = %s\n",
				the_digitizer_buffer);
			goto try_again;
		}
	}




if(strcmp(the_digitizer_buffer,previous_buffer_values) == 0)
	return;

strcpy(previous_buffer_values,the_digitizer_buffer);



	token_ptr = strtok(the_digitizer_buffer,",");
	digi_x = atoi(token_ptr);

	token_ptr = strtok(NULL,",");
	digi_y = atoi(token_ptr);

	token_ptr = strtok(NULL,",");
	temp_buttons = atoi(token_ptr);

	switch(temp_buttons)
	{
		case 0 :
		{
			*buttons = 0;
			break;
		}

		case 1 :
		{
			*buttons = 8;
			break;
		}

		case 2 :
		{
			*buttons = 4;
			break;
		}

		case 4 :
		{
			*buttons = 2;
			break;
		}

		case 8 :
		{
			*buttons = 1;
			break;
		}


		default :
			*buttons = 0;
	}


	*x = (int)(digi_x*1.862) - 2048;
	*y = (int)(digi_y*1.862) - 2048;

}

