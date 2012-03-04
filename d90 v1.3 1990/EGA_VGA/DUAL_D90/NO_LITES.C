#include "stdio.h"


extern void far my_pckb();
extern int far read_host_buffer();



void far alt_functions()
{
int the_char,i;


	the_char = ' ';
	while(the_char != 13 /*cr*/)
	{
		the_char = read_host_buffer();

		while(the_char == -1)
		{
			for(i = 0 ; i < 1000 ; i++)
				;

			my_pckb();

			the_char = read_host_buffer();
		}
	}
}
