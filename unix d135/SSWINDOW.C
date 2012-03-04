

#include "my_globs.h"   /* includes  stdio.h */
#include <stdlib.h>
#include <string.h>
#include <pw_lib.h>
#include "expanded.h"
#include "my_utils.h"
#include "mywindows.h"
#include <malloc.h>


/* double board ONLY!!!!!  (for BLT commands) */
#define FULL_SCREEN_WIDTH   1020
#define FULL_SCREEN_HEIGHT  800


#define DEFAULT_TEXT_SIZE 15



#define NUMBER_OF_WINDOWS  7
#define SHADOW_SIZE  10
#define MY_EOF  -2001
#define TOTAL_NUMBER_TO_READ  NUMBER_OF_WINDOWS*9*2  /* 9 windows with 2 pos */



#define MAX_BYTES_OF_BLT 15000

#define NEXT_BLT_PAGE  16377
#define END_OF_BLT 16376
#define EMS_MARKER 16300
#define BLT_BORDER 0

#define RES_1024_768 3
#define RES_1280_1024 4


#define VT100_MODE  0
#define MESS_MON  1


struct light_line
{
	int col,row;
	char is_on;
	char light_message[80];
};




extern char do_not_dlrun_flag;

extern int  count1;
extern int  vt100_mess_mon_flag;
extern char  which_high_res;

extern float  actual_size;

extern char  in_the_home_menu;

extern struct light_line  light_lines[];
extern char  lights_wiped;


extern unsigned char  *blt_page_frame_ptr;

/*
extern short  software_ems_buffer[];
*/

extern int  which_blt_list;

extern int  find_which_row();
extern char  numeric_value();
extern void  move_pick_to_main_menu();
extern void  change_page_gmark_undraw();

extern void  set_sub_wrmasks();
extern int  list_size;
extern void  my_size_chars();
extern char  picked_lights;


/* for EXITING when it CAN'T find  WINDOWS.CFG */
extern int  handle;
extern void  exit_dealloc_pages();
extern void  close_host();
extern void  restore_int9();

extern void  xor_lighted_bar();
extern char  is_inside_menu();
extern void  change_page_gmark_undraw();
extern void  move_mouse_to_new_pos();
extern char  row_is_lit;
extern int  last_row_lit;

extern void  color_reg();
extern void  restore_graphics_viewport();
extern short  io_masks[];
extern int  which_page;
extern char  read_more_flag;



int  system_background = 0;

struct a_window  my_windows[16];

FILE  *window_config_file;
char  window_config_name[111] = "windows.cfg";
char  window_temp_string[111];
int  number_of_values_found = 0;
int  global_screen_color = 0;

/*
char  last_blt_flag = RESTORED_GRAPHICS;
*/

extern char  light_is_on[];




char  is_window_array_empty(the_window)
int the_window;
{
char far *temp_ptr;
int i,scroll_height,row;


	if(my_windows[the_window].is_retracted == 't')
		scroll_height = my_windows[the_window].retracted.scroll_height;
	else
		scroll_height = my_windows[the_window].full_size.scroll_height;


	for(row = 0 ; row < scroll_height ; row++)
	{
		temp_ptr = my_windows[the_window].text_array[row];

		for(i = 0 ; (i < 83) && (*(temp_ptr + i) != '\0') ; i++)
			if(*(temp_ptr + i) != ' ')
				return('f');

	}


	return('t');
}




int  return_window_color(the_window,which_color)
int the_window,which_color;
{

	if(my_windows[the_window].is_retracted == 't')
	{
		switch(which_color)
		{
			case FOREGROUND :
				return(my_windows[the_window].retracted.
					foreground);

			case BACKGROUND :
				return(my_windows[the_window].retracted.
					background);

			case SHADOW :
				return(my_windows[the_window].retracted.
					shadow);

			case BORDER :
				return(my_windows[the_window].retracted.
					border);

			default :
				return(-1);
		}

	}
	else   /* its NOT retracted */
	{
		switch(which_color)
		{
			case FOREGROUND :
				return(my_windows[the_window].full_size.
					foreground);

			case BACKGROUND :
				return(my_windows[the_window].full_size.
					background);

			case SHADOW :
				return(my_windows[the_window].full_size.
					shadow);

			case BORDER :
				return(my_windows[the_window].full_size.
					border);

			default :
				return(-1);
		}

	}

}



void  update_the_windows()
{
int i;
char display_lights;


	if(read_more_flag == 't')
		return;


	if(lights_wiped == 't')
	{
		lights_wiped = 'f';
		return;
	}


	display_pop_up_window(MAIN_MENU);


	display_lights = 'f';

	for(i = 0 ; i < 32 ; i++)
		if(light_lines[i].light_message[0] != '\0')
		{
			if(light_lines[i].is_on != light_is_on[i])
			{
				display_lights = 't';
				break;
			}
		} 


	if(display_lights == 't')
		display_pop_up_window(LIGHTS);


	for(i = 0 ; i < 32 ; i++)
		light_is_on[i] = light_lines[i].is_on;

	
	if(my_windows[CALCULATOR].was_updated == 't')
	{
		my_windows[CALCULATOR].was_updated = 'f';
		display_pop_up_window(CALCULATOR);
	}


	if(my_windows[PART_INFO].was_updated == 't')
	{
		my_windows[PART_INFO].was_updated = 'f';
		display_pop_up_window(PART_INFO);
	}


	restore_graphics_viewport();
}






extern int  run_vt_flag;

short  vt100_dl[4000];



