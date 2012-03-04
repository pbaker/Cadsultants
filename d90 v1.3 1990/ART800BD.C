#include "c:\emulate\d90\corecode\display.h"
#include "stdio.h"
#include "math.h"

#define SVWPT_PAGE_1 4

#define TRUE  't'
#define FALSE  'f'

#define INDIC_RADIUS 4

extern void far insert_word(void);

extern void far release_version();

extern void far restore_int9();

extern char far black;
extern int far my_x;
extern int far my_y;
extern FILE far *out_ptr;
extern struct a_word far an_element;
extern int far num_pages;
extern char far board_string[];
extern char far rotate_mode;
extern float far new_zoom;
extern int far pan_x;
extern int far pan_y;
extern int far current_color;
extern int far line_style;

unsigned int far the_port,far port_byte;

float far res_factor = 1.28;
float far x_res_factor = 1.28;
float far y_res_factor = 1.371;


extern char far have_display_list;


/*1024 x 768 version of artist 10 series graphics calls*/



void far close_board()
{

}



/*test the artist board & fill board_string*/
void far test_board()
{
unsigned int the_port,port_byte;


	release_version();

	the_port = 0x03e2;

	outp(the_port,0x00);
	port_byte = inp(the_port) & 0x0f;



	switch(port_byte)
	{
		case 0x03 :
		{
			sprintf(board_string,"Artist 10E  800 x 560");
			num_pages = 2;
			rotate_mode = 'a';
			break;
		}

		case 0x0f :
		{
			sprintf(board_string,"Artist 10  800 x 560");
			num_pages = 2;
			rotate_mode = 'a';

			break;
		}

		case 0x0c :
		{
			sprintf(board_string,"Artist 10/16  800 x 560");
			num_pages = 1;
			rotate_mode = 'm';

			break;
		}

		default :
		{
			 printf("Invalid Controller Board\n");
			 restore_int9();
			 exit();
		}
	}

	outp(the_port,0x0d);	
	port_byte = inp(the_port) & 0x0f;

	if(port_byte != 0x07)
	{
		printf("Invalid Graphics Controller Board!!!\n");
		restore_int9();
		exit();
	}

}





/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
	if(my_which_page == SVWPT_PAGE_1)
		svwpt(0,0,(int)(1023/x_res_factor),(int)(767/y_res_factor));
	else
	{
#ifdef jdfgdlksgfdslkgfkndl
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*both pages*/
				svwpt(0,0,1023,767 + 768);
			else
				svwpt(x1,y1 + my_which_page*768,x2,y2 + my_which_page*768);
		}
		else
#endif
			svwpt((int)(x1/x_res_factor),(int)(y1/y_res_factor),
				(int)(x2/x_res_factor),(int)(y2/y_res_factor));
	}
}



/*locks the drawing within the rectangular clipping window*/
void far my_lock_viewport()
{
	lokvwp();
}


/*scrolls between one page and the other*/
void far my_scroll(my_which_page)
int my_which_page;
{
/*	if(num_pages == 2)
		scrola(0,my_which_page*768);
	else
*/
		scrola(0,0);
}


/*selects a color register*/
void far my_color(i)
int i;
{
	scolr(i & 0x0f);
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	usrcol(the_register & 0x0f,red,green,blue);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{
	usrlin(i);
}


/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{
	sltyp(i);
}


/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	linxy((int)(x1/x_res_factor),(int)(y1/y_res_factor),
		(int)(x2/x_res_factor),(int)(y2/y_res_factor));
}





/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	schar(the_char,(int)(x/x_res_factor),(int)(y/y_res_factor),rotate);
}






void far redisplay_charactor(the_char)
int the_char;
{
long temp_y,remainder;
int y;

	the_char &= 0x7f;

	if((the_char == 10) || (the_char == 13))  /*newline*/
	{
		my_x = 0;
		my_y += (int)(32*y_res_factor);
	}
	else
	{
		temp_y = (long)my_y*10000L;
        	remainder = temp_y%10156L;
		temp_y /= 10156L;
	
	        y = (int)temp_y;

		if(remainder == 5078L)
        		y++;
	
		y = 768 - y/4;

		place_char(the_char,my_x/4,y,6);

		my_x += (int)(32*x_res_factor);
	}

}





