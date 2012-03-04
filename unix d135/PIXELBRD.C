
/* PIXELBRD.C   generic graphics board functions */



/*March 22, 1989 made my_dash_pattern()  do nothing at all due to a dave
problem with slpat32()*/

/*march 22, 1989 removed my interrupt handler to its own file*/

/* Sept. 15, 1989:

- increased TIME OUT constant to 300,000 (from 75,000)

*/

#include <pw_lib.h>
#include "my_globs.h"
#include <ctype.h>
#include "expanded.h"
#include "mywindows.h"


#define TIME_OUT 0
#define SUCCESS 1
#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1
#define RES_1024_768 3
#define RES_1280_1024 4


extern void  new_read_config();


extern void  error_logger();
extern OPCODE  io_masks[];
extern struct a_window  my_windows[];

extern int  mouse_x_max,  mouse_y_max;
extern char  use_the_mouse;
extern void  restore_int3();

extern void  read_window_config();
extern void  read_hardcopy_info();
extern void  compare_serial_ids();
extern void  read_lock();



extern void  exit_dealloc_pages();

extern char  use_the_debug_mode;

extern char  licensee[];

extern void  flood_draw_gmark();

extern void  close_host();


extern void  read_curser_pos();
extern void  goto_row_col();
extern char  ems_mode;
extern void  release_version();
extern void  restore_int9();
extern void  read_config_file();
extern void  asterisk();
extern void  plus();
extern void  dot();
extern int  init_baud_rate;
extern void  clear_display_page();


/*my interrupt handler function*/
extern void inthand();


int  x, y;
char  pen_press;
char  complement_on = 'f';
char  m_dash_enable = 'f';
int  m_dash_pattern[20];
char  a_char_str[5];
int  the_color = 15;
int  pattern_num = 0;
char  is_eof = 'f';
int  ch, drive_ok;
char  test_name[100];

char  which_high_res;

char  have_display_list;
unsigned long  max_wait_time = 700000L;

/* used in my_set_viewport() to shift viewport to the left side of screen */
int  window_shift = 683;


char  is_there_graphics = 'f';
int  graphics_window_x1,  graphics_window_x2,  graphics_window_y1,
	 graphics_window_y2;


/*1024 x 768 version of clipper graphics calls*/





int  my_pw_dl_wait()
{
unsigned long duration;
int save_row,save_col;

	pw_dl_wait();


#ifdef llllllllllllllllllllll
	duration = 0L;

	while((*interrupt_reason_ptr) < 0)
	{
		duration = duration + 1L;


		if(duration > max_wait_time)

		{
/*
			save_row = row_curser;
			save_col = col_curser;
			read_curser_pos();
			goto_row_col(24,68);

			putchar(7);
			printf("TIME OUT   ");

			goto_row_col(save_row,save_col);
			row_curser = save_row;
			col_curser = save_col;
*/

			error_logger("TIME OUT DURING DLRUN()\n");

			return(TIME_OUT);
		}
	}
#endif

	return(SUCCESS);
}