void  run_vt()
{
int x1,x2,y1,y2,row,i;
int the_window,the_color;
int text_size,scroll_height,shadow;
char bottom_clipped;

/*
	if(pw_dl_busy() == TRUE)
		return;
*/
	pw_dl_wait();

	scroll_height = my_windows[VT100_WINDOW].full_size.scroll_height;
	text_size = my_windows[VT100_WINDOW].full_size.text_size;
	shadow = my_windows[VT100_WINDOW].full_size.shadow;


	run_vt_flag = 0;
	the_window = VT100_WINDOW;

	which_page = !which_page;


/* VERY IMPORTANT to make wrmask in sub 1,2,5  agree with this page switch */
	set_sub_wrmasks();



/* MAKE D L */
	pw_dl_init(NULL,0,NULL,0);

/*
	pw_dl_init(vt100_dl,0,NULL,0);
*/
	gtsize((short)text_size,(short)text_size);
	wrmask(io_masks[which_page]);
	linpat(0);

/* window/viewport */
	x1 = my_windows[the_window].full_size.screen_col;
	y2 = my_windows[the_window].full_size.screen_row;

	x2 = x1 + my_windows[the_window].full_size.screen_width;
	y1 = y2 - my_windows[the_window].full_size.screen_height;


	x1 -= SHADOW_SIZE;
	y2 += SHADOW_SIZE;


	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
	{
		if(x2 > 1279)
			x2 = 1279;
	}


	if(y1 <= 0)
	{
		bottom_clipped = 't';
		y1 = 0;
	}
	else
		bottom_clipped = 'f';


	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
	{
		if(y2 > 1023)
			y2 = 1023;
	}


	window((short)x1,(short)x2,(short)y1,(short)y2);	
	viewport((short)(x1 + SHADOW_SIZE),(short)x2,(short)y1,
		(short)(y2 - SHADOW_SIZE));
	

/* flood window with its background color */
	the_color = my_windows[the_window].full_size.background;
	flood((short)((the_color << 4) | the_color));

	viewport((short)x1,(short)x2,(short)y1,(short)y2);


	if(shadow >= 0)
	{
	/* shadow code */
		color((short)((shadow << 4) | shadow));

		if(bottom_clipped == 'f')
		{
		/*vertical bar */
			move2((short)x1,(short)(y1 + SHADOW_SIZE));
			box((short)(x1 + SHADOW_SIZE - 1),(short)y2);
		}
		else
		{
		/*vertical bar */
			move2((short)x1,(short)y1);
			box((short)(x1 + SHADOW_SIZE - 1),(short)y2);
		}


	/*horizontal bar*/
		move2((short)x1,(short)(y2 - SHADOW_SIZE + 1));
		box((short)(x2 - SHADOW_SIZE),(short)y2);



	/* fill in corners of shadow with global_screen_color */
		color((short)((global_screen_color << 4) | global_screen_color));
		if(bottom_clipped == 'f')
		{
			move2((short)x1,(short)y1);
			box((short)(x1 + SHADOW_SIZE),(short)(y1 + SHADOW_SIZE));
		}

		move2((short)(x2 - SHADOW_SIZE - 1),(short)(y2 - SHADOW_SIZE - 1));
		box((short)x2,(short)y2);

	}


	if(my_windows[the_window].full_size.border >= 0)
	{
	/* border */
		the_color = my_windows[the_window].full_size.border;
		color((short)((the_color << 4) | the_color));

		move2((short)(x1 + SHADOW_SIZE),(short)y1);
		draw2((short)(x1 + SHADOW_SIZE),(short)(y2 - SHADOW_SIZE));
		draw2((short)x2,(short)(y2 - SHADOW_SIZE));
		draw2((short)x2,(short)y1);
		draw2((short)(x1 + SHADOW_SIZE),(short)y1);
	}


/* text */
	the_color = my_windows[the_window].full_size.foreground;
	color((short)((the_color << 4) | the_color));


	for(row = 0 ; row <= scroll_height ; row++)
	{
		x1 = my_windows[the_window].full_size.screen_col + text_size;
		y1 = my_windows[the_window].full_size.screen_row -
			text_size*(row + 3)*2;

		move2((short)x1,(short)y1);
		gtext(my_windows[the_window].text_array[row]);
	}


	rdmask(io_masks[which_page]);
	linpat(1);
	my_size_chars(list_size);

/*
	dlend();

	pw_dl_init(NULL,0,NULL,0);


	dlrun(vt100_dl);
*/

}






void  clear_pop_up_window(the_window,the_color,the_position)
int the_window,the_color,the_position;
{
int x1,x2,y1,y2;
int row,shadow;


	if(my_windows[the_window].is_visible == 'f')
		return;

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	if(the_position == EXPANDED_POS)
	{
	/* viewport/window */
		x1 = my_windows[the_window].full_size.screen_col;
		y2 = my_windows[the_window].full_size.screen_row;

		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = y2 - my_windows[the_window].full_size.screen_height;
		shadow = my_windows[the_window].full_size.shadow;
	}
	else
	{
	/* viewport/window */
		x1 = my_windows[the_window].retracted.screen_col;
		y2 = my_windows[the_window].retracted.screen_row;

		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = y2 - my_windows[the_window].retracted.screen_height;
		shadow = my_windows[the_window].retracted.shadow;
	}


	if(shadow >= 0)
	{
		x1 -= SHADOW_SIZE;
		y2 += SHADOW_SIZE;
	}

	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
	{
		if(x2 > 1279)
			x2 = 1279;
	}

	if(y1 < 0)
		y1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
	{
		if(y2 > 1023)
			y2 = 1023;
	}


	if(y2 < 0)
		return;


	viewport((short)x1,(short)x2,(short)y1,(short)y2);

	wrmask(0xff);
	flood((short)((the_color << 4) | the_color));
	wrmask(io_masks[which_page]);
}