void far display_charactor(the_char)
int the_char;
{
int y;
long temp_y,remainder;

	the_char &= 0x7f;

	if(have_display_list == TRUE)
	{
		an_element.op_code = 4;
		an_element.a_char = the_char;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.color = current_color;
		an_element.font = line_style;

		insert_word();	
	}


	if((the_char == 10) || (the_char == 13))  /*newline*/
	{
		my_x = 0;
		my_y += (int)(32*y_res_factor);
	}
	else
	{
	        temp_y = (long)my_y*10000L;
        	remainder = temp_y%10156L;
	        temp_y /= 10156L;

        	y = (int)temp_y;

	        if(remainder == 5078L)
        	        y++;
	
		y = 768 - y/4;

		place_char(the_char,my_x/4,y,6);

		my_x += (int)(32*x_res_factor);
	}

	if(out_ptr != NULL)
	{
		if((the_char == 10) || (the_char == 13))  /*newline*/
		{
			putc('\n',out_ptr);
			fflush(out_ptr);
		}
		else
			putc(the_char,out_ptr);
	}
}









/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
	sizchr(8,8,size,size);
}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
#ifdef ijierwjfeiwdikdfkogffo
	if(num_pages == 2)
	{
		if(my_which_page == 2)     /*both pages*/
			clrwrd(0,0,1023,767 + 768,0);
		else
			clrwrd(0,my_which_page*768,1023,767 + 768*my_which_page,0);
	}
	else
#endif
		clrwrd(0,0,(int)(1023/x_res_factor),(int)(767/y_res_factor),0);
}


/*clear screen in fast mode*/
void far clear_screen()
{
	gclrf();
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
int i;
{
	drwmod(i);
}






void far dot_plot(x,y)
int x,y;
{
long temp_y,remainder;

	temp_y = (long)y*10000L;
        remainder = temp_y%10156L;
	temp_y /= 10156L;

        y = (int)temp_y;

	if(remainder == 5078L)
        	y++;

	x = (int)((x - 2048)*new_zoom) + 2048 + pan_x;
	y = (int)((y - 1536)*new_zoom) + 1536 + pan_y;


	x /= 4;
	y = 768 - y/4;

        plotxy((int)(x/x_res_factor),(int)(y/y_res_factor));
}






void far my_circle(x,y)
int x,y;
{
long temp_y,remainder;
double angle,delta_angle;

	temp_y = (long)y*10000L;
        remainder = temp_y%10156L;
	temp_y /= 10156L;

        y = (int)temp_y;

	if(remainder == 5078L)
        	y++;
	
	x = (int)((x - 2048)*new_zoom) + 2048 + pan_x;
	y = (int)((y - 1536)*new_zoom) + 1536 + pan_y;	

	x /= 4;
	y = 768 - y/4;

	delta_angle = 1.0/INDIC_RADIUS;

	for(angle = 0.0 ; angle <= 6.29 ; angle += delta_angle)
		plotxy((int)(INDIC_RADIUS*cos(angle) + x/x_res_factor),
			(int)(INDIC_RADIUS*sin(angle) + y/y_res_factor));
}








void far draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
long remainder,temp_y;

	#ifdef HAVE_DIAGS
		printf("DRAW_LINE(%d %d %d %d)\n",x1,y1,x2,y2);
	#endif

        temp_y = (long)y1*10000L;
        remainder = temp_y%10156L;
        temp_y /= 10156L;

        y1 = (int)temp_y;

        if(remainder == 5078L)
                y1++;

        temp_y = (long)y2*10000L;
        remainder = temp_y%10156L;
        temp_y /= 10156L;

        y2 = (int)temp_y;

        if(remainder == 5078L)
                y2++;

	x1 = (int)((x1 - 2048)*new_zoom) + 2048 + pan_x;
	x2 = (int)((x2 - 2048)*new_zoom) + 2048 + pan_x;
	y1 = (int)((y1 - 1536)*new_zoom) + 1536 + pan_y;
	y2 = (int)((y2 - 1536)*new_zoom) + 1536 + pan_y;



	x1 /= 4;
	x2 /= 4;
	y1 /= 4;
	y2 /= 4;

        y1 = 768 - y1;
        y2 = 768 - y2;

	#ifdef HAVE_DIAGS
		printf("LINXY(%d %d %d %d)\n",x1,y1,x2,y2);
	#endif

        linxy((int)(x1/x_res_factor),(int)(y1/y_res_factor),
		(int)(x2/x_res_factor),(int)(y2/y_res_factor));
}








void far move_redisplay(x,y)
int x,y;
{

	x = (int)((x - 2048)*new_zoom) + 2048 + pan_x;
	y = (int)((y - 1536)*new_zoom) + 1536 + pan_y;
	

	my_x = x;
	my_y = y;

}