/*opens & inits clipper board & reads NEW Work.cfg file*/
int  init_artist10()
{
int i;


	read_hardcopy_info();


	if(use_the_debug_mode == 't')
		printf("\t* Reading CUSTOMER.ID File\n");


	read_lock();

	if(use_the_debug_mode == 't')
	{
		printf("\t* Read Complete\n");
		printf("\t* Reading WORK.CFG File\n");
	}


/*
	read_config_file();
*/
	new_read_config();

	compare_serial_ids();


	if(which_high_res == RES_1280_1024)
	{
		mouse_x_max = 1279;
		mouse_y_max = 1023;
	}
	else
	{
		mouse_x_max = 1023;
		mouse_y_max = 767;
	}



	if(use_the_debug_mode == 't')
	{
		printf("\t* Read Complete\n");
		printf("\t* Install Clipper Interrupt Routine\n");
	}

	i = pw_setup(10,6,0x340,NULL);


	if(use_the_debug_mode == 't')
		printf("\t* Clipper Interrupt Routine Installed\n");


	if(i != 0)
	{
		exit_dealloc_pages(handle);
		printf("ERROR DURING pw_setup = %d\n",i);
/*
		close_host();
*/

		exit(0);
	}


	if(use_the_debug_mode == 't')
		printf("\t* Initializing Transform Matrices\n");


	pw_dl_init(NULL,0,NULL,0);
	mdiden2();
	mdiden3();
	mdorg2(0,0);
	mdorg3(0,0,0);
	vwmatx3(&my_3d_viewing_matrix[0]);
	vwrpt3(0,0,0);

	gtsize(40,40);
	gtstyle(2);
	gtprop(0);

	rdmask(0x0f);
	wrmask(0x0f);
	linpat(0);
	window(-2048,2048,-2048,2048);


byteswap(1);


	read_window_config();




	if(use_the_debug_mode == 't')
		printf("\t* Matrices Initialized\n");

	return(0);
}



/*closes expanded memory board*/
void  close_board()
{
	dealloc_pages_for_handle(handle);
}




/*test the artist board & fill board_string*/
void  test_board()
{
	sprintf(board_string,"Pixel Works");
}


void  calculate_graphics_viewport()
{
short x1,y1,x2,y2;
int my_which_page;
short viewport_shift;
float viewport_scale;

	x1 = -2048;
	y1 = -2048;
	x2 = 2048;
	y2 = 2048;

	viewport_scale = my_windows[GRAPHICS_WINDOW].full_size.
		screen_width/768.0;

	if(which_high_res == RES_1024_768)
	{
		viewport_shift = (short)(window_shift/5.3333333333);

		x1 = (short)(x1/5.33333333333) + 512;
		x2 = (short)(x2/5.33333333333) + 512;
		y1 = (short)((y1 + 2048)/5.333333333333);
		y2 = (short)((y2 + 2048)/5.333333333333);
	}
	else
	{
		viewport_shift = (short)(window_shift*1.25/5.3333333333);

		x1 = (short)(x1*1.25/5.33333333333) + 640;
		x2 = (short)(x2*1.25/5.33333333333) + 640;
		y1 = (short)((y1 + 2048)/5.12*1.28);
		y2 = (short)((y2 + 2048)/5.12*1.28);
	}




	graphics_window_x1 = (int)(viewport_scale*(x1 - viewport_shift)) + 
		my_windows[GRAPHICS_WINDOW].full_size.screen_col;

	graphics_window_x2 = (int)(viewport_scale*(x2 - viewport_shift)) +
		my_windows[GRAPHICS_WINDOW].full_size.screen_col;

	graphics_window_y1 = (int)(viewport_scale*y1) + my_windows[GRAPHICS_WINDOW].
		full_size.screen_row;
					
	graphics_window_y2 = (int)(viewport_scale*y2) + my_windows[GRAPHICS_WINDOW].
		full_size.screen_row;

}




/*sets up a rectanglular clipping window*/
void  my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
int viewport_shift;
float viewport_scale;


	viewport_scale = my_windows[GRAPHICS_WINDOW].full_size.
		screen_width/768.0;


	window((short)x1,(short)x2,(short)y1,(short)y2);


	if(which_high_res == RES_1024_768)
	{
		viewport_shift = (int)(window_shift/5.3333333333);

		x1 = (int)(x1/5.33333333333) + 512;
		x2 = (int)(x2/5.33333333333) + 512;
		y1 = (int)((y1 + 2048)/5.333333333333);
		y2 = (int)((y2 + 2048)/5.333333333333);
	}
	else
	{
		viewport_shift = (int)(window_shift*1.25/5.3333333333);

		x1 = (int)(x1*1.25/5.33333333333) + 640;
		x2 = (int)(x2*1.25/5.33333333333) + 640;
		y1 = (int)((y1 + 2048)/4.0);
		y2 = (int)((y2 + 2048)/4.0);
	}




	x1 = (int)(viewport_scale*(x1 - viewport_shift)) + 
		my_windows[GRAPHICS_WINDOW].full_size.screen_col;

	x2 = (int)(viewport_scale*(x2 - viewport_shift)) +
		my_windows[GRAPHICS_WINDOW].full_size.screen_col;

	y1 = (int)(viewport_scale*y1) + my_windows[GRAPHICS_WINDOW].
		full_size.screen_row;
					
	y2 = (int)(viewport_scale*y2) + my_windows[GRAPHICS_WINDOW].
		full_size.screen_row;

