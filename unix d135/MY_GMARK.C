
/*  MY_GMARK.C  */



#include <string.h>
#include "my_globs.h"
#include "mywindows.h"
#include <pw_lib.h>


#define GMARK_COLOR 0xaa

#define RES_1024_768 3
#define RES_1280_1024 4



#define VT100_MODE  0
#define MESS_MON  1


extern int  vt100_mess_mon_flag;
extern char  which_high_res;
extern int  gmark_color,  xor_bar_color;
extern int  mouse_x_max;
extern char  digitizer_enabled;
extern char  use_the_mouse;
extern int  digitizer_x,  digitizer_y,  digitizer_buttons;
extern struct a_window  my_windows[];
extern OPCODE io_masks[];
extern int  graphics_window_x1,  graphics_window_x2,
	  graphics_window_y1,  graphics_window_y2;



extern void  toggle_retraction();

extern void  draw_mode();
extern void  my_send_ext();

void  my_read_digitizer();
char  reverse_the_buttons();
void  read_mouse();



char  picked_lights = 'f';
char  row_is_lit = 'f';
int  gmark_row_text = 0,  last_row_lit = -1;
int  last_old_x = 0,  last_old_y = 0;



char  is_inside_menu(the_window,row,col)
int the_window,row,col;
{

	if(my_windows[the_window].is_retracted == 't')
	{
		if((col > my_windows[the_window].retracted.screen_col) &&
		   (col < my_windows[the_window].retracted.screen_col +
			my_windows[the_window].retracted.screen_width) &&
		   (row < my_windows[the_window].retracted.screen_row) &&
		   (row > my_windows[the_window].retracted.screen_row -
		   	my_windows[the_window].retracted.screen_height))
		{
			return('t');
		}
		else
			return('f');
	}
	else
	{
		if((col > my_windows[the_window].full_size.screen_col) &&
		   (col < my_windows[the_window].full_size.screen_col +
			my_windows[the_window].full_size.screen_width) &&
		   (row < my_windows[the_window].full_size.screen_row) &&
		   (row > my_windows[the_window].full_size.screen_row -
		   	my_windows[the_window].full_size.screen_height))
		{
			return('t');
		}
		else
			return('f');
	}

}



void  set_window_viewport(the_window)
int the_window;
{
int x1,x2,y1,y2;
int row;


	if(my_windows[the_window].is_visible == 'f')
		return;

/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();

	if(my_windows[the_window].is_retracted == 'f')
	{
	/* viewport/window */
		x1 = my_windows[the_window].full_size.screen_col;
		y2 = my_windows[the_window].full_size.screen_row;

		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = y2 - my_windows[the_window].full_size.screen_height;
	}
	else
	{
	/* viewport/window */
		x1 = my_windows[the_window].retracted.screen_col;
		y2 = my_windows[the_window].retracted.screen_row;

		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = y2 - my_windows[the_window].retracted.screen_height;
	}




	if(x1 < 0)
		x1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(x2 > 1023)
			x2 = 1023;
	}
	else
		if(x2 > 1279)
			x2 = 1279;

	if(y1 < 0)
		y1 = 0;

	if(which_high_res == RES_1024_768)
	{
		if(y2 > 767)
			y2 = 767;
	}
	else
		if(y2 > 1023)
			y2 = 1023;

	if(y2 < 0)
		return;


	viewport((short)x1,(short)x2,(short)y1,(short)y2);
	window((short)x1,(short)x2,(short)y1,(short)y2);

}




void  send_pick_to_host(the_window,the_row)
int the_window,the_row;
{
char  *temp_ptr,  * field_ptr, temp_buffer[150];
int i;


	if(the_window == MAIN_MENU)
	{
		temp_ptr = my_windows[MAIN_MENU].text_array[the_row];
		strcpy(temp_buffer,temp_ptr);
		field_ptr = strtok(temp_buffer," \t\n\r");

		i = atoi(field_ptr) + 35;
		my_send_ext(i);
	}
	else
		switch(the_row + 1)
		{
			case 1 : /* blank/unblank */
			{
				my_send_ext(50);
				break;
			}

			case 2 :  /* calculator */
			{
				my_send_ext(51);
				break;
			}

			case 3 : /* display control */
			{
				my_send_ext(58);
				break;
			}

			case 4 : /* edit work view */
			{
				my_send_ext(59);
				break;
			}

			case 5 : /* file terminate */
			{
				my_send_ext(34);
				break;
			}

			case 6 : /* layor control */
			{
				my_send_ext(57);
				break;
			}

			case 7 : /* macros */
			{
				my_send_ext(32);
				break;
			}

			case 8 : /* redisplay */
			{
				my_send_ext(35);
				break;
			}

			case 9 : /* system params */
			{
				my_send_ext(53);
				break;
			}

			case 10 : /* wcs control */
			{
				my_send_ext(54);
				break;
			}

			case 11 : /* zoom/pan/regen*/
			{
				my_send_ext(56);
				break;
			}

			case 13 : /* 1 change module */
			{
				my_send_ext(33);
				break;
			}

			case 14 : /* 2 module function */
			{
				my_send_ext(55);
				break;
			}

			case 15 : /* 3 module params */
			{
				my_send_ext(52);
				break;
			}

			case 17 : /* alt action */
			{
				my_send_ext(63);
				break;
			}

			case 18 : /* entry complete */
			{
				my_send_ext(60);
				break;
			}

			case 19 : /* reject */
			{
				my_send_ext(61);
				break;
			}

			case 20 : /* terminate */
			{
				my_send_ext(62);
				break;
			}
		}

}