void  display_pop_up_window(the_window)
int the_window;
{
int x1,x2,y1,y2;
int foreground,background,row;
int text_size,scroll_height,shadow,border;
char bottom_clipped;


	if(read_more_flag == 't')
		return;

	if(my_windows[the_window].is_visible == 'f')
		return;

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	change_page_gmark_undraw();


	linpat(0);


	if(in_the_home_menu == 'f')
	{
		wrmask(0xff);
	}
	else
		wrmask(io_masks[which_page]);


/* viewport/window , etc */
	if(my_windows[the_window].is_retracted == 'f')
	{
		x1 = my_windows[the_window].full_size.screen_col;
		y2 = my_windows[the_window].full_size.screen_row;

		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = y2 - my_windows[the_window].full_size.screen_height;

		shadow = my_windows[the_window].full_size.shadow;
		foreground = my_windows[the_window].full_size.foreground;
		background = my_windows[the_window].full_size.background;
		border = my_windows[the_window].full_size.border;
		text_size = my_windows[the_window].full_size.text_size;
		scroll_height = my_windows[the_window].full_size.scroll_height;
	}
	else
	{
		x1 = my_windows[the_window].retracted.screen_col;
		y2 = my_windows[the_window].retracted.screen_row;

		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = y2 - my_windows[the_window].retracted.screen_height;

		shadow = my_windows[the_window].retracted.shadow;
		foreground = my_windows[the_window].retracted.foreground;
		background = my_windows[the_window].retracted.background;
		border = my_windows[the_window].retracted.border;
		text_size = my_windows[the_window].retracted.text_size;
		scroll_height = my_windows[the_window].retracted.scroll_height;
	}


	x1 -= SHADOW_SIZE;
	y2 += SHADOW_SIZE;


	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
	{
		if(x2 > 1279)
			x2 = 1279;
	}

	if(y1 <= 0)
	{
		bottom_clipped = 't';
		y1 = 0;
	}
	else
		bottom_clipped = 'f';


	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
	{
		if(y2 > 1023)
			y2 = 1023;
	}


	if(y2 < 0)
		goto my_return;



	window((short)x1,(short)x2,(short)y1,(short)y2);
	viewport((short)(x1 + SHADOW_SIZE),(short)x2,(short)y1,
		(short)(y2 - SHADOW_SIZE));
	flood((short)((background << 4) | background));
	viewport((short)x1,(short)x2,(short)y1,(short)y2);


	gtsize((short)text_size,(short)text_size);

	if(shadow >= 0)
	{
	/* shadow code */
		color((short)((shadow << 4) | shadow));

		if(bottom_clipped == 'f')
		{
		/*vertical bar */
			move2((short)x1,(short)(y1 + SHADOW_SIZE));
			box((short)(x1 + SHADOW_SIZE - 1),(short)y2);
		}
		else
		{
		/*vertical bar */
			move2((short)x1,(short)y1);
			box((short)(x1 + SHADOW_SIZE - 1),(short)y2);
		}


	/*horizontal bar*/
		move2((short)x1,(short)(y2 - SHADOW_SIZE + 1));
		box((short)(x2 - SHADOW_SIZE),(short)y2);
	}


	if(border >= 0)
	{
	/* border */
		color((short)((border << 4) | border));

		move2((short)(x1 + SHADOW_SIZE),(short)y1);
		draw2((short)(x1 + SHADOW_SIZE),(short)(y2 - SHADOW_SIZE));
		draw2((short)x2,(short)(y2 - SHADOW_SIZE));
		draw2((short)x2,(short)y1);
		draw2((short)(x1 + SHADOW_SIZE),(short)y1);
	}

/* text */
	color((short)((foreground << 4) | foreground));

	for(row = 0 ; row < scroll_height ; row++)
	{
		if(my_windows[the_window].is_retracted == 'f')
		{
			x1 = my_windows[the_window].full_size.screen_col;
			y1 = my_windows[the_window].full_size.screen_row -
				text_size*(row + 1)*5/4;
		}
		else
		{
			x1 = my_windows[the_window].retracted.screen_col;
			y1 = my_windows[the_window].retracted.screen_row -
				text_size*(row + 1)*5/4;
		}

		move2((short)(x1 + text_size),(short)(y1 - text_size));
		gtext(my_windows[the_window].text_array[row]);
	}



my_return:

	wrmask(io_masks[which_page]);
	linpat(1);
	my_size_chars(list_size);



	change_page_gmark_undraw();



	if(the_window == MAIN_MENU)
	{
		if(picked_lights == 'f')
		{
			if(is_inside_menu(MAIN_MENU,new_y,new_x) == 't')
			{
				if(row_is_lit == 't')
					xor_lighted_bar(MAIN_MENU,last_row_lit);
			}	
		}
		else
		{
			move_pick_to_main_menu();
			picked_lights = 'f';
		}
	}
}





void  no_gmark_display_window(the_window)
int the_window;
{
int x1,x2,y1,y2;
int foreground,background,row;
int text_size,scroll_height,shadow,border;
char bottom_clipped;


	if(read_more_flag == 't')
		return;

	if(my_windows[the_window].is_visible == 'f')
		return;

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


	linpat(0);


	if(in_the_home_menu == 'f')
	{
		wrmask(0xff);
	}
	else
		wrmask(io_masks[which_page]);


/* viewport/window , etc */
	if(my_windows[the_window].is_retracted == 'f')
	{
		x1 = my_windows[the_window].full_size.screen_col;
		y2 = my_windows[the_window].full_size.screen_row;

		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = y2 - my_windows[the_window].full_size.screen_height;

		shadow = my_windows[the_window].full_size.shadow;
		foreground = my_windows[the_window].full_size.foreground;
		background = my_windows[the_window].full_size.background;
		border = my_windows[the_window].full_size.border;
		text_size = my_windows[the_window].full_size.text_size;
		scroll_height = my_windows[the_window].full_size.scroll_height;
	}
	else
	{
		x1 = my_windows[the_window].retracted.screen_col;
		y2 = my_windows[the_window].retracted.screen_row;

		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = y2 - my_windows[the_window].retracted.screen_height;

		shadow = my_windows[the_window].retracted.shadow;
		foreground = my_windows[the_window].retracted.foreground;
		background = my_windows[the_window].retracted.background;
		border = my_windows[the_window].retracted.border;
		text_size = my_windows[the_window].retracted.text_size;
		scroll_height = my_windows[the_window].retracted.scroll_height;
	}


	x1 -= SHADOW_SIZE;
	y2 += SHADOW_SIZE;


	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
	{
		if(x2 > 1279)
			x2 = 1279;
	}

	if(y1 <= 0)
	{
		bottom_clipped = 't';
		y1 = 0;
	}
	else
		bottom_clipped = 'f';


	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
	{
		if(y2 > 1023)
			y2 = 1023;
	}


	if(y2 < 0)
		goto my_return;



	window((short)x1,(short)x2,(short)y1,(short)y2);
	viewport((short)(x1 + SHADOW_SIZE),(short)x2,(short)y1,
		(short)(y2 - SHADOW_SIZE));
	flood((short)((background << 4) | background));
	viewport((short)x1,(short)x2,(short)y1,(short)y2);


	gtsize((short)text_size,(short)text_size);

	if(shadow >= 0)
	{
	/* shadow code */
		color((short)((shadow << 4) | shadow));

		if(bottom_clipped == 'f')
		{
		/*vertical bar */
			move2((short)x1,(short)(y1 + SHADOW_SIZE));
			box((short)(x1 + SHADOW_SIZE - 1),(short)y2);
		}
		else
		{
		/*vertical bar */
			move2((short)x1,(short)y1);
			box((short)(x1 + SHADOW_SIZE - 1),(short)y2);
		}


	/*horizontal bar*/
		move2((short)x1,(short)(y2 - SHADOW_SIZE + 1));
		box((short)(x2 - SHADOW_SIZE),(short)y2);
	}


	if(border >= 0)
	{
	/* border */
		color((short)((border << 4) | border));

		move2((short)(x1 + SHADOW_SIZE),(short)y1);
		draw2((short)(x1 + SHADOW_SIZE),(short)(y2 - SHADOW_SIZE));
		draw2((short)x2,(short)(y2 - SHADOW_SIZE));
		draw2((short)x2,(short)y1);
		draw2((short)(x1 + SHADOW_SIZE),(short)y1);
	}

/* text */
	color((short)((foreground << 4) | foreground));

	for(row = 0 ; row < scroll_height ; row++)
	{
		if(my_windows[the_window].is_retracted == 'f')
		{
			x1 = my_windows[the_window].full_size.screen_col;
			y1 = my_windows[the_window].full_size.screen_row -
				text_size*(row + 1)*5/4;
		}
		else
		{
			x1 = my_windows[the_window].retracted.screen_col;
			y1 = my_windows[the_window].retracted.screen_row -
				text_size*(row + 1)*5/4;
		}

		move2((short)(x1 + text_size),(short)(y1 - text_size));
		gtext(my_windows[the_window].text_array[row]);
	}



my_return:

	wrmask(io_masks[which_page]);
	linpat(1);
	my_size_chars(list_size);



	if(the_window == MAIN_MENU)
	{
		if(picked_lights == 'f')
		{
			if(is_inside_menu(MAIN_MENU,new_y,new_x) == 't')
			{
				if(row_is_lit == 't')
					xor_lighted_bar(MAIN_MENU,last_row_lit);
			}	
		}
		else
		{
			move_pick_to_main_menu();
			picked_lights = 'f';
		}
	}
}





