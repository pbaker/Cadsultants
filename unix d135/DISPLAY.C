/*  6/11/90  - ADDED CALL TO RUN_VT FOR EVERY LINE FEED
              this should improve scrolling 
*/

/*  6/12/90   added wait_it_out_flag  check to  Above Modification 
*/



#include "my_globs.h"
#include "my_utils.h"
#include <pw_lib.h>
#include "mywindows.h"



#define dim_chr          28
#define bright_chr       29
#define space            20
#define cr               13
#define lf               10
#define bs               8
#define bel              7



#define VT100_MODE  0
#define MESS_MON  1




#define GRAPHICS_VIEWPORT  0
#define MESSAGE_VIEWPORT   1



#define W_BORDER_INDENT  9
#define V_BORDER_INDENT  4

#define CHAR_WIDTH  50
#define CHAR_HEIGHT  60

#define W_LEFT  8000
#define W_RIGHT  9600
#define W_TOP  2048
#define W_BOTTOM  -1000

#define V_LEFT  0
#define V_RIGHT 355  /* was 255 */
#define V_TOP  767
#define V_BOTTOM 0

#define FORE_COLOR  0x77
#define BACK_COLOR  0
#define BORDER_COLOR  0x55
#define CHAR_X_SHIFT  3*CHAR_WIDTH
#define CHAR_Y_SHIFT  2*CHAR_HEIGHT


#define RES_1024_768 3
#define RES_1280_1024 4


extern char  which_high_res;
extern int  run_vt_flag;
extern char  last_blt_flag;

extern char  is_there_graphics;
extern int  global_screen_color;
extern struct a_window  my_windows[];

extern int  vt100_mess_mon_flag;

extern char  stop_after_matrix_sub;
extern char  read_more_flag;
extern OPCODE  io_masks[];


/* for scroll VT100 window delay */
char  wait_it_out_flag = 'f';



char  did_host_poscur = 'f';
char  buffer_non_protect = 'f';
char  which_screen_area = MAIN_MENU;
char  my_test_flag = 'f';

char  char_type = 29;
char  bright_attribute = 15;
char  lite_attribute = 7;
char  dim_attribute = 7;
char  menu_attribute = 7;




int  mono_row = 0, mono_col = 0;
int  new_text_row,  new_text_col;
char  gtext_buffer[222] = "\0";
int  gtext_buffer_index = 0;
int  col_offset = 0, row_offset = 0;

char  in_which_viewport = MESSAGE_VIEWPORT;



void  restore_graphics_viewport()
{
	if(read_more_flag == 't')
		return;

/*
	if(in_which_viewport == GRAPHICS_VIEWPORT)
		return;

	in_which_viewport = GRAPHICS_VIEWPORT;
*/


	wrmask(io_masks[which_page]);


	if(which_cseg == 0)   /*use full screen*/
		my_set_viewport(-2048,-2048,2048,2048,0);
	else
	{
		stop_after_matrix_sub = 't';
		display_a_seg(which_cseg);
		stop_after_matrix_sub = 'f';
	}

	my_size_chars(0);

}





void  clear_whole_screen()
{
	if(which_high_res == RES_1024_768)
	{
		viewport(0,1023,0,767);
	}
	else
		viewport(0,1279,0,1023);


	wrmask(0xff);
	flood((short)((global_screen_color << 4) | global_screen_color));
	wrmask(io_masks[which_page]);
	rdmask(io_masks[which_page]);

	restore_graphics_viewport();
}





void  set_message_viewport()
{
#ifdef ksdslslslsl
	if(in_which_viewport == MESSAGE_VIEWPORT)
		return;

	in_which_viewport = MESSAGE_VIEWPORT;




	wrmask(0xff);

	my_size_chars(0);

	window(W_LEFT + CHAR_X_SHIFT,W_RIGHT + CHAR_X_SHIFT,
		W_BOTTOM + CHAR_Y_SHIFT,W_TOP + CHAR_Y_SHIFT);
	viewport(V_LEFT,V_RIGHT,V_BOTTOM,V_TOP);


	color(BORDER_COLOR);
	move2(W_LEFT + CHAR_X_SHIFT + W_BORDER_INDENT,
		W_BOTTOM + CHAR_Y_SHIFT + W_BORDER_INDENT);
	draw2(W_LEFT + CHAR_X_SHIFT + W_BORDER_INDENT,
		W_TOP + CHAR_Y_SHIFT - W_BORDER_INDENT);
	draw2(W_RIGHT + CHAR_X_SHIFT - W_BORDER_INDENT,
		W_TOP + CHAR_Y_SHIFT - W_BORDER_INDENT);
	draw2(W_RIGHT + CHAR_X_SHIFT - W_BORDER_INDENT,
		W_BOTTOM + CHAR_Y_SHIFT + W_BORDER_INDENT);
	draw2(W_LEFT + CHAR_X_SHIFT + W_BORDER_INDENT,
		W_BOTTOM + CHAR_Y_SHIFT + W_BORDER_INDENT);

	window(W_LEFT + CHAR_X_SHIFT + CHAR_WIDTH,
		W_RIGHT + CHAR_X_SHIFT  - CHAR_WIDTH,
		W_BOTTOM + CHAR_Y_SHIFT + CHAR_HEIGHT,
		W_TOP + CHAR_Y_SHIFT  - CHAR_HEIGHT);
	viewport(V_LEFT + V_BORDER_INDENT,V_RIGHT - V_BORDER_INDENT,
		V_BOTTOM + V_BORDER_INDENT,V_TOP - V_BORDER_INDENT);

#endif
}





