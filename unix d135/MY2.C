/* MY2.C contains various utilities */

#include "my_globs.h"


void  comp_xy()
{
	short_x &= 0x3f;

	if((flag & 0x04) != 0)  /* then x is negative */
	{
		short_x = ((~short_x) & 0x3f) + 1;
		xx = (int)short_x;
		xx = -xx;
	}
	else
		xx = (int)short_x;

	xx = xx*(size + 1);
	short_y &= 0x3f;

	if((flag & 0x10) != 0)  /* then y is negative */
	{
		short_y = ((~short_y) & 0x3f) + 1;
		yy = (int)short_y;
		yy = -yy;
	}
	else
		yy = (int)short_y;

	yy = yy*(size + 1);
}



unsigned int  comp16_3(first,second,third)
unsigned char first,second,third;
{
unsigned int i;

	i = (int) first & 0x0f;
	i <<= 6;
	i |= (int)second & 0x3f;
	i <<= 6;
	i |= (int)third & 0x3f;

	return(i);
}



int  comp_abs_coord(most_sig,least_sig)
unsigned char most_sig,least_sig;
{
int ii,jj,a_coord;


	ii = (int) most_sig; 
	ii &= 0x003f;
	ii <<= 6;

	jj = (int) least_sig; 
	jj &= 0x003f;
	a_coord = ii | jj;

	if((a_coord & 2048) != 0) /* then its negative */
	{
		a_coord = ((~a_coord) & 0x0fff) + 1;
		a_coord = -a_coord;
	}

	return(a_coord);
}  /* end comp_abs_coord */



int  comp_rel_coord(most_sig,least_sig)
unsigned char most_sig,least_sig;
{
int ii,jj,a_coord;


	ii = (int) most_sig; 
	ii &= 0x003f;
	ii <<= 6;

	jj = (int) least_sig; 
	jj &= 0x003f;
	a_coord = ii | jj;

	if((a_coord & 2048) != 0) /* then its negative */
	{
		a_coord = ((~a_coord) & 0x0fff) + 1;
		a_coord = -a_coord;
	}

	return(a_coord);
}  /* end comp_rel_coord */




int  sub6_abs_coord(val12)
int val12;
{
int i;

	i = val12;

	if((i & 2048) != 0) /*then its negative*/
	{
		i = ((~i) & 0x0fff) + 1;
		i = -i;
	}

	return(i);
}




int  sub6_rel_coord(val12)
int val12;
{
int i;

	i = val12;

	if((i & 2048) != 0) /*then its negative*/
	{
		i = ((~i) & 0x0fff) + 1;
		i = -i;
	}

	return(i);
}


int  comp_coord(val12)
int val12;
{
int i;

	i = val12;

	if((i & 2048) != 0) /*then its negative*/
	{
		i = ((~i) & 0x0fff) + 1;
		i = -i;
	}

	return(i);
}