#ifdef rrrrrrrrrrrrrrrr

void  insert_char_to_window(the_window,the_char,row,col)
int the_window,the_char,row,col;
{
int i,temp,scroll_height,insert_row;
char far *temp_ptr;


	insert_row = my_windows[the_window].top_row_for_insert;

	if(my_windows[the_window].is_retracted == 'f')
		scroll_height = my_windows[the_window].full_size.scroll_height;
	else
		scroll_height = my_windows[the_window].retracted.scroll_height;


	if(row + insert_row >= MAX_WINDOW_ROWS)  /* swap top and bottom */
	{
		my_windows[the_window].first_row_to_display = 0;
		my_windows[the_window].top_row_for_insert = 0;
		insert_row = 0;

		for(i = 0 ; i < scroll_height ; i++)
		{
			temp_ptr = my_windows[the_window].text_array[i];
			my_windows[the_window].text_array[i] = 
				my_windows[the_window].text_array[
					MAX_WINDOW_ROWS - scroll_height + i];
			my_windows[the_window].text_array[MAX_WINDOW_ROWS -
				scroll_height + i] = temp_ptr;
		}

	}
	else if(row + insert_row >= my_windows[the_window].top_row_for_insert
		+ my_windows[the_window].full_size.scroll_height)
	{
	/* scroll 1 line */
		my_windows[the_window].top_row_for_insert++;
		insert_row++;

		temp_ptr = my_windows[the_window].text_array[row + insert_row];

		for(i = 0 ; i < 83 ; i++)
			*(temp_ptr + i) = ' ';

		*(temp_ptr + 82) = '\0';
	}


	temp_ptr = my_windows[the_window].text_array[row + insert_row];
	*(temp_ptr + col) = the_char;
}

#endif




void  insert_line_to_window(the_window,the_line,row)
int the_window;
char far *the_line;
int row;
{
char far *temp_ptr;

	temp_ptr = my_windows[the_window].text_array[row];
	strcpy(temp_ptr,the_line);
}





void  save_window_blt(the_window)
int the_window;
{
int x1,x2,y1,y2;
int top_window,bottom_window;
int strip_height;
int temp_int,blt_logical_page,last_blt_logical_page;


/*
	if(do_not_dlrun_flag == 't')
*/
		return;


	if(my_windows[the_window].blt_status == SAVED_GRAPHICS)
	{
		sprintf(diag_string,
			"save_window_blt(%d) : window Already saved\n",
				the_window);
		error_logger(diag_string);
		return;
	}

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


	wrmask(0xff);

	my_windows[the_window].blt_status = SAVED_GRAPHICS;
	which_blt_list = the_window;
	blt_logical_page = my_windows[the_window].blt_list_start =
		get_next_free_page(BLT_LIST);

/* printf("SAVE_WINDOW_BLT() : FIRST PAGE = %d\n",blt_logical_page);
*/


	if(my_windows[the_window].is_retracted == 'f')
	{
		x1 = my_windows[the_window].full_size.screen_col;
		y2 = my_windows[the_window].full_size.screen_row;

		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = y2 - my_windows[the_window].full_size.screen_height;
	}
	else
	{
		x1 = my_windows[the_window].retracted.screen_col;
		y2 = my_windows[the_window].retracted.screen_row;

		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = y2 - my_windows[the_window].retracted.screen_height;
	}


	x1 -= SHADOW_SIZE;
	y2 += SHADOW_SIZE;



	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
	{
		if(x2 > 1279)
			x2 = 1279;
	}

	if(y1 < 0)
		y1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
	{
		if(y2 > 1023)
			y2 = 1023;
	}


	if(y2 < 0)
		return;


	top_window = y2;
	bottom_window = y1;

	if(which_high_res == RES_1024_768)
	{
		window(0,FULL_SCREEN_WIDTH - 1,0,FULL_SCREEN_HEIGHT - 1);
		viewport(0,FULL_SCREEN_WIDTH - 1,0,FULL_SCREEN_HEIGHT - 1);
	}
	else
	{
		window(0,1279,0,1023);
		viewport(0,1279,0,1023);
	}


/* do the BLT by horizontal strips to EMS */
	strip_height = MAX_BYTES_OF_BLT/(x2 - x1);
	y2 = strip_height + y1;



	while(99)
	{
/*
printf("move2(%d,%d)  bltrwind(%d,%d)   pcblt(buffer,%d)\n",x1,y1,
	x2 - x1 + 1,y2 - y1,x2 - x1 + 1);
*/

		move2((short)x1,(short)y1);
		bltrwind((short)(x2 - x1 + 1),(short)(y2 - y1));

/*
		pcblt(software_ems_buffer,(short)(x2 - x1 + 1));

		blt_map_handle(blt_logical_page,handle);
		memcpy(blt_page_frame_ptr,software_ems_buffer,16380);

		*(blt_page_frame_ptr + EMS_MARKER) = blt_logical_page;
*/
	
		pcblt(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][0]),
			(short)(x2 - x1 + 1));
		sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][EMS_MARKER] =
			blt_logical_page;
		memcpy(&my_ems_pages[blt_logical_page][0],&(sub_struct_ptr ->
			my_ems_buffer[SUB_HOLDER_BLT][0]),16384);


		if(y2 >= top_window)
		{

/* printf("\nMARK page %d as the Last page\n",blt_logical_page);
*/

/*
			*(blt_page_frame_ptr + END_OF_BLT) = TRUE;
			*(blt_page_frame_ptr + NEXT_BLT_PAGE) = 0;
*/
			my_ems_pages[blt_logical_page][END_OF_BLT] = TRUE;
			my_ems_pages[blt_logical_page][NEXT_BLT_PAGE] = 0;

			wrmask(io_masks[which_page]);

/*
			while(pw_dl_busy() == TRUE)
				;
*/
			pw_dl_wait();

			return;
		}
		else
		{
	/*
			*(blt_page_frame_ptr + END_OF_BLT) = FALSE;
	*/
			my_ems_pages[blt_logical_page][END_OF_BLT] = FALSE;

			last_blt_logical_page = blt_logical_page;
			blt_logical_page = get_next_free_page(BLT_LIST);

/*
			*(blt_page_frame_ptr + NEXT_BLT_PAGE) = 
				blt_logical_page;
*/
			my_ems_pages[last_blt_logical_page][NEXT_BLT_PAGE] =
				blt_logical_page;

/* printf("\nGo on to page %d\n",blt_logical_page);
*/
		}


		y1 += strip_height;
		y2 += strip_height;

	}


	wrmask(io_masks[which_page]);

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	my_set_viewport(-2048,-2048,2048,2048,0);
}






