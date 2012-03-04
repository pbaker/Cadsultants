
/* INDICATORS.C   asterisk, period, plus, and letter 'o','O'  code */


/*
 NEVER USE   VECT3(....)   IN HERE EVER AGAIN 
	(SCREWS UP DRAGS, WHICH ARE RELATIVE MOVES & DRAWS)
*/





#include <pw_lib.h>
#include <stdio.h>


#define IND_NON_DIAG 40
#define IND_DIAG 30


extern int  my_x;



void  pixel_asterisk(dx,dy)
int dx,dy;
{
/*
	vect3(x - IND_NON_DIAG,y,z,x + IND_NON_DIAG,y,z);
	vect3(x,y - IND_NON_DIAG,z,x,y + IND_NON_DIAG,z);
	vect3(x - IND_DIAG,y - IND_DIAG,z,x + IND_DIAG,y + IND_DIAG,z);
	vect3(x - IND_DIAG,y + IND_DIAG,z,x + IND_DIAG,y - IND_DIAG,z);
*/

	move3r((short)dx,(short)dy,0);   /* goto center of CELL */

/*  Plus Part */
	move3r(-IND_NON_DIAG,0,0);
	draw3r(2*IND_NON_DIAG,0,0);
	move3r(-IND_NON_DIAG,IND_NON_DIAG,0);
	draw3r(0,-2*IND_NON_DIAG,0);
	move3r(0,IND_NON_DIAG,0);   /* back to center of CELL */

/* Diagonal Lines */
	move3r(IND_DIAG,IND_DIAG,0);
	draw3r(-2*IND_DIAG,-2*IND_DIAG,0);
	move3r(IND_DIAG,IND_DIAG,0);     /* back to center of CELL */
	move3r(IND_DIAG,-IND_DIAG,0);
	draw3r(-2*IND_DIAG,2*IND_DIAG,0);
	move3r(IND_DIAG,-IND_DIAG,0);    /* back to center of CELL */

	move3r((short)(dx + 24),(short)-dy,0);
}


int  pixel_plus_offset = 2;


void  pixel_plus(dx,dy)
int dx,dy;
{
/*
	vect3(x - IND_NON_DIAG,y,z,x + IND_NON_DIAG,y,z);
	vect3(x,y - IND_NON_DIAG,z,x,y + IND_NON_DIAG,z);
*/

	move3r((short)dx,(short)dy,0);   /* goto center of CELL */

	move3r(-IND_NON_DIAG,0,0);
	draw3r(2*IND_NON_DIAG,0,0);
	move3r(-IND_NON_DIAG,IND_NON_DIAG,0);
	draw3r(0,-2*IND_NON_DIAG,0);

	move3r(0,IND_NON_DIAG,0);   /* back to center of CELL */

	move3r((short)(dx + 24),(short)-dy,0);
}




void  pixel_period(dx,dy)
int dx,dy;
{
/*
	vect3(x - 2,y - 2,z,x + 2,y - 2,z);
	vect3(x - 2,y,z,x + 2,y,z);
	vect3(x - 2,y + 2,z,x + 2,y + 2,z);
*/

	move3r((short)dx,(short)dy,0);   /* goto center of CELL */

	move3r(-2,-2,0);
	draw3r(4,0,0);
	draw3r(0,4,0);
	draw3r(-4,0,0);
	draw3r(0,-4,0);

	move3r(2,2,0);     /* goto center of CELL */

	move3r((short)(dx + 24),(short)-dy,0);
}


void  pixel_lower_o(dx,dy)
int dx,dy;
{
	move3r((short)dx,(short)dy,0);   /* goto center of CELL */

	draw3r(IND_DIAG,-IND_DIAG,0);
	draw3r(-IND_DIAG,-IND_DIAG,0);
	draw3r(-IND_DIAG,IND_DIAG,0);
	draw3r(IND_DIAG,IND_DIAG,0);

	move3r((short)(dx + 24),(short)-dy,0);
}

void  pixel_upper_o(dx,dy)
int dx,dy;
{
	pixel_lower_o(dx,dy);
}


void  my_insert_indicator(the_char,x,y,z)
char the_char;
int x,y,z;
{

        if(the_char == '*')
        {
                x = 2*12;
                y = 2*24;

		pixel_asterisk(x,y,z);
        }
        else if(the_char == '+')
        {
                x = 2*12;
                y = 2*18;

		pixel_plus(x,y);
        }
        else if(the_char == '.')
	{
                x = 2*16;
		y = 0;

		pixel_period(x,y,z);

		my_x += 48;
	}
	else if(the_char == 'o')
	{
		x = 2*12;    /* guess on */
		y = 2*12;    /* centering */

		pixel_lower_o(x,y);
	}
	else if(the_char == 'O')
	{
		x = 2*12;   /* guess on */
		y = 2*18;   /* centering */

		pixel_upper_o(x,y);
	}

}


