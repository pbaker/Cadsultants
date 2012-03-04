#include <stdio.h>


extern char  diag_flag;



void  printme(the_char)
int the_char;
{

	if(diag_flag == 't')
	{
		the_char &= 0x7f;

		if(the_char > 31)
		{
			putchar(the_char);
		}
		else if(the_char == 0x1b)
		{
			printf("\nESC  ");
		}
		else
			printf(" %x\n",the_char);

	}

}