void  restore_window_blt(the_window)
int the_window;
{
int x1,x2,y1,y2;
int top_window,bottom_window;
int strip_height;
int temp_int,blt_logical_page,last_blt_logical_page;


/*
	if(do_not_dlrun_flag == 't')
	{
*/
		temp_int = my_windows[GRAPHICS_WINDOW].full_size.background;
		clear_pop_up_window(HOME_MENU,temp_int,EXPANDED_POS);

		return;

/*
	}
*/


	if(my_windows[the_window].blt_status == RESTORED_GRAPHICS)
	{
		sprintf(diag_string,
			"restore_window_blt(%d) : window wasn't saved first\n",
				the_window);
		error_logger(diag_string);
		return;
	}

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


	wrmask(0xff);

	my_windows[the_window].blt_status = RESTORED_GRAPHICS;
	which_blt_list = the_window;
	blt_logical_page = my_windows[the_window].blt_list_start;

/* printf("\nRESTORE_WINDOW_BLT() : First page = %d\n",blt_logical_page);
*/
	available_pages[blt_logical_page] = -1;  /* mark EMS page as Free */
	my_windows[the_window].blt_list_start = -1;    /* mark as removed */


	if(my_windows[the_window].is_retracted == 'f')
	{
		x1 = my_windows[the_window].full_size.screen_col;
		y2 = my_windows[the_window].full_size.screen_row;

		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = y2 - my_windows[the_window].full_size.screen_height;
	}
	else
	{
		x1 = my_windows[the_window].retracted.screen_col;
		y2 = my_windows[the_window].retracted.screen_row;

		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = y2 - my_windows[the_window].retracted.screen_height;
	}



	x1 -= SHADOW_SIZE;
	y2 += SHADOW_SIZE;



	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
	{
		if(x2 > 1279)
			x2 = 1279;
	}

	if(y1 < 0)
		y1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
	{
		if(y2 > 1023)
			y2 = 1023;
	}


	if(y2 < 0)
		return;


	top_window = y2;
	bottom_window = y1;


/* do the BLT by horizontal strips to EMS */

	strip_height = MAX_BYTES_OF_BLT/(x2 - x1);
	y2 = strip_height + y1;

	if(which_high_res == RES_1024_768)
	{
		window(0,FULL_SCREEN_WIDTH - 1,0,FULL_SCREEN_HEIGHT - 1);
		viewport(0,FULL_SCREEN_WIDTH - 1,0,FULL_SCREEN_HEIGHT - 1);
	}
	else
	{
		window(0,1279,0,1023);
		viewport(0,1279,0,1023);
	}


	while(99)
	{

/*
printf("move2(%d,%d)  bltrwind(%d,%d)   bltpc(buffer,%d)\n",
	x1,y1,x2 - x1 + 1,y2 - y1,x2 - x1 + 1);
*/

		move2((short)x1,(short)y1);
		bltrwind((short)(x2 - x1 + 1),(short)(y2 - y1));

/*
		blt_map_handle(blt_logical_page,handle);
		memcpy(software_ems_buffer,blt_page_frame_ptr,16380);
		bltpc(software_ems_buffer,(short)(x2 - x1 + 1));
*/

		memcpy(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][0]),
			&my_ems_pages[blt_logical_page][0],16384);
		bltpc(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][0]),
			(short)(x2 - x1 + 1));

/*
		while(pw_dl_busy() == TRUE)
			;
*/
		pw_dl_wait();

/*
		if(*(blt_page_frame_ptr + END_OF_BLT) == TRUE)
*/
		if(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT]
			[END_OF_BLT] == TRUE)
		{
			wrmask(io_masks[which_page]);
			return;
		}

/*
		blt_logical_page = *(blt_page_frame_ptr + NEXT_BLT_PAGE);
*/
		blt_logical_page = sub_struct_ptr -> my_ems_buffer[
			SUB_HOLDER_BLT][NEXT_BLT_PAGE];

	/*mark EMS page as Available */
		available_pages[blt_logical_page] = -1; 


		y1 += strip_height;
		y2 += strip_height;

	}

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


	my_set_viewport(-2048,-2048,2048,2048,0);
}




void  restore_graphics_if_needed()
{
	if(my_windows[MAIN_MENU].blt_status == RESTORED_GRAPHICS)
		return;

	
	restore_window_blt(PART_INFO);
	my_windows[PART_INFO].is_retracted = 't';
	display_pop_up_window(PART_INFO);

	change_page_gmark_undraw();


	restore_window_blt(MAIN_MENU);
	my_windows[MAIN_MENU].is_retracted = 't';
	display_pop_up_window(MAIN_MENU);

	change_page_gmark_undraw();

}