/*
printf("my_set_viewport() : x1 = %d , y1 = %d   x2 = %d , y2 = %d\n",x1,y1,x2,y2);
*/


	if(my_which_page == SVWPT_PAGE_1)
	{
		if(which_high_res == RES_1024_768)
		{
			x1 = (int)(viewport_scale*(128 - viewport_shift)) + 
				my_windows[GRAPHICS_WINDOW].
					full_size.screen_col;

			x2 = (int)(viewport_scale*(895 - viewport_shift)) +
				my_windows[GRAPHICS_WINDOW].
					full_size.screen_col;

			y1 = my_windows[GRAPHICS_WINDOW].full_size.screen_row;
					
			y2 = (int)(viewport_scale*767) +
				 my_windows[GRAPHICS_WINDOW].
					full_size.screen_row;



			viewport((short)x1,(short)x2,(short)y1,(short)y2);
		}
		else
		{
			x1 = (int)(viewport_scale*(160 - viewport_shift)) + 
				my_windows[GRAPHICS_WINDOW].
					full_size.screen_col;

			x2 = (int)(viewport_scale*(1120 - viewport_shift)) +
				my_windows[GRAPHICS_WINDOW].
					full_size.screen_col;

			y1 = my_windows[GRAPHICS_WINDOW].full_size.screen_row;
					
			y2 = (int)(viewport_scale*1023) +
				 my_windows[GRAPHICS_WINDOW].
					full_size.screen_row;


			viewport((short)x1,(short)x2,(short)y1,(short)y2);
		}

	}
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*"both pages"*/
			{
				if(which_high_res == RES_1024_768)
				{
					x1 = (int)(viewport_scale*(128 - viewport_shift)) + 
						my_windows[GRAPHICS_WINDOW].
							full_size.screen_col;

					x2 = (int)(viewport_scale*(895 - viewport_shift)) +
						my_windows[GRAPHICS_WINDOW].
							full_size.screen_col;

					y1 = my_windows[GRAPHICS_WINDOW].
						full_size.screen_row;
					
					y2 = (int)(viewport_scale*767) +
						 my_windows[GRAPHICS_WINDOW].
							full_size.screen_row;



					viewport((short)x1,(short)x2,
						(short)y1,(short)y2);
				}
				else
				{
					x1 = (int)(viewport_scale*(160 - viewport_shift)) + 
						my_windows[GRAPHICS_WINDOW].
							full_size.screen_col;

					x2 = (int)(viewport_scale*(1120 - viewport_shift)) +
						my_windows[GRAPHICS_WINDOW].
							full_size.screen_col;

					y1 = my_windows[GRAPHICS_WINDOW].
						full_size.screen_row;
					
					y2 = (int)(viewport_scale*1023) +
						 my_windows[GRAPHICS_WINDOW].
							full_size.screen_row;


					viewport((short)x1,(short)x2,
						(short)y1,(short)y2);
				}

			}
			else
				viewport((short)x1,(short)x2,
					(short)y1,(short)y2);
		}
		else
			viewport((short)x1,(short)x2,(short)y1,(short)y2);
	}

}