void  xor_lighted_bar(the_window,the_row)
int the_window,the_row;
{
int x1,x2,y1,y2;
int text_size;

	if(the_row < 0)
		return;


	set_window_viewport(the_window);

	if(my_windows[the_window].is_retracted == 'f')
	{
		text_size = my_windows[the_window].full_size.text_size;
		x1 = my_windows[the_window].full_size.screen_col;
		x2 = x1 + my_windows[the_window].full_size.screen_width;
		y1 = my_windows[the_window].full_size.screen_row - text_size*
			(the_row + 1)*5/4;
		y2 = y1 + text_size;
	}
	else
	{
		text_size = my_windows[the_window].retracted.text_size;
		x1 = my_windows[the_window].retracted.screen_col;
		x2 = x1 + my_windows[the_window].retracted.screen_width;
		y1 = my_windows[the_window].retracted.screen_row - text_size*
			(the_row + 1)*5/4;
		y2 = y1 + text_size;
	}



	y1 = y1 - text_size;
	y2 = y2 - text_size;



	color(xor_bar_color);

	wrmask(0xff);
	rdmask(io_masks[which_page]);

	draw_mode(3);
	move2(x1,y1);
	box(x2,y2);
	draw_mode(0);

	wrmask(io_masks[which_page]);

}



char  numeric_value(the_row)
int the_row;
{
char  *temp_ptr,  * field_ptr, temp_buffer[150];

	temp_ptr = my_windows[MAIN_MENU].text_array[the_row];
	strcpy(temp_buffer,temp_ptr);

	field_ptr = strtok(temp_buffer," \t\n\r");

	if(atoi(field_ptr) > 0)
	{
		return('t');
	}
	else
		return('f');

}



char  not_blank_light(the_row)
int the_row;
{
char  *temp_ptr, * field_ptr,temp_buffer[150];

	if(the_row < 0)
		return('f');

	temp_ptr = my_windows[LIGHTS].text_array[the_row];
	strcpy(temp_buffer,temp_ptr);
	field_ptr = strtok(temp_buffer," \t\n\r");

	if(field_ptr == NULL)
	{
		return('f');
	}
	else
		return('t');

}



void  find_main_menu_xy(menu_row)
int menu_row;
{
int text_size,screen_row,screen_col;
int the_window;

	the_window = MAIN_MENU;

	if(my_windows[the_window].is_retracted == 't')
	{
		text_size = my_windows[the_window].retracted.text_size;
		screen_row = my_windows[the_window].retracted.screen_row;
		screen_col = my_windows[the_window].retracted.screen_col;
	}
	else
	{
		text_size = my_windows[the_window].full_size.text_size;
		screen_row = my_windows[the_window].full_size.screen_row;
		screen_col = my_windows[the_window].full_size.screen_col;
	}


	mouse_y = new_y = old_y = screen_row - 5*(menu_row + 1)*text_size/4 -
		text_size/2;
	mouse_x = new_x = old_x = screen_col + 55;
}



int  find_which_row(the_window)
int the_window;
{
int text_size,screen_row;

	if(my_windows[the_window].is_retracted == 't')
	{
		text_size = my_windows[the_window].retracted.text_size;
		screen_row = my_windows[the_window].retracted.screen_row;
	}
	else
	{
		text_size = my_windows[the_window].full_size.text_size;
		screen_row = my_windows[the_window].full_size.screen_row;
	}

	return(4*(screen_row - new_y)/5/text_size - 1);
}



void  my_marker(x,y)
int x,y;
{
	if(vt100_mess_mon_flag == VT100_MODE)
		return;

	linpat(0);
	move2(x,y);
	gmark(9);
	linpat(1);
}






void  change_page_gmark_undraw()
{
	wrmask(io_masks[which_page]);
	color(gmark_color);
	draw_mode(3);

	if(which_high_res == RES_1024_768)
	{
		window(0,1023,0,767);
		viewport(0,1023,0,767);
	}
	else
	{
		window(0,1279,0,1023);
		viewport(0,1279,0,1023);
	}

	my_marker(old_x,old_y);
	draw_mode(0);
}