void  toggle_retraction(the_window)
int the_window;
{
char inside_main_menu;
int main_menu_row;

/*
	if(do_not_dlrun_flag == 't')
*/
		return;

	if(vt100_mess_mon_flag == VT100_MODE)
		return;

	if(is_a_seg_open == 't')
		return;

	if(count1 != 0)
		return;


/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	picked_lights = 'f';
	inside_main_menu = is_inside_menu(MAIN_MENU,new_y,new_x);

/* find row at old main menu position before moving the window */
	main_menu_row = find_which_row(MAIN_MENU);


	if(my_windows[the_window].blt_status == RESTORED_GRAPHICS)
	{
		change_page_gmark_undraw();


		my_windows[MAIN_MENU].is_retracted = 'f';
		save_window_blt(MAIN_MENU);
		clear_pop_up_window(MAIN_MENU,global_screen_color,
			RETRACTED_POS);
		no_gmark_display_window(MAIN_MENU);


		if(inside_main_menu == 't')
		{
			if(numeric_value(main_menu_row) == 't')
			{
				xor_lighted_bar(MAIN_MENU,last_row_lit);
				move_mouse_to_new_pos();
			}
		}

		change_page_gmark_undraw();


		my_windows[PART_INFO].is_retracted = 'f';
		save_window_blt(PART_INFO);
		clear_pop_up_window(PART_INFO,global_screen_color,
			RETRACTED_POS);
		display_pop_up_window(PART_INFO);
	}
	else
	{
		change_page_gmark_undraw();


		restore_window_blt(MAIN_MENU);
		my_windows[MAIN_MENU].is_retracted = 't';
		no_gmark_display_window(MAIN_MENU);


		if(inside_main_menu == 't')
		{
			if(numeric_value(main_menu_row) == 't')
			{
				xor_lighted_bar(MAIN_MENU,last_row_lit);
				move_mouse_to_new_pos();
			}
		}

		change_page_gmark_undraw();


		restore_window_blt(PART_INFO);
		my_windows[PART_INFO].is_retracted = 't';
		display_pop_up_window(PART_INFO);
	}

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();
}






/* WINDOW CONFIG FILE READING PARTS  config */




#ifdef kleeklkekk

void far window_error()
{
	printf("\n\nFATAL ERROR  WINDOWS.CFG  Only Has %d Values Out Of %d\n",
		number_of_values_found,TOTAL_NUMBER_TO_READ);
	printf("\n\nCAN NOT CONTINUE\n\n");


	exit_dealloc_pages(handle);
	close_host();
	restore_int9();

	exit(0);
}




int far read_white_space()
{
int ch;

	for(ch = getc(window_config_file) ;
		((ch == ' ') || (ch == '\t') || (ch == 10) || (ch == 13)) 
		  &&  (ch != EOF) ;
			ch = getc(window_config_file))
				;

	return(ch);
}







int far get_next_config_value()
{
int ch,i;


	number_of_values_found++;


try_again:
	ch = read_white_space();
	if(ch == EOF)
		return(MY_EOF);



	if(ch == '$')  /* config comment */
	{
		for(ch = getc(window_config_file) ; (ch != 10) && (ch != 13) &&
			(ch != EOF) ; ch = getc(window_config_file))
				;

		if(ch == EOF)
			return(MY_EOF);


		goto try_again;

	}
	else   /* read config value */ 
	{
	/* read till colon */
		for(ch = getc(window_config_file) ; (ch != ':') && (ch != EOF)
			; ch = getc(window_config_file))
				;

		if(ch == EOF)
			return(MY_EOF);


		ch = read_white_space();
		window_temp_string[0] = ch;

	/* read the value */
		for(i = 1, ch = getc(window_config_file) ; (ch != EOF) &&
		    (ch != ' ') && (ch != '\t') && (ch != 10) && (ch != 13) ;
			i++, ch = getc(window_config_file))
				window_temp_string[i] = ch;


		if(ch == EOF)
			return(MY_EOF);


		window_temp_string[i] = '\0';

		return(atoi(window_temp_string));		
	}


}
#endif


void far scale_up_window_data()
{
int i;

	for(i = MAIN_MENU ; i <= HOME_MENU ; i++)
	{
		my_windows[i].full_size.screen_col = 
			5*my_windows[i].full_size.screen_col/4;
		my_windows[i].full_size.screen_width =
			5*my_windows[i].full_size.screen_width/4;
		my_windows[i].full_size.screen_row =
			4*my_windows[i].full_size.screen_row/3;
		my_windows[i].full_size.screen_height =
			4*my_windows[i].full_size.screen_height/3;


		my_windows[i].retracted.screen_col = 
			5*my_windows[i].retracted.screen_col/4;
		my_windows[i].retracted.screen_width =
			5*my_windows[i].retracted.screen_width/4;
		my_windows[i].retracted.screen_row =
			4*my_windows[i].retracted.screen_row/3;
		my_windows[i].retracted.screen_height =
			4*my_windows[i].retracted.screen_height/3;
	}
}