void  display_non_protect_row()
{
int temp;

#ifdef kdkddkkdkkdkk


	temp = gtext_buffer_index;

	if(gtext_buffer_index > 0)
	{
		gtext_buffer[gtext_buffer_index] = '\0';
		gtext_buffer_index = 0;

		set_message_viewport();
		wrmask(0xff);



		move2((new_text_col + col_offset)*CHAR_WIDTH + W_LEFT - 9,
			W_TOP - (new_text_row + row_offset)*CHAR_HEIGHT -
			10);
		color(BACK_COLOR);

		if(gtext_buffer[1] != '\0')  	/* clear the row out fully */
		{
			boxr(CHAR_WIDTH*30,CHAR_HEIGHT);
		}
		else     /* clear this character */
			boxr(CHAR_WIDTH,CHAR_HEIGHT);




		color(FORE_COLOR);
		move2((new_text_col + col_offset)*CHAR_WIDTH + W_LEFT,
			W_TOP - (new_text_row + row_offset)*CHAR_HEIGHT);
		gtext(gtext_buffer);

		wrmask(io_masks[which_page]);
		restore_graphics_viewport();
	}
#endif
}





void  handle_non_protect(the_char)
int the_char;
{
#ifdef lslslsssllsl


	if(buffer_non_protect == 't')
	{
		if(did_host_poscur == 't')    /*display the buffer*/
		{
			did_host_poscur = 'f';

			display_non_protect_row();

			new_text_row = mono_row;
			new_text_col = mono_col;

			gtext_buffer_index = 1;
			gtext_buffer[0] = the_char;
		}
		else
		{
			gtext_buffer[gtext_buffer_index] = the_char;
			gtext_buffer_index++;
		}
	}
	else    /* do not buffer it */
	{
		set_message_viewport();
		wrmask(0xff);


	/* clear char box */
		move2((mono_col + col_offset)*CHAR_WIDTH + W_LEFT - 9,
			W_TOP - (mono_row + row_offset)*CHAR_HEIGHT -
				10);

		color(BACK_COLOR);
		boxr(CHAR_WIDTH,CHAR_HEIGHT);


		color(FORE_COLOR);

	/* draw the text char */
		gtext_buffer_index = 0;
		gtext_buffer[0] = the_char;
		gtext_buffer[1] = '\0';

		color(FORE_COLOR);
	
		move2((mono_col + col_offset)*CHAR_WIDTH + W_LEFT,
			W_TOP - (mono_row + row_offset)*CHAR_HEIGHT);
		gtext(gtext_buffer);


		wrmask(io_masks[which_page]);
		restore_graphics_viewport();
	}

#endif
}



void  clear_protect_dls()
{

}






void  pixel_char(the_char)
int the_char;
{
char   *temp_ptr;


	if(vt100_mess_mon_flag == MESS_MON)
	{
		my_windows[which_screen_area].was_updated = 't';

		temp_ptr = my_windows[which_screen_area].
			text_array[mono_row + row_offset];

		*(temp_ptr + (mono_col + col_offset)) = the_char;
	}
	else
	{
		*(my_windows[VT100_WINDOW].
			text_array[mono_row] + mono_col) = the_char;
	}


/*
	if(vt100_mess_mon_flag == VT100_MODE)
		run_vt_flag = 1;
	else
*/
	if((vt100_mess_mon_flag == MESS_MON) && 
		(which_screen_area == MAIN_MENU))
	{
		if(buffer_non_protect == 'f')
		{ 
			display_pop_up_window(MAIN_MENU);
		}
			
	}

}






/* HOST Invoked */
void  poscur(row,col)
int row,col;
{
	mono_row = row;
	mono_col = col;

	if(col >= 59)
	{
		which_screen_area = CALCULATOR;
		col_offset = -59;
		row_offset = 0;
	}
	else if(row >= 19)
	{
		which_screen_area = PART_INFO;

		col_offset = 0;
		row_offset = -19;
	}
	else
	{
		col_offset = -5;
		row_offset = 0;
		which_screen_area = MAIN_MENU;
	}

}