void  flood_draw_gmark()
{
	if(is_inside_menu(MAIN_MENU,old_y,old_x) == 'f') 
	{
		if((old_x >= graphics_window_x1) && 
			(old_x <= graphics_window_x2) &&
			(old_y >= graphics_window_y1) &&
			(old_y <= graphics_window_y2))
			{
				change_page_gmark_undraw();

			}
	}
}



void  update_gmark()
{
	if((new_x != old_x) || (new_y != old_y))
	{
/*
printf("Update_gmark() : old x = %d , old y = %d    new x = %d , new y = %d\n",
	old_x,old_y,new_x,new_y);
printf("Press Enter\n");
getch();
*/
		color(gmark_color);
		draw_mode(3);

		if(which_high_res == RES_1024_768)
		{
			window(0,1023,0,767);
			viewport(0,1023,0,767);
		}
		else
		{
			window(0,1279,0,1023);
			viewport(0,1279,0,1023);
		}

		my_marker(old_x,old_y);
		my_marker(new_x,new_y);
		draw_mode(0);
	}

}



void  move_pick_to_main_menu()
{
int i;

	for(i = 0 ; i < 8 ; i++)
		if(numeric_value(i) == 't')
			break;

	gmark_row_text = last_row_lit = i;
	xor_lighted_bar(MAIN_MENU,last_row_lit);
	row_is_lit = 't';

	change_page_gmark_undraw();


	find_main_menu_xy(last_row_lit);

	change_page_gmark_undraw();

}


void  move_mouse_to_new_pos()
{
	find_main_menu_xy(last_row_lit);
}



char  update_lighted_bar()
{
char inside_main_menu,inside_light_menu;

/*
printf("\nInside my_gmark()\n");
*/

	if((pen_press & 4) != 0)
	{
		my_send_ext(61);
	}
	else if((pen_press & 2) != 0)
		my_send_ext(60);


	inside_main_menu = is_inside_menu(MAIN_MENU,new_y,new_x);
	inside_light_menu = is_inside_menu(LIGHTS,new_y,new_x);


	if((inside_main_menu == 't') || (inside_light_menu == 't'))
	{
		if(inside_main_menu == 't')
		{
			if(is_inside_menu(LIGHTS,old_y,old_x) ==
				't')
			{
				if(row_is_lit == 't')
				{	
					row_is_lit = 'f';
					xor_lighted_bar(LIGHTS,last_row_lit);
					last_row_lit = -1;
				}
			}


			gmark_row_text = find_which_row(MAIN_MENU);
			if(numeric_value(gmark_row_text) == 't')
			{
				if(last_row_lit != gmark_row_text)
				{
					xor_lighted_bar(MAIN_MENU,
						last_row_lit); /* unlight last */
					xor_lighted_bar(MAIN_MENU, 
						gmark_row_text); /*light new*/
					last_row_lit = gmark_row_text;
					row_is_lit = 't';
				}

				if((pen_press & 1) != 0)
				{
					send_pick_to_host(MAIN_MENU,
						gmark_row_text);
				}
					
			}
			else
			{
				if((pen_press & 1) != 0)
				{
					return('t');
				}
				else if(last_row_lit >= 0)
				{
					xor_lighted_bar(MAIN_MENU,
						last_row_lit); /* unlight last */
					last_row_lit = -1;
					row_is_lit = 'f';
				}
			}
		}
		else  /* inside light menu */
		{
			if(is_inside_menu(MAIN_MENU,old_y,old_x) ==
				't')
			{
				if(row_is_lit == 't')
				{
					row_is_lit = 'f';
					xor_lighted_bar(MAIN_MENU,
						last_row_lit);
					last_row_lit = -1;
				}
			}


			gmark_row_text = find_which_row(LIGHTS);
			if(not_blank_light(gmark_row_text) == 't')
			{
				if(last_row_lit != gmark_row_text)
				{
					xor_lighted_bar(LIGHTS,
						last_row_lit); /* unlight last */
					xor_lighted_bar(LIGHTS, 
						gmark_row_text); /*light new*/
					last_row_lit = gmark_row_text;
					row_is_lit = 't';
				}

				if((pen_press & 1) != 0)
				{
					send_pick_to_host(LIGHTS,
						gmark_row_text);

					if(gmark_row_text < 16)
						picked_lights = 't';
				}

			}
			else
			{
				if(last_row_lit >= 0)
				{
					xor_lighted_bar(LIGHTS,
						last_row_lit); /* unlight last */
					last_row_lit = -1;
					row_is_lit = 'f';
				}
			}
		}

	} 
	else 	 /* outside a pick window */
	{
		if(row_is_lit == 't')
		{
			row_is_lit = 'f';

			if(is_inside_menu(MAIN_MENU,old_y,old_x) 
				== 't')
			{
				xor_lighted_bar(MAIN_MENU,last_row_lit);
				last_row_lit = -1;
			}

			if(is_inside_menu(LIGHTS,old_y,old_x)
				 == 't')
			{
				xor_lighted_bar(LIGHTS,last_row_lit);
				last_row_lit = -1;
			}
		}
	}


	return('f');
}