void far default_window_parameters()
{
	global_screen_color = 9;

	my_windows[MAIN_MENU].full_size.text_size = 10;
	my_windows[MAIN_MENU].full_size.screen_col = 100;
	my_windows[MAIN_MENU].full_size.screen_width = 380;
	my_windows[MAIN_MENU].full_size.screen_row = 600;
	my_windows[MAIN_MENU].full_size.screen_height = 260;
	my_windows[MAIN_MENU].full_size.background = 3;
	my_windows[MAIN_MENU].full_size.foreground = 15;
	my_windows[MAIN_MENU].full_size.shadow = 0;
	my_windows[MAIN_MENU].full_size.border = 15;

	my_windows[MAIN_MENU].retracted.text_size = 10;
	my_windows[MAIN_MENU].retracted.screen_col = 790;
	my_windows[MAIN_MENU].retracted.screen_width = 210;
	my_windows[MAIN_MENU].retracted.screen_row = 750;
	my_windows[MAIN_MENU].retracted.screen_height = 260;
	my_windows[MAIN_MENU].retracted.background = 3;
	my_windows[MAIN_MENU].retracted.foreground = 15;
	my_windows[MAIN_MENU].retracted.shadow = 0;
	my_windows[MAIN_MENU].retracted.border = 15;


	my_windows[LIGHTS].full_size.text_size = 10;
	my_windows[LIGHTS].full_size.screen_col = 790;
	my_windows[LIGHTS].full_size.screen_width = 210;
	my_windows[LIGHTS].full_size.screen_row = 473;
	my_windows[LIGHTS].full_size.screen_height = 275;
	my_windows[LIGHTS].full_size.background = 3;
	my_windows[LIGHTS].full_size.foreground = 15;
	my_windows[LIGHTS].full_size.shadow = 0;
	my_windows[LIGHTS].full_size.border = 15;

	my_windows[LIGHTS].retracted.text_size = 10;
	my_windows[LIGHTS].retracted.screen_col = 790;
	my_windows[LIGHTS].retracted.screen_width = 210;
	my_windows[LIGHTS].retracted.screen_row = 473;
	my_windows[LIGHTS].retracted.screen_height = 275;
	my_windows[LIGHTS].retracted.background = 3;
	my_windows[LIGHTS].retracted.foreground = 15;
	my_windows[LIGHTS].retracted.shadow = 0;
	my_windows[LIGHTS].retracted.border = 15;


	my_windows[CALCULATOR].full_size.text_size = 10;
	my_windows[CALCULATOR].full_size.screen_col = 790;
	my_windows[CALCULATOR].full_size.screen_width = 210;
	my_windows[CALCULATOR].full_size.screen_row = 181;
	my_windows[CALCULATOR].full_size.screen_height = 175;
	my_windows[CALCULATOR].full_size.background = 3;
	my_windows[CALCULATOR].full_size.foreground = 15;
	my_windows[CALCULATOR].full_size.shadow = 0;
	my_windows[CALCULATOR].full_size.border = 15;

	my_windows[CALCULATOR].retracted.text_size = 10;
	my_windows[CALCULATOR].retracted.screen_col = 790;
	my_windows[CALCULATOR].retracted.screen_width = 210;
	my_windows[CALCULATOR].retracted.screen_row = 181;
	my_windows[CALCULATOR].retracted.screen_height = 175;
	my_windows[CALCULATOR].retracted.background = 3;
	my_windows[CALCULATOR].retracted.foreground = 15;
	my_windows[CALCULATOR].retracted.shadow = 0;
	my_windows[CALCULATOR].retracted.border = 15;


	my_windows[PART_INFO].full_size.text_size = 10;
	my_windows[PART_INFO].full_size.screen_col = 100;
	my_windows[PART_INFO].full_size.screen_width = 600;
	my_windows[PART_INFO].full_size.screen_row = 80;
	my_windows[PART_INFO].full_size.screen_height = 82;
	my_windows[PART_INFO].full_size.background = 3;
	my_windows[PART_INFO].full_size.foreground = 15;
	my_windows[PART_INFO].full_size.shadow = 0;
	my_windows[PART_INFO].full_size.border = 15;

	my_windows[PART_INFO].retracted.text_size = 10;
	my_windows[PART_INFO].retracted.screen_col = 100;
	my_windows[PART_INFO].retracted.screen_width = 600;
	my_windows[PART_INFO].retracted.screen_row = -33;
	my_windows[PART_INFO].retracted.screen_height = 12;
	my_windows[PART_INFO].retracted.background = 3;
	my_windows[PART_INFO].retracted.foreground = 15;
	my_windows[PART_INFO].retracted.shadow = 0;
	my_windows[PART_INFO].retracted.border = 15;


	my_windows[GRAPHICS_WINDOW].full_size.text_size = 10;
	my_windows[GRAPHICS_WINDOW].full_size.screen_col = 10;
	my_windows[GRAPHICS_WINDOW].full_size.screen_width = 760;
	my_windows[GRAPHICS_WINDOW].full_size.screen_row = 5;
	my_windows[GRAPHICS_WINDOW].full_size.background = 0;
	my_windows[GRAPHICS_WINDOW].full_size.foreground = 0;
	my_windows[GRAPHICS_WINDOW].full_size.shadow = 9;
	my_windows[GRAPHICS_WINDOW].full_size.border = 9;

	my_windows[GRAPHICS_WINDOW].retracted.text_size = 10;


	my_windows[VT100_WINDOW].full_size.text_size = 10;
	my_windows[VT100_WINDOW].full_size.screen_col = 100;
	my_windows[VT100_WINDOW].full_size.screen_width = 700;
	my_windows[VT100_WINDOW].full_size.screen_row = 700;
	my_windows[VT100_WINDOW].full_size.screen_height = 600;
	my_windows[VT100_WINDOW].full_size.background = 1;
	my_windows[VT100_WINDOW].full_size.foreground = 15;
	my_windows[VT100_WINDOW].full_size.shadow = 0;
	my_windows[VT100_WINDOW].full_size.border = 15;

	my_windows[VT100_WINDOW].retracted.text_size = 10;


	my_windows[HOME_MENU].full_size.text_size = 10;
	my_windows[HOME_MENU].full_size.screen_col = 20;
	my_windows[HOME_MENU].full_size.screen_width = 400;
	my_windows[HOME_MENU].full_size.screen_row = 700;
	my_windows[HOME_MENU].full_size.screen_height = 200;
	my_windows[HOME_MENU].full_size.background = 3;
	my_windows[HOME_MENU].full_size.foreground = 15;
	my_windows[HOME_MENU].full_size.shadow = 1;
	my_windows[HOME_MENU].full_size.border = 15;

	my_windows[HOME_MENU].retracted.text_size = 10;
}





