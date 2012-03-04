
/* INDICATORS.C   asterisk, period, and plus   code */


#include "c:\emulate\clipper\pw_lib.h"
#include "stdio.h"


#define IND_NON_DIAG 40
#define IND_DIAG 30


extern int far my_x;



void far pixel_asterisk(x,y,z)
int x,y,z;
{
	vect3(x - IND_NON_DIAG,y,z,x + IND_NON_DIAG,y,z);
	vect3(x,y - IND_NON_DIAG,z,x,y + IND_NON_DIAG,z);
	vect3(x - IND_DIAG,y - IND_DIAG,z,x + IND_DIAG,y + IND_DIAG,z);
	vect3(x - IND_DIAG,y + IND_DIAG,z,x + IND_DIAG,y - IND_DIAG,z);
}



void far pixel_plus(x,y,z)
int x,y,z;
{
	vect3(x - IND_NON_DIAG,y,z,x + IND_NON_DIAG,y,z);
	vect3(x,y - IND_NON_DIAG,z,x,y + IND_NON_DIAG,z);
}


void far pixel_period(x,y,z)
int x,y,z;
{
	vect3(x - 2,y - 2,z,x + 2,y - 2,z);
	vect3(x - 2,y,z,x + 2,y,z);
	vect3(x - 2,y + 2,z,x + 2,y + 2,z);
}



void far my_insert_indicator(the_char,x,y,z)
char the_char;
int x,y,z;
{

        if(the_char == '*')
        {
                x += 2*12;
                y += 2*24;

		pixel_asterisk(x,y,z);
        }
        else if(the_char == '+')
        {
                x += 2*12;
                y += 2*18;

		pixel_plus(x,y,z);
        }
        else if(the_char == '.')
	{
                x += 2*16;

		pixel_period(x,y,z);

		my_x += 48;
	}

}
