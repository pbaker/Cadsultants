#include "display.h"
#include "stdio.h"
#include "math.h"
#include "graph.h"
#include "malloc.h"


#define X_OFF 19
#define X_ON  17

#define INVERT_Y 482
#define SIZE_CIRCLE 4


#define SVWPT_PAGE_1 4
#define TRUE 1
#define FALSE 0


extern void far ugout(int);
extern void far clear_display_page();
extern void far clrall();
extern void far init_terminal();

extern void far restore_int9();

extern char far black;
extern int far my_x;
extern int far my_y;
extern int far current_color;
extern int far line_style;


extern int far name_char_count;
extern int far view_name_col;
extern int far view_name_row;

extern char far my_clear_tek_flag;


unsigned int far the_port,far port_byte;
long far d90_color;

/*1024 x 768 version of artist 10 series graphics calls*/


char far *little_curser;   /*will point to place for storing my little curser*/
char far temporary_string[5];
char far in_listing_mode = FALSE;
char huge image_save[170000];




void far enter_graphics_mode()
{	
	ugout(X_OFF);

	_setvideomode(_ERESCOLOR);
	init_terminal();

	ugout(X_ON);
}



void far alt_g_restore()
{
	if(in_listing_mode == FALSE)
		return;

	in_listing_mode = FALSE;
	my_y = 0;

	ugout(X_OFF);

	clear_display_page();
	_setvideomode(_VRES16COLOR);

	init_terminal();   /*remap all vga colors to d90 colors*/
	_clearscreen(_GCLEARSCREEN);

	_putimage(0,0,&image_save[0],_GOR);

	ugout(X_ON);
}





void far restore_graphics()
{
	if(my_clear_tek_flag == TRUE)
	{
		if(in_listing_mode == FALSE)
			_clearscreen(_GCLEARSCREEN);
		my_clear_tek_flag = FALSE;
	}

	if(in_listing_mode == FALSE)
		return;

	in_listing_mode = FALSE;
	my_y = 0;

	ugout(X_OFF);

	clear_display_page();
	_setvideomode(_VRES16COLOR);

	init_terminal();   /*remap all vga colors to d90 colors*/
	_clearscreen(_GCLEARSCREEN);

	_putimage(0,0,&image_save[0],_GOR);

	ugout(X_ON);
}



void far my_little_curser(x,y)
int x,y;
{
	restore_graphics();

	x = (int)(x/1.6);
	y = (int)(y/1.6);

        _putimage(x - 6,y - 6,little_curser,_GXOR);
}



void far make_little_crosshair()
{
/*make and save my little curser*/
	_setvideomode(_VRES16COLOR);

        _setcolor(7);
        _moveto(0,6);
        _lineto(12,6);
        _moveto(6,0);
        _lineto(6,12);


        little_curser = (char far *)malloc((unsigned int)_imagesize(0,
                 0,13,13) + 25);


        _getimage(0,0,13,13,little_curser);
        _clearscreen(_GCLEARSCREEN);
	_setvideomode(_TEXTC80);
}




void far close_board()
{
	clrall();
}




void far test_board()
{
	clrall();
}





/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{

}



/*locks the drawing within the rectangular clipping window*/
void far my_lock_viewport()
{

}


/*scrolls between one page and the other*/
void far my_scroll(my_which_page)
int my_which_page;
{

}


/*selects a color register*/
void far my_color(i)
int i;
{
	_setcolor(i & 0x0f);
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	restore_graphics();

	d90_color = ((long)red) | ((long)green << 8) |
		((long)blue << 16);

	_remappalette(the_register,d90_color);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{

}


/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{
	restore_graphics();


	switch(i)
	{
		case 0 : /*solid*/
		{
			_setlinestyle(0xffff);
			break;
		}

		case 1 : /*dashed*/
		{
			_setlinestyle(0xcccc);
			break;
		}

		case 2 : /*centerline*/
		{
			_setlinestyle(0xff1c);
			break;
		}

		case 4 : /*phantom*/
		{
			_setlinestyle(0xfccc);
			break;
		}
	}
}


/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{

}





/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	if(in_listing_mode == TRUE)
		return;

	if(black == FALSE)
		_settextcolor(current_color);
	else
		_settextcolor(0);

	_settextposition(y,x + 1);
	temporary_string[0] = the_char & 0x7f;
	temporary_string[1] = '\0';
	_outtext(temporary_string);
}




void far display_charactor(the_char)
int the_char;
{
	if(in_listing_mode == FALSE)
	{
		if((my_x < 100) && (my_y >= 3000))
		{
			ugout(X_OFF);

			my_clear_tek_flag = FALSE;
			in_listing_mode = TRUE;
			_getimage(0,0,639,479,&image_save[0]);
			_clearscreen(_GCLEARSCREEN);
			_setvideomode(_TEXTC80);

			ugout(X_ON);
		}
	}


	if(my_clear_tek_flag == TRUE)
	{
		my_clear_tek_flag = FALSE;
		if(in_listing_mode == FALSE)
			_clearscreen(_GCLEARSCREEN);
	}
		


	the_char &= 0x7f;

	if((the_char == 10) || (the_char == 13))  /*newline*/
	{
		my_x = 0;
		my_y += (int)(32*1.6);

		if(in_listing_mode == TRUE)
		{
			if(the_char == 13)
				putchar('\n');
		}
	}
	else
	{
		if(in_listing_mode == FALSE)
		{
			name_char_count++;

			if(name_char_count == 1)
			{
				view_name_row = 25 - view_name_row/123;
				view_name_col = view_name_col/51;
			}

			place_char(the_char,view_name_col,view_name_row,6);

			view_name_col++;
		}
		else
			putchar(the_char);

		my_x += (int)(32*1.6);
	}

}









/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{

}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	if(in_listing_mode == FALSE)
		_clearscreen(_GCLEARSCREEN);
}



/*clear screen in fast mode*/
void far clear_screen()
{
	if(in_listing_mode == FALSE)
		_clearscreen(_GCLEARSCREEN);
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
int i;
{

}






void far dot_plot(x,y)
int x,y;
{
	restore_graphics();

	_setpixel(5*x/32,INVERT_Y - 5*y/32);
}






void far my_circle(x,y)
int x,y;
{
	restore_graphics();

	_ellipse(_GBORDER,5*x/32 - SIZE_CIRCLE,INVERT_Y - 5*y/32 - SIZE_CIRCLE,
		5*x/32 + SIZE_CIRCLE,INVERT_Y - 5*y/32 + SIZE_CIRCLE);
}








void far draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	restore_graphics();

	_moveto(5*x1/32,INVERT_Y - 5*y1/32);
	_lineto(5*x2/32,INVERT_Y - 5*y2/32);
}

