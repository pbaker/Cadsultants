
#include <stdio.h>
#include <pw_lib.h>

int mdc_pfk_port;
int count1;
char mdc_pfk_in_use;


char *strupr(a_string)
char *a_string;
{
int i,ch;

	for(i = 0 ; *(a_string + i) != '\0' ; i++)
	{
		ch = *(a_string + i);
		if((ch >= 'a') && (ch <= 'z'))
			*(a_string + i) = *(a_string + i) - 'a' + 'A';
	}

	return(a_string);
}




void w_flag(i)
int i;
{

}

void r_flag(i)
int i;
{

}

void clear_display_page()
{

}

void write_config()
{

}

void viewport(x1,x2,y1,y2)
short x1,x2,y1,y2;
{
	vwport(x1,x2,y1,y2);
}


void vt100_pckb()
{

}

void read_digitizer(x,y,buttons)
int *x,*y,*buttons;
{

}