void far read_window_config()
{
int window_number,row,a_value;
int int_temp;

	color_reg(0,0,0,0);
	color_reg(1,6 << 2,6 << 2,6 << 2);
	color_reg(2,4 << 2,8 << 2,2 << 2);
	color_reg(3,0 << 2,0 << 2,15 << 2);
	color_reg(4,15 << 2,6 << 2,6 << 2);
	color_reg(5,0 << 2,15 << 2,0 << 2);
	color_reg(6,8 << 2,4 << 2,4 << 2);
	color_reg(7,0 << 2,15 << 2,15 << 2);
	color_reg(8,15 << 2,8 << 2,0 << 2);
	color_reg(9,15 << 2,0 << 2,0 << 2);
	color_reg(10,7 << 2,0 << 2,8 << 2);
	color_reg(11,15 << 2,0 << 2,15 << 2);
	color_reg(12,9 << 2,0 << 2,0 << 2);
	color_reg(13,15 << 2,15 << 2,0 << 2);
	color_reg(14,3 << 2,10 << 2,7 << 2);
	color_reg(15,15 << 2,15 << 2,15 << 2);


	default_window_parameters();

	if(which_high_res == RES_1280_1024)
		scale_up_window_data();

#ifdef kkkkkkkkkkkkkkkkkk
	number_of_values_found = 0;

	window_config_file = fopen(window_config_name,"r");



	if(window_config_file == NULL)
	{
		printf("\n\nUnable To Open  WINDOWS.CFG  File\n\n");
		printf("Can't Continue\n\n\n");

		exit_dealloc_pages(handle);
		close_host();
		restore_int9();

		exit(0);
	}





	a_value = get_next_config_value();
	if(a_value == MY_EOF)
		window_error();

	if(a_value >= 0)
		global_screen_color = a_value;
	else
		global_screen_color = 0;

#endif


	for(window_number = 0 ; window_number < NUMBER_OF_WINDOWS ;
		window_number++)
	{
		for(row = 0 ; row < MAX_WINDOW_ROWS ; row++)
		{
			my_windows[window_number].text_array[row] = 
				malloc(90);

			for(a_value = 0 ; a_value < 83 ; a_value++)
				*(my_windows[window_number].
					text_array[row] + a_value) = ' ';

			*(my_windows[window_number].text_array[row] + 82) = 
				'\0';
		}

#ifdef rrrrrrrrrrrr
		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value > 5)
			my_windows[window_number].full_size.text_size = a_value;
		else
			my_windows[window_number].full_size.text_size = 
				DEFAULT_TEXT_SIZE;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].full_size.screen_col = a_value;
		else
			my_windows[window_number].full_size.screen_col = 0;

		if(which_high_res == RES_1280_1024)
			my_windows[window_number].full_size.screen_col =
				5*my_windows[window_number].full_size.screen_col
					/4;

		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 8)
			my_windows[window_number].full_size.screen_width =
				 a_value;
		else
			my_windows[window_number].full_size.screen_width = 8;

		if(which_high_res == RES_1280_1024)
			my_windows[window_number].full_size.screen_width =
				5*my_windows[window_number].full_size.screen_width
					/4;

		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].full_size.screen_row = a_value;
		else
			my_windows[window_number].full_size.screen_row = 0;

		if(which_high_res == RES_1280_1024)
			my_windows[window_number].full_size.screen_row =
				4*my_windows[window_number].full_size.screen_row
					/3;

		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 5)
			my_windows[window_number].full_size.screen_height =
				 a_value;
		else
			my_windows[window_number].full_size.screen_height = 5;


		if(which_high_res == RES_1280_1024)
			my_windows[window_number].full_size.screen_height = 
				4*my_windows[window_number].full_size.screen_height
					/3;

		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].full_size.background = a_value;
		else
			my_windows[window_number].full_size.background = 0;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].full_size.foreground = a_value;
		else
			my_windows[window_number].full_size.foreground = 2;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= -1)
			my_windows[window_number].full_size.shadow = a_value;
		else
			my_windows[window_number].full_size.shadow = -1;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].full_size.border = a_value;
		else
			my_windows[window_number].full_size.border = 4;



		my_windows[window_number].full_size.row_offset = 0;
		my_windows[window_number].full_size.col_offset = 0;
#endif
	}



	actual_size = (float)my_windows[GRAPHICS_WINDOW].full_size.screen_width;


/* Retracted initial values set up */


#ifdef jkhgffrtyyhhgvgfff
	for(window_number = 0 ; window_number < NUMBER_OF_WINDOWS ;
		window_number++)
	{
		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value > 5)
			my_windows[window_number].retracted.text_size = a_value;
		else
			my_windows[window_number].retracted.text_size = 
				DEFAULT_TEXT_SIZE;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 10)
			my_windows[window_number].retracted.screen_col = a_value;
		else
			my_windows[window_number].retracted.screen_col = 10;

		if(which_high_res == RES_1280_1024)
			my_windows[window_number].retracted.screen_col = 
				5*my_windows[window_number].retracted.screen_col
					/4;

		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 8)
			my_windows[window_number].retracted.screen_width =
				 a_value;
		else
			my_windows[window_number].retracted.screen_width = 8;

		if(which_high_res == RES_1280_1024)
			my_windows[window_number].retracted.screen_width =
				5*my_windows[window_number].retracted.screen_width
					/4;

		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

/*
		if(a_value > 0)
*/

		my_windows[window_number].retracted.screen_row = a_value;

/*
		else
			my_windows[window_number].retracted.screen_row = 1;
*/


		if(which_high_res == RES_1280_1024)
			my_windows[window_number].retracted.screen_row = 
				4*my_windows[window_number].retracted.screen_row
					/3;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 5)
			my_windows[window_number].retracted.screen_height =
				 a_value;
		else
			my_windows[window_number].retracted.screen_height = 5;

		if(which_high_res == RES_1280_1024)
			my_windows[window_number].retracted.screen_height = 
				4*my_windows[window_number].retracted.screen_height
					/3;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].retracted.background = a_value;
		else
			my_windows[window_number].retracted.background = 0;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].retracted.foreground = a_value;
		else
			my_windows[window_number].retracted.foreground = 2;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= -1)
			my_windows[window_number].retracted.shadow = a_value;
		else
			my_windows[window_number].retracted.shadow = -1;


		a_value = get_next_config_value();
		if(a_value == MY_EOF)
			window_error();

		if(a_value >= 0)
			my_windows[window_number].retracted.border = a_value;
		else
			my_windows[window_number].retracted.border = 4;



		my_windows[window_number].retracted.row_offset = 0;
		my_windows[window_number].retracted.col_offset = 0;
	}
#endif

	my_windows[MAIN_MENU].can_be_retracted = 't';
	my_windows[LIGHTS].can_be_retracted = 't';
	my_windows[CALCULATOR].can_be_retracted = 't';
	my_windows[PART_INFO].can_be_retracted = 't';
	my_windows[GRAPHICS_WINDOW].can_be_retracted = 'f';
	my_windows[VT100_WINDOW].can_be_retracted = 'f';
	my_windows[HOME_MENU].can_be_retracted = 't';



	for(window_number = 0 ; window_number < NUMBER_OF_WINDOWS ;
		window_number++)
	{
		my_windows[window_number].first_row_to_display = 0;
		my_windows[window_number].top_row_for_insert = 0;

		my_windows[window_number].blt_list_start = -1;
		my_windows[window_number].blt_status = RESTORED_GRAPHICS;
		my_windows[window_number].was_updated = 'f';
		my_windows[window_number].is_visible = 't';
		my_windows[window_number].is_retracted = 'f';


/*
		my_windows[window_number].retracted.text_size =
			my_windows[window_number].full_size.text_size;
*/

		if(window_number == VT100_WINDOW)
		{
			my_windows[window_number].full_size.scroll_height = 
				my_windows[window_number].full_size.screen_height/
				(my_windows[window_number].full_size.text_size)
					*2/5;

			my_windows[window_number].retracted.scroll_height =
				my_windows[window_number].retracted.screen_height/
				(my_windows[window_number].retracted.text_size)
					*2/5;
		}
		else
		{
			my_windows[window_number].full_size.scroll_height = 
				my_windows[window_number].full_size.screen_height/
				(my_windows[window_number].full_size.text_size)
					*4/5;

			my_windows[window_number].retracted.scroll_height =
				my_windows[window_number].retracted.screen_height/
				(my_windows[window_number].retracted.text_size)
					*4/5;
		}

	}

}