void  my_poscur(row,col)
int row,col;
{
	poscur(row,col);
}





void  clrall()
{
int row,col,i;


/*
	if(vt100_mess_mon_flag == VT100_MODE)
		clear_vt100_window(VT100_WINDOW,my_windows[VT100_WINDOW].
			background);
*/



	for(i = 0 ; i < 6 ; i++)
	{
		for(row = 0 ; row < MAX_WINDOW_ROWS ; row++)
		{
			for(col = 0 ; col < MAX_WINDOW_COLS - 1 ; col++)
				*(my_windows[i].text_array[row] + col) = ' ';

			*(my_windows[i].text_array[row] + (MAX_WINDOW_COLS - 1))
				 = 0;
		}
	}



	col_offset = row_offset = 0;
	poscur(0,0);

}



void  clrnon()
{
int row,col;


	for(row = 0 ; row < MAX_WINDOW_ROWS ; row++)
	{
		for(col = 0 ; col < MAX_WINDOW_COLS - 1 ; col++)
			*(my_windows[MAIN_MENU].text_array[row] + col) = ' ';

		*(my_windows[MAIN_MENU].text_array[row] + (MAX_WINDOW_COLS - 1))
			 = 0;
	}


	poscur(0,0);
}


extern int  run_vt_flag;
extern void  run_vt();


void  scroll_vt100()
{
int i;
char  *save_ptr;
int scroll_height;

	scroll_height = my_windows[VT100_WINDOW].full_size.scroll_height;

/*
printf("SCROLL_VT100()\n");
*/

	save_ptr = my_windows[VT100_WINDOW].text_array[0];

	for(i = 0 ; i < scroll_height ; i++)
		my_windows[VT100_WINDOW].text_array[i] = 
			my_windows[VT100_WINDOW].text_array[i + 1];


	for(i = 0 ; i < 83 ; i++)
		*(save_ptr + i) = ' ';

	*(save_ptr + 82) = '\0';

	my_windows[VT100_WINDOW].text_array[scroll_height] = save_ptr;

	run_vt_flag = 1;


	if(wait_it_out_flag == 't')
	{
wait_it_out:
	
		run_vt();	/* scroll the window */

	        if (run_vt_flag == 1)		/* flag reset if scrolled */
			goto wait_it_out;
	}

}




void  my_teletype(the_char)
int the_char;
{
int scroll_height;

        the_char &= 0xff;
	scroll_height = my_windows[VT100_WINDOW].full_size.scroll_height;



        if(the_char == cr)
	{
		if(vt100_mess_mon_flag == VT100_MODE)
			run_vt_flag = 1;

                poscur(mono_row,0);
	}
        else if(the_char == lf)
        {
		if(vt100_mess_mon_flag == MESS_MON)
		{
			if(mono_row >= 24)
			{
				clrall();
				poscur(0,0);
			}
		}
		else
		{
			run_vt_flag = 1;

			if(mono_row >= scroll_height)
			{
				scroll_vt100();
				mono_row = scroll_height - 1;
			}
			else 
				poscur(mono_row + 1,mono_col);
		}
        }
	else if(the_char == 8)
	{
		if(mono_col > 0)
			my_poscur(mono_row,mono_col - 1);
	}
        else
        {
		pixel_char(the_char);
                my_poscur(mono_row,mono_col + 1);
        }

}





void  dischr(the_char)
char the_char;
{
int scroll_height;


	scroll_height = my_windows[VT100_WINDOW].full_size.scroll_height;
	the_char = the_char & 0xff;



	if(my_test_flag == 't')
	{
		is_there_graphics = 'f';


		if(vt100_mess_mon_flag == MESS_MON)
			clear_whole_screen();

		vt100_mess_mon_flag = VT100_MODE;
		my_test_flag = 'f';
		clear_pop_up_window(VT100_WINDOW,my_windows[VT100_WINDOW].
			full_size.background,EXPANDED_POS);
	}



	if(char_type == dim_chr)
	{
		switch(the_char)
		{
			case space :
				goto bright;

			case cr    :
				goto bright;

			case lf    :
				goto bright;

			case bs    :
				goto bright;

			case bel   :
				goto bright;
		}

		pixel_char(the_char);

		if(mono_col == 79)
			poscur(mono_row + 1,0);
		else
			my_poscur(mono_row,mono_col + 1);

	}
	else
	{
bright:


		if(vt100_mess_mon_flag == MESS_MON)
		{
			if(mono_row >= 24)
			{
				clrall();
				poscur(0,0);
			}
		}
		else
		{
			if(mono_row >= scroll_height)
			{
				scroll_vt100();
				mono_row = scroll_height - 1;
			}
		}


		my_teletype(the_char);
	}

	
}