/*locks the drawing within the rectangular clipping window*/
void  my_lock_viewport()
{

}


/*scrolls between one page and the other*/
void  my_scroll(my_which_page)
int my_which_page;
{

}


/*selects a color register*/
void  my_color(i)
int i;
{

	color((short)((i & 0x0f) | ((i & 0x0f) << 4)));
	the_color = i & 0x0f;
}


/* changes data in a color register */

void  color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	lut8((short)(the_register & 0x0f),(short)((red << 2) & 0xff),
		(short)((green << 2) & 0xff),(short)((blue << 2) & 0xff));
	lut8((short)((the_register & 0x0f) << 4),(short)((red << 2) & 0xff),
		(short)((green << 2) & 0xff),(short)((blue << 2) & 0xff));



/*
fprintf(stdprn,"COLOR REG # %d  :  red = %d  green = %d  blue = %d\n",
	the_register,red,green,blue);
fflush(stdprn);
*/

}




/* routine sets up the dashed line pattern;
   not currently executed - causes communication data error */

void  my_dash_pattern(i)
unsigned int i;
{
	switch(i)
	{
		case 0xffff : /*solid*/
		{
			slpat32(1,0xffffffff);
			break;
		}

		case 0xeeee : /*dashed*/
		{
			slpat32(1,0xf0f0f0f0);
			break;
		}

		case 0xffb6 : /*phantom*/
		{
			slpat32(1,0xfff0f0f0);
			break;
		}

		case 0xfff6 : /*centerline*/
		{
			slpat32(1,0xfffff0f0);
			break;
		}
	}
}




/*determines whether lines are dashed or not*/
void  my_dash_enable(i)
int i;
{


	if(i == 0)
	{
		linpat(0);
	}
	else if(i == 4)
		linpat(1);
}




/*draws a line from (x1,y1) to (x2,y2)*/
void  my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	vect3((short)x1,(short)y1,(short)0,(short)x2,(short)y2,(short)0);
}


/*puts a charactor on the screen with rotation = "rotate"*/
void  place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	if((the_char >= 32) && (the_char <= 127))
	{
		move2((short)x,(short)y);
		a_char_str[0] = the_char;
		a_char_str[1] = '\0';
		gtext(a_char_str);
	}
}



/*sets up the size of charactors*/
void  my_size_chars(size)
int size;
{
	if(size == 0)
	{
		gtsize(40,40);
	}
	else
		gtsize(80,80);
}



void  my_flood(i)
unsigned int i;
{
	flood((short)i);
}



/*clears a page of the display*/
void  clear_graphics_window()
{
int temp;

	if(is_there_graphics == 'f')
		return;


	temp = my_windows[GRAPHICS_WINDOW].full_size.background;
	my_set_viewport(-2048,-2048,2048,2048,0);

	flood((short)((temp << 4) | temp));
}


#ifdef lflflflfllfll
/*clears a page of the display*/
void  clear_page(my_which_page)
int my_which_page;
{
int temp;

	if(is_there_graphics == 'f')
		return;


	temp = my_windows[GRAPHICS_WINDOW].full_size.background;
	my_set_viewport(-2048,-2048,2048,2048,0);

	flood((temp << 4) | temp);
	flood_draw_gmark();
	my_set_viewport(-2048,-2048,2048,2048,0);
}
#endif


/*clear screen in fast mode*/
void  clear_screen()
{
int temp;

	if(is_there_graphics == 'f')
		return;


	temp = my_windows[GRAPHICS_WINDOW].full_size.background;
	my_set_viewport(-2048,-2048,2048,2048,0);

	flood((short)((temp << 4) | temp));

	flood_draw_gmark();
	my_set_viewport(-2048,-2048,2048,2048,0);

}



/*sets drawing mode (normal or complement)*/
void  draw_mode(i)
int i;
{


	if(i == 3)
	{
		linfun(11);
	}
	else if(i == 0)
		linfun(12);
}



