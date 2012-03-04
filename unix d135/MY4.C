
/* MY4.C   Joystick functions */


/*October 10, 1988 11:30 added if(pen_press == 2) set pen_press2 = 't',
 pen2_x = old_x, pen2_y = old_y  to joystick().

Added check for curser is displayed to all ifs for deciding if you call
display_a_seg() stuff, in all joystick commands.
*/

/*October 10, 1988 11:30 added to joystick() to if(pen_press == 2) 
  joy_off = 't' & set_joystick(0)
*/

/*October 10,1988 11:30 added change_disp = 'f'; at end of read joystick*/

/*October 12, 1988 11:50 took out set_joystick(0) from if(pen_press == 2)
in joystick(). Put if((locate_called == 'f') || (curser_displayed == 't'))
around code to update new_x,new_y,old_x,old_y in joystick()*/

/*October 17, 1988 11:25 made all display_a_seg() calls depend only on
whether the curser is.displayed*/


/*October 17, 1988 11:37 "fixed" curser display code in read joystick & enable 
joystick to be the same as everywhere else*/

/*October 21, 1988 made all decisions about whether to display drag segment
depend on whether locate_called == 't' and curser_displayed == 't'*/

/*November 7, 1988 joystick() function: replaced micmxy() with read_mouse()
changed pen_press ifs to work with new pen_press values*/

/*November 15, 1988 Made changes to use generic graphics board calls*/

/*November 17, 1988 Made Mouse Menu reset curser to (0,0) only when F1 
pressed*/

/*January 30, 1989 made changes to send all error messages to a file*/

/*February 8,1989 removed "cursor speed control" menu from joystick() &
put it into the HOME menu in my2funcs.c*/

/*March 22, 1989 fixed all crosshair code to use host coords, instead of
artist/metheus screen size*/


/*March 24, 1989 changed comp_sub_data() for non-interpreted sub method*/





/* January 4, 1990  Joystick():      REMOVED THE 2

	if((locate_called == 'f') || (curser_displayed == 't'))

conditions because they prevent updating of mouse variables in Drag & Zoom/Pan
Combinations
	(Can't remember why I wanted those ifs anyway)
*/




#include "my_globs.h"
#include "my_utils.h"
#include <pw_lib.h> 
#include <math.h>
#include <malloc.h>
#include "mywindows.h"




#define X_MIN 0
int  mouse_x_max = 1023;
#define Y_MIN 0
int  mouse_y_max = 767;




/* ALT - Z caret position values */
#define NO_CARET 0
#define IS_PAN 1
#define IS_ZOOM 2
#define IS_FREE_CONTINUOUS 3
#define IS_FREE_CONTROLLED 4
#define IS_FIXED_CONTINUOUS 5
#define IS_FIXED_CONTROLLED 6
#define IS_UNDO_PREVIOUS 7
#define IS_EXPAND_UNEXPAND 8

#define ZOOM_Y_OFF 0

#define OLD_X_Y  0
#define NEW_X_Y  1


extern struct a_window  my_windows[];

extern void  toggle_retraction();
extern void  my_send_ext();
extern void  xor_lighted_bar();
extern char  is_inside_menu();
extern void  update_gmark();
extern char  update_lighted_bar();

extern void  restore_graphics_viewport();
extern char  use_the_mouse;
extern void  read_digitizer();
extern int  current_color;
extern int  save_system_color;
extern char  use_the_mouse;
extern char  row_is_lit;
extern int  last_row_lit;

extern void read_keyboard();


float  virtual_size = 4096.0,  actual_size = 768.0;



int  joy_x_sample, joy_y_sample;
int  last_mouse_x, last_neg_mouse_y;
int  last_mouse_y;

int  where_is_caret = NO_CARET;
int  initial_joy_count = 700;
int  my_temp_x,  my_temp_y;
char  my_temp_buttons;




/* digitizer  variables */
char  digitizer_enabled = 'f';
char  digitizer_read = 'f';
char  digitizer_curser_enabled = 'f';
int  digitizer_resolution;
int  digitizer_x, digitizer_y;
char  digitizer_buttons;
char  digitizer_echo;
float  digitizer_scale;
int  number_digitizer_bits;
unsigned int  digitizer_offset;
int  digitizer_x_offset;
int  digitizer_y_offset;
unsigned char  digitizer_terminator = 0x0d;
int  max_x_digitizer = 2048,  max_y_digitizer = 2048;
int  min_x_digitizer = -2047,  min_y_digitizer = -2047;
int  digitizer_phase,  digitizer_button_byte;
char  digitizer_depress_flag = 'f';


char  crosshair_undrawn = 'f';
char  have_checked_xy = 'f';


void  joystick();
void  digitizer();
void  display_locate_seg();
void  display_the_curser();
void  read_input_device();
void  read_mouse();



struct queue_record
{
	int x,y;
	char buttons;
	struct queue_record  *next_record;
};


struct queue_header
{
	struct queue_record  *queue_front,  *queue_rear;
};


struct queue_header  mouse_queue = {NULL,NULL};
int  next_x,  next_y;
char  next_buttons;


int  check_xy()
{
struct queue_record  *temp_ptr,  *temp2_ptr;

	if(have_checked_xy == 't')
	{
		if((is_inside_menu(MAIN_MENU,new_y,new_x) == 't') ||
			(is_inside_menu(LIGHTS,new_y,new_x) == 't'))
		{
			return('t');
		}
		else
			return('f');
	}


	have_checked_xy = 't';

	if(mouse_queue.queue_rear == NULL)
	{
		next_x = mouse_x;
		next_y = mouse_y;
	}
	else
	{
		next_x = mouse_queue.queue_rear -> x;
		next_y = mouse_queue.queue_rear -> y;
	}

	read_mouse(&next_x,&next_y,&next_buttons);

	temp_ptr = (struct queue_record  *)
		malloc(sizeof(struct queue_record));

	temp_ptr -> x = next_x;
	temp_ptr -> y = next_y;
	temp_ptr -> buttons = next_buttons;


	if(mouse_queue.queue_front == NULL)
	{
		mouse_queue.queue_front = mouse_queue.queue_rear = temp_ptr;
		temp_ptr -> next_record = NULL;
	}
	else
	{
		temp2_ptr = mouse_queue.queue_rear;
		mouse_queue.queue_rear = temp_ptr;
		temp_ptr -> next_record = temp2_ptr;
	}





	if((is_inside_menu(MAIN_MENU,next_y,next_x) == 't') ||
		(is_inside_menu(LIGHTS,next_y,next_x) == 't'))
	{
		return('t');
	}
	else
		return('f');

}


void  pop_mouse_queue()
{
struct queue_record  *temp_ptr;


	if(mouse_queue.queue_rear == mouse_queue.queue_front)
	{
		free(mouse_queue.queue_rear);
		mouse_queue.queue_rear = mouse_queue.queue_front = NULL;
	}
	else
	{
		for(temp_ptr = mouse_queue.queue_rear ; 
			temp_ptr -> next_record -> next_record != NULL ;
			temp_ptr = temp_ptr -> next_record)
				;

		free(mouse_queue.queue_front);
		temp_ptr -> next_record = NULL;
		mouse_queue.queue_front = temp_ptr;
	}

}



void  read_new_xy()
{
	have_checked_xy = 'f';

	if(mouse_queue.queue_rear == NULL)
	{
		new_x = mouse_x = next_x;
		new_y = mouse_y = next_y;
		pen_press = next_buttons;
	}
	else
	{
		new_x = mouse_x = mouse_queue.queue_front -> x;
		new_y = mouse_y = mouse_queue.queue_front -> y;
		pen_press = mouse_queue.queue_front -> buttons;

		pop_mouse_queue();
	}
}




int  reverse_the_buttons(the_buttons)
int the_buttons;
{
	switch(the_buttons)
	{
		case 0 :
			return(0);

		case 1 :
			return(8);

		case 2 :
			return(4);

		case 4 :
			return(2);

		case 8 :
			return(1);

		default :
			return(0);
	}
}




void  send_digitizer_to_host()
{
unsigned char digi_msx,digi_xbits,digi_lsx;
unsigned char digi_msy,digi_ybits,digi_lsy;
unsigned char first_char;
unsigned int temp_x,temp_y;


/*
printf("digitizer buttons = %x\n",digitizer_buttons);
*/

	first_char = digitizer_buttons;

	first_char |= 32;

	
	my_ugout(0x1d);
	my_ugout(first_char | 0x40);


/*
printf("SEND Digi:  digi X = %d     digi Y = %d\n",digitizer_x,digitizer_y);
*/

	temp_x = digitizer_x*16;
	temp_y = digitizer_y*16;


	digi_msx = (temp_x >> 12) & 0x0f;
	digi_xbits = (temp_x >> 6) & 0x3f;
	digi_lsx = temp_x & 0x3f;

	digi_msy = (temp_y >> 12) & 0x0f;
	digi_ybits = (temp_y >> 6) & 0x3f;
	digi_lsy = temp_y & 0x3f;


	my_ugout(digi_msx | 0x40);
	my_ugout(digi_xbits | 0x40);
	my_ugout(digi_lsx | 0x40);


	my_ugout(digi_msy | 0x40);
	my_ugout(digi_ybits | 0x40);
	my_ugout(digi_lsy | 0x40);

	my_ugout(0x0d);

}





/* updates digitizer x & y */
void  my_read_digitizer(the_x,the_y,the_buttons)
int  *the_x,  *the_y;
char  *the_buttons;
{
	read_digitizer(the_x,the_y,&digitizer_phase);


	if((*the_x) > max_x_digitizer)
		*the_x = max_x_digitizer;
	else if((*the_x) < min_x_digitizer)
		*the_x = min_x_digitizer;

	if((*the_y) > max_y_digitizer)
		*the_y = max_y_digitizer;
	else if((*the_y) < min_y_digitizer)
		*the_y = min_y_digitizer;



	if(digitizer_phase == 0)
	{
		if(digitizer_depress_flag == 'f')
		{
			*the_buttons = 0;
			digitizer_button_byte = 0;
		}
		else
		{
			digitizer_depress_flag = 'f';
			*the_buttons = digitizer_button_byte;
			digitizer_button_byte = 0;
		}
	}
	else
	{
		digitizer_depress_flag = 't';
		digitizer_button_byte = digitizer_phase;
	}

}





/* updates new x,y  , old x,y  ,  and cursors */
void  digitizer()
{
/*
	my_read_digitizer();

	new_x = digitizer_x;
	new_y = digitizer_y;
*/

	read_input_device();   /* sets new_x,new_y */


	if((digitizer_read == 't') && (digitizer_buttons != 0))
	{
		digitizer_read = 'f';
		send_digitizer_to_host();
		digitizer_enabled = 'f';
		set_joystick(0);

		if((digitizer_curser_enabled == 't') &&
			(locate_called == 't'))
		{
			display_locate_seg(OLD_X_Y);
		}

		save_joy_x = old_x;
		save_joy_y = old_y;


		if(digitizer_curser_enabled == 't')
		{
			display_the_curser(OLD_X_Y);
		}


		return;
	}


	if((old_x != new_x) || (old_y != new_y))
	{
		if((digitizer_curser_enabled == 't') &&
			(locate_called == 't'))
		{
			display_locate_seg(OLD_X_Y);
			display_locate_seg(NEW_X_Y);
		}

		save_joy_x = old_x;
		save_joy_y = old_y;


		if(digitizer_curser_enabled == 't')
		{
			display_the_curser(OLD_X_Y);
			display_the_curser(NEW_X_Y);
		}


		old_x = new_x;
		old_y = new_y;

	}

}






void  read_input_device()
{
	if(digitizer_enabled == 't') /* HOST WANTS DIGITIZER */
	{
		if(use_the_mouse == 'f')  /* WE USE Digitizer */
		{
			my_read_digitizer(&digitizer_x,&digitizer_y,
				&digitizer_buttons);
		}
		else    /* WE USE Mouse/Joystick */
		{
			read_mouse(&digitizer_x,&digitizer_y,
				&digitizer_buttons);
			digitizer_buttons = 
				reverse_the_buttons(digitizer_buttons);
		}


		new_x = digitizer_x;
		new_y = digitizer_y;
	}
	else    /* HOST WANTS THE JOYSTICK */
	{
		if(use_the_mouse == 'f')  /* WE USE Digitizer */
		{
			my_read_digitizer(&mouse_x,&mouse_y,&pen_press);
			pen_press = reverse_the_buttons(pen_press);
		}
		else   /* WE USE Mouse/Joystick */
		{
			read_mouse(&mouse_x,&mouse_y,&pen_press);
		}


		new_x = mouse_x;
		new_y = mouse_y;
	}

}




void  input_device()
{
char do_toggle;

	if(is_a_seg_open == 't')
		return;


	if(check_xy() == 't')   /* would be pick window == TRUE */
	{
		if((curser_displayed == 't') && (crosshair_undrawn == 'f'))
		{
			display_the_curser(OLD_X_Y);
			if(locate_called == 't')
				display_locate_seg(OLD_X_Y);

			crosshair_undrawn = 't';
		}

		read_new_xy();
		update_gmark();


		do_toggle = update_lighted_bar();

		old_x = new_x;
		old_y = new_y;

		if(do_toggle == 't')
			toggle_retraction(MAIN_MENU);



	}
	else   /* would not be pick window */
	{
		if((curser_displayed == 'f') && (crosshair_undrawn == 't'))
			crosshair_undrawn = 'f';

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

		joystick();
	}


/*
	if(digitizer_enabled == 't')
	{
		digitizer();
	}
	else
		joystick();
*/
}




/* for pan box movement scaling calculation */
short  read_3d_viewing_matrix[16];



void  display_locate_seg(old_new)
int old_new;
{
int graphics_row,graphics_col;

	graphics_col = my_windows[GRAPHICS_WINDOW].full_size.screen_col;
	graphics_row = my_windows[GRAPHICS_WINDOW].full_size.screen_row;

	save_my_x = my_x;
	save_my_y = my_y;

	if(old_new == OLD_X_Y)
	{
		save_joy_x = (int)((old_x - graphics_col)*virtual_size/
			actual_size) - 2048;
		save_joy_y = (int)((old_y - graphics_row)*virtual_size/
			actual_size) - 2048;
	}
	else
	{
		save_joy_x = (int)((new_x - graphics_col)*virtual_size/
			actual_size) - 2048;
		save_joy_y = (int)((new_y - graphics_row)*virtual_size/
			actual_size) - 2048;
	}

	draw_mode(0);
	display_a_seg(locate_seg);
	draw_mode(0);
	my_x = save_my_x;
	my_y = save_my_y;
}



void  display_the_curser(old_new)
int old_new;
{
int graphics_row,graphics_col;

	graphics_col = my_windows[GRAPHICS_WINDOW].full_size.screen_col;
	graphics_row = my_windows[GRAPHICS_WINDOW].full_size.screen_row;


	save_my_x = my_x;
	save_my_y = my_y;

	if(old_new == OLD_X_Y)
	{
		my_x = (int)((old_x - graphics_col)*virtual_size/actual_size)
			 - 2048;
		my_y = (int)((old_y - graphics_row)*virtual_size/actual_size)
			 - 2048;
	}
	else
	{
		my_x = (int)((new_x - graphics_col)*virtual_size/actual_size)
			 - 2048;
		my_y = (int)((new_y - graphics_row)*virtual_size/actual_size)
			 - 2048;
	}

	save_line_type = line_type;
	my_dash_enable(0);

/*
	draw_mode(3);
*/

	interp_my_sub6();  /* this will do the draw_mode()'s */

/*
	draw_mode(0);
*/

	my_color(save_color);
	line_type = save_line_type;
	my_dash_enable(line_type);
	my_x = save_my_x;
	my_y = save_my_y;
}





char  only_two_segments_visible()
{
int i,visi_count;

	visi_count = 0;

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if(segment_visibility[i] == 't')
			visi_count++;


	if(visi_count <= 2)       /* only ONE view is visible */
		return('t');
	else
		return('f');
}




void  my_pan_box(x,y)
int x,y;
{
double current_hcrst_scale,col_0_x,col_0_y,col_0_z;
int i;

/*
	goto_row_col(win_y_low + 1,win_x_low + 35);
	printf("X = %-5d ",new_x/4);
	goto_row_col(win_y_low + 2,win_x_low + 35);
	printf("Y = %-5d ",new_y/4);
*/

	if(only_two_segments_visible() == 'f')
		return;


/* read current 3d viewing matrix */
	rdmatvw3(&read_3d_viewing_matrix[0]);

	col_0_x = read_3d_viewing_matrix[0]/2048.0;
	col_0_y = read_3d_viewing_matrix[4]/2048.0;
	col_0_z = read_3d_viewing_matrix[8]/2048.0;

	current_hcrst_scale = sqrt(col_0_x*col_0_x + col_0_y*col_0_y +
				col_0_z*col_0_z);

	x = (int)((x - last_mouse_x)*current_hcrst_scale);
	y = (int)((y - last_mouse_y)*current_hcrst_scale);

	draw_mode(3);
	my_color(save_system_color);

	vect2(x - 2000,y - 2000,x - 2000,y + 2000);
	vect2(x - 2000,y - 2000,x + 2000,y - 2000);
	vect2(x + 2000,y - 2000,x + 2000,y + 2000);
	vect2(x - 2000,y + 2000,x + 2000,y + 2000);

	my_color(current_color);
	draw_mode(0);
}




void  my_zoom_box(y)
int y;
{
float zoom_factor,last_zoom_factor;
int row,col;


	if(only_two_segments_visible() == 'f')
	{
/*
		goto_row_col(win_y_low + 1,win_x_low + 35);
		printf("X = %-5d ",new_x/4);
		goto_row_col(win_y_low + 2,win_x_low + 35);
		printf("Y = %-5d ",new_y/4);

*/
		return;
	}


	draw_mode(3);
	my_color(save_system_color);

	if(y >= 1)  /*zoom in*/
	{
		last_zoom_factor =(9.1356*last_neg_mouse_y + 2038.8644)/2048.0;
		zoom_factor = (9.1356*y + 2038.8644)/2048.0/
			last_zoom_factor;

/*outer box*/
		vect2(- (int)(2000.0/zoom_factor),
			 - (int)(2000.0/zoom_factor),
			 (int)(2000.0/zoom_factor),
			 - (int)(2000.0/zoom_factor));
		vect2(- (int)(2000.0/zoom_factor),
			- (int)(2000.0/zoom_factor),
			- (int)(2000.0/zoom_factor),
			(int)(2000.0/zoom_factor));
		vect2(- (int)(2000.0/zoom_factor),
			(int)(2000.0/zoom_factor),
			(int)(2000.0/zoom_factor),
			(int)(2000.0/zoom_factor));
		vect2((int)(2000.0/zoom_factor),
			(int)(2000.0/zoom_factor),
			(int)(2000.0/zoom_factor),
			- (int)(2000.0/zoom_factor));

/*   inner cross   */
		vect2(0,
			- (int)(2000.0/zoom_factor),
			0,
			(int)(2000.0/zoom_factor));
		vect2(- (int)(2000.0/zoom_factor),
			0,
			(int)(2000.0/zoom_factor),
			0);
		prmfill(0);
		move2(0,0);
		circle((int)(120.0/zoom_factor));
	}
	else  /*zoom out*/
	{
		zoom_factor = (0.91356*y + 2047.0)/2048.0;

/*box*/
		vect2(- (int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor));
		vect2(- (int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor));
		vect2(- (int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor));
		vect2((int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor));


/*outer cross*/
		vect2(- (int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor) - 10000,
			- (int)(2000.0*zoom_factor) - 10000);
		vect2( - (int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor) - 10000,
			(int)(2000.0*zoom_factor) + 10000);
		vect2((int)(2000.0*zoom_factor),
			- (int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor) + 10000,
			- (int)(2000.0*zoom_factor) - 10000);
		vect2((int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor),
			(int)(2000.0*zoom_factor) + 10000,
			(int)(2000.0*zoom_factor) + 10000);

	}    /*end else zoom out*/


	my_color(current_color);
	draw_mode(0);
}



int  my_sign(i)
int i;
{
	if(i > 0)
		return(1);
	else if(i < 0)
		return(-1);
	else
		return(0);
}



int  alt_z_caret_position()
{
int row;


	for(row = 2 ; row < 12 ; row++)
		if(my_windows[MAIN_MENU].text_array[row][2] == '>')
			break;


	switch(row)
	{
		case 3 :
		{
		/* Don't call restore_graphics_viewport() when NOT needed */
			if(my_windows[MAIN_MENU].blt_status == SAVED_GRAPHICS)
			{
				restore_graphics_if_needed();
				restore_graphics_viewport();
			}

			return(IS_PAN);
		}

		case 4 :
		{
		/* Don't call restore_graphics_viewport() when NOT needed */
			if(my_windows[MAIN_MENU].blt_status == SAVED_GRAPHICS)
			{
				restore_graphics_if_needed();
				restore_graphics_viewport();
			}

			return(IS_ZOOM);
		}


		case 6 :
			return(IS_FREE_CONTROLLED);

		case 7 :
			return(IS_FREE_CONTINUOUS);

		case 8 :
			return(IS_FIXED_CONTROLLED);

		case 9 :
			return(IS_FIXED_CONTINUOUS);

		case 10 :
			return(IS_UNDO_PREVIOUS);

		case 11 :
			return(IS_EXPAND_UNEXPAND);

		default :
			return(NO_CARET);
	}
}





void  read_mouse(the_x,the_y,the_buttons)
int  *the_x,  *the_y;
char  *the_buttons;
{
int dx,dy;
int ddx,ddy;

	*the_x = *the_y = *the_buttons = 0;

#ifdef llllllllllllll



	my_input_regs.x.ax = 11;     /*raw mouse movements*/
	int86(0x33,&my_input_regs,&my_output_regs);

	ddx = dx = my_output_regs.x.cx;
	ddy = dy = my_output_regs.x.dx;


#ifdef zekeyer_than_you

	while((ddx != 0) || (ddy != 0))
	{
		my_input_regs.x.ax = 11;     /*raw mouse movements*/
		int86(0x33,&my_input_regs,&my_output_regs);
		ddx = my_output_regs.x.cx;
		ddy = my_output_regs.x.dx;

		dx += ddx;
		dy += ddy;
	}

#endif


	dy = -dy;   /*invert mouse y right here and NOWHERE else*/


	if(curser_ratio > 0)
	{
		dy *= curser_ratio;
		dx *= curser_ratio;
	}
	else
	{
		dy /= (-curser_ratio);
		dx /= (-curser_ratio);
	}

	(*the_x) += dx;
	(*the_y) += dy;


	if((*the_x) > mouse_x_max)
		*the_x = mouse_x_max;
	else if((*the_x) < X_MIN)
		*the_x = X_MIN;

	if((*the_y) > mouse_y_max)
		*the_y = mouse_y_max;
	else if((*the_y) < Y_MIN)
		*the_y = Y_MIN;


	my_input_regs.x.ax = 3;
	int86(0x33,&my_input_regs,&my_output_regs);

	mouse_phase = my_output_regs.x.bx;

	if(mouse_phase == 0)
	{
		if(depress_flag == 'f')
		{
			*the_buttons = 0;
			button_byte = 0;
		}
		else
		{
			depress_flag = 'f';
			*the_buttons = button_byte;
			button_byte = 0;
		}
	}
	else
	{
		depress_flag = 't';
		button_byte = mouse_phase;
	}
#endif
}



int  positive_only(i)
int i;
{
	if(i > 0)
		return(i);
	else
		return(0);
}





void  my_zoom_pan(which_one)
char which_one;
{
int graphics_row,graphics_col;

	graphics_col = my_windows[GRAPHICS_WINDOW].full_size.screen_col;
	graphics_row = my_windows[GRAPHICS_WINDOW].full_size.screen_row;


	read_input_device();   /* sets new_x , new_y */


	if(which_one == 'z')
	{
		if(last_neg_mouse_y != 0)  /*mouse button 1 pushed*/
		{
			if(my_sign(last_neg_mouse_y) != 
				my_sign((int)(virtual_size*(new_y - graphics_row)
					/actual_size) - 2048))
			{
				if(abs((int)(virtual_size*(new_y - graphics_row)
					/actual_size) - 2048) >
					 positive_only(curser_ratio)) 
						mouse_y = new_y = old_y;
			}
		}
	}


	if((new_x != old_x) || (new_y != old_y))
	{
		if(which_one == 'p')
		{
			my_pan_box((int)((old_x - graphics_col)*virtual_size/actual_size)
			 - 2048,(int)((old_y - graphics_row)*virtual_size/actual_size) - 2048);
			my_pan_box((int)((new_x - graphics_col)*virtual_size/actual_size)
			 - 2048,(int)((new_y - graphics_row)*virtual_size/actual_size) - 2048);
		}
		else if(which_one == 'z')
		{

			my_zoom_box((int)((old_y - graphics_row)*virtual_size/actual_size)
				 - 2048);
			my_zoom_box((int)((new_y - graphics_row)*virtual_size/actual_size)
				 - 2048);
		}

		old_x = new_x;
		old_y = new_y;			
	}
}



void comp_sub_data(sub_num_index,sub_end,sub_num)
int *sub_num_index,*sub_end,sub_num;
{
int i;

	*sub_num_index = sub_num;

	*sub_end = sub_insert_ptrs[sub_num];
}




void  compile_sub_curser(sub_num)
int sub_num;
{
int sub_ptr,which_sub,sub_end,x,y;


	comp_sub_data(&which_sub,&sub_end,sub_num);

	for(sub_ptr = 0 ; sub_ptr <= sub_end ; sub_ptr++)
	{
		get_sub_value(sub_ptr,which_sub);

		if(ptr_sub -> op_code == 50)  /*its compressed data*/
		{
			if(((ptr_sub -> arg1 >> 12) & 0x000f) != 12)
			{
				sprintf(diag_string,"UNKNOWN OP_CODE IN COMPRESSED DATA\n");
				error_logger(diag_string);
			}
			else
			{
				x = ptr_sub -> arg1 & 0x0fff;
				y = ptr_sub -> arg2 & 0x0fff;

				switch((ptr_sub -> arg2 >> 12) & 0x000f)
				{
					case 6 : /*move: abs x,rel dy*/
					{
						an_element.op_code = 80;
						an_element.arg1 =
							sub6_abs_coord(x);
						an_element.arg2 =
							sub6_rel_coord(y);

						insert_sub(an_element,
							NUMBER_OF_SUBS);
						break;
					}

					case 14 : /*draw: abs x,rel dy*/
					{
						an_element.op_code = 82;
						an_element.arg1 =
							sub6_abs_coord(x);
						an_element.arg2 =
							sub6_rel_coord(y);

						insert_sub(an_element,
							NUMBER_OF_SUBS);
						break;
					}

					case 7 : /*move: rel dx,abs y*/
					{
						an_element.op_code = 81;
						an_element.arg1 = 
							sub6_rel_coord(x);
						an_element.arg2 =
							sub6_abs_coord(y);

						insert_sub(an_element,
							NUMBER_OF_SUBS);
						break;
					}

					case 15 : /*draw: rel dx,abs y*/
					{
						an_element.op_code = 83;
						an_element.arg1 = 
							sub6_rel_coord(x);
						an_element.arg2 =
							sub6_abs_coord(y);

						insert_sub(an_element,
							NUMBER_OF_SUBS);
						break;
					}

					default :
					{
						sprintf(diag_string,
							"UNKNOWN MOVE/DRAW CODE IN SUB6 COMPILER:\n ");
						error_logger(diag_string);
						sprintf(diag_string,
							"Sub %d  ms = %u ls = %u\n",sub_num,
						       ptr_sub -> arg1,ptr_sub -> arg2);
						error_logger(diag_string);
					}

				}
			}
		}
		else if(ptr_sub -> op_code != 0) /*not a call sub*/
		{
			an_element.op_code = ptr_sub -> op_code;
			an_element.arg1 = ptr_sub -> arg1;
			an_element.arg2 = ptr_sub -> arg2;

			insert_sub(an_element,NUMBER_OF_SUBS);
		}
		else  /*is a sub call*/
		{
			if(ptr_sub -> arg1 == 3)
			{
				an_element.op_code = 90;
				insert_sub(an_element,
					NUMBER_OF_SUBS);
			}
			else
				compile_sub_curser(ptr_sub -> arg1);
		}
	}
}





void  q_func(sub_count,which_subr)  /*disable something*/
int *sub_count,which_subr;
{

		read_more(1);
		a_char = megatek_data[0];

		switch(a_char & 0x7f)
		{
			case '@' : /*disable keyboard*/
			{
				diag_num = 22;
#ifdef HAVE_DIAGS
				my_diag("disable keyboard\n");
#endif
				break;
			}

			case 'A' : /*disable function pad*/
			{
				diag_num = 23;
#ifdef HAVE_DIAGS
				my_diag("disable function pad\n");
#endif
				break;
			}

			case 'B' : /*disable joystick*/
			{
				diag_num = 24;
#ifdef HAVE_DIAGS
				my_diag("disable joystick\n");		
#endif

				if(joy_enable == 'f')
					return;

				set_joystick(0);

				if((curser_displayed == 't') && 
					(locate_called == 't'))
				{
					display_locate_seg(OLD_X_Y);

				}

				save_joy_x = old_x;
				save_joy_y = old_y;

				if(curser_displayed == 't')
				{
		if((is_inside_menu(MAIN_MENU,new_y,new_x) == 'f') &&
			(is_inside_menu(LIGHTS,new_y,new_x) == 'f'))
		{
					display_the_curser(OLD_X_Y);
		}
		else
			crosshair_undrawn = 't';

				}

				joy_enable = 'f';
				read_joystick = 'f';
				curser_displayed = 'f';

				break;
			}

			case 'C' : /*disable digitizer*/
			{
				diag_num = 25;
#ifdef HAVE_DIAGS
				my_diag("disable digitizer\n");
#endif

				if(digitizer_enabled == 't')
				{
					set_joystick(0);

					if((digitizer_curser_enabled == 't') &&
						(locate_called == 't'))
					{
						display_locate_seg(OLD_X_Y);
					}

					save_joy_x = old_x;
					save_joy_y = old_y;

					if(digitizer_curser_enabled == 't')
					{
						display_the_curser(OLD_X_Y);
					}

				}


				digitizer_enabled = 'f';
				digitizer_curser_enabled = 'f';
				digitizer_read = 'f';
				
				break;
			}

			case 'D' : /*disable pick module*/
			{
				diag_num = 26;
#ifdef HAVE_DIAGS
				my_diag("disable pick module\n");
#endif
				break;
			}

			default : /*error*/
			{
				sprintf(diag_string,
					"Error in 2nd byte of command q\n");
				error_logger(diag_string);
				break;
			}
		}
	
}








void  my_pan_menu()
{
#ifdef llwlweqlewle

	read_curser_pos();
	turn_off_curser();
	make_win(10,17,57,23);
	save_win();

	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("  Move Mouse To Pan");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("  Button #1 To Update View");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("  Button #2 To Reset View");

#endif
}



void  my_zoom_menu()
{
#ifdef klelelelel

	read_curser_pos();
	turn_off_curser();
	make_win(10,17,57,23);
	save_win();

	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	if(old_y > 0)
		printf("   Zoom In");
	else
		printf("   Zoom Out");	

	goto_row_col(win_y_low + 2,win_x_low + 1);
	printf("   Move Mouse Up/Down");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("   Button #1 To Update View");
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("   Button #2 To Reset View");

#endif
}



void  my_rotate_menu()
{
#ifdef kedkelele

	read_curser_pos();
	turn_off_curser();
	make_win(10,17,57,23);
	save_win();

	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("   Move Mouse To Rotate");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("   Button #1 To Update View");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("   Button #2 To Reset View");

#endif
}





void  remove_xform_menu()
{
#ifdef ldwlwdldwldl

	restore_win();   /*retrieve stored screen information*/
	goto_row_col(row_curser,col_curser);
	turn_on_curser();
	make_win(10,3,60,16);  /*restore Home Menu's Window*/

#endif
}



void  fixed_rotations_message()
{
#ifdef ldlldslldsls

	if((where_is_caret == IS_FIXED_CONTINUOUS) ||
		(where_is_caret == IS_FIXED_CONTROLLED))
	{
		goto_row_col(win_y_low + 1,win_x_low + 35);
		printf("X = %-5d ",new_x/4);
		goto_row_col(win_y_low + 2,win_x_low + 35);
		printf("Y = %-5d ",new_y/4);

	}
#endif
}








void  r_func(sub_count,which_subr) /*sample something*/
int *sub_count,which_subr;
{
unsigned char most_x,most_y,least_x,least_y,x_y_6_bit;
int joy_x,joy_y;
long temp;
char reset_mouse_variables;
int graphics_row,graphics_col;

	graphics_col = my_windows[GRAPHICS_WINDOW].full_size.screen_col;
	graphics_row = my_windows[GRAPHICS_WINDOW].full_size.screen_row;



	reset_mouse_variables = 'f';
	read_more(1);
	a_char = megatek_data[0];
	

	switch(a_char & 0x7f)
	{
		case '@' : /*sample keyboard*/
		{
			diag_num = 27;
#ifdef HAVE_DIAGS
			my_diag("sample keyboard\n");
#endif
			break;
		}

		case 'A' : /*sample function pad*/
		{
			diag_num = 28;
#ifdef HAVE_DIAGS
			my_diag("sample function pad\n");
#endif
			break;
		}

		case 'B' : /*sample joystick*/
		{
			diag_num = 29;
#ifdef HAVE_DIAGS
			my_diag("sample joystick\n");
#endif


		    if(rotate_mode == 'm')
		    {
			where_is_caret = alt_z_caret_position();

			pen_press = 0;

			switch(where_is_caret)
			{
				case IS_PAN :
				{
					my_pan_menu();
					my_pan_box((int)((old_x - graphics_col)
						*virtual_size/actual_size)
						 - 2048,
						(int)((old_y - graphics_row)
						*virtual_size/actual_size)
							 - 2048);

					while((pen_press & 1) == 0)
					{
						my_zoom_pan('p');

						if((pen_press & 4) != 0)
						{
							reset_mouse_variables
								= 't';
							break;
						}

						read_keyboard();

						if((type_char_read != 0) ||
							(value_char_read != 0))
								break;
					}

					my_pan_box((int)((old_x - graphics_col)
						*virtual_size/
						actual_size) - 2048,
						(int)((old_y - graphics_row)
						*virtual_size/
						actual_size) - 2048);
					remove_xform_menu();

					if(reset_mouse_variables == 't')
					{
						mouse_x = mouse_y = 384;
						new_y = old_y = 384;
						new_x = old_x = 384;
						last_neg_mouse_y = 0;
					}

					break;
				}


				case IS_ZOOM :
				{
					my_zoom_menu();
					my_zoom_box((int)((old_y - graphics_row)
						*virtual_size/
						actual_size) - 2048);

					while((pen_press & 1) == 0)
					{
						my_zoom_pan('z');

			/*
					        goto_row_col(win_y_low + 1,
							win_x_low + 1);

						if(old_y > 0)
							printf("   Zoom In  ");
						else
							printf("   Zoom Out ");
			*/

						if((pen_press & 4) != 0)
						{
							reset_mouse_variables
								= 't';
							break;
						}

						read_keyboard();

						if((type_char_read != 0) ||
							(value_char_read != 0))
								break;
					}

					my_zoom_box((int)((old_y - graphics_row)
						*virtual_size/
						actual_size) - 2048);
					remove_xform_menu();

					if(reset_mouse_variables == 't')
					{
						mouse_y = 384;
						new_y = old_y = 384;
						last_neg_mouse_y = 0;
					}

					break;
				}



				case NO_CARET :
					break;




				default :
				{
					my_rotate_menu();

					while((pen_press & 1) == 0)
					{
						read_keyboard();

						if((type_char_read != 0) ||
							(value_char_read != 0))
								break;

						input_device();
						fixed_rotations_message();

						if((pen_press & 4) != 0)
						{
							mouse_x = mouse_y = 384;
							new_y = old_y = 384;
							new_x = old_x = 384;
							last_neg_mouse_y = 0;

							break;
						}
					}

					remove_xform_menu();
					break;
				}
			}

/* set sample coordinates to last values reported to host */

			if(where_is_caret != NO_CARET)
			{
				joy_x = joy_x_sample;
				joy_y = joy_y_sample;
			}
			else
			{
				joy_x = new_x;
				joy_y = new_y;
			}


/*
   if mouse button 1 was pressed, update joystick coordinates,
   & update sample coordinates. 
*/

			if((type_char_read == 0) && (value_char_read == 0))
			{
				joy_x = new_x;
				joy_y = new_y;

				joy_x_sample = new_x;
				joy_y_sample = new_y;

/*save last mouse position*/
				last_mouse_x = (int)(virtual_size*(new_x -
					graphics_col)/
					actual_size) - 2048;
				last_mouse_y = (int)(virtual_size*(new_y -
					graphics_row)/
					actual_size) - 2048;
				last_neg_mouse_y = (int)(virtual_size*(new_y -
					graphics_row)/
					actual_size) - 2048;
			}
		   }
		   else  /*auto mode*/
		   {
			joy_x = new_x;
			joy_y = new_y;
		   }


			joy_x = (int)((joy_x - graphics_col)*
				virtual_size/actual_size) - 2048;
			joy_y = (int)((joy_y - graphics_row)*
				virtual_size/actual_size) - 2048;

			if(joy_x > 2047)
				joy_x = 2047;

			if(joy_x < -2048)
				joy_x = -2048;

			if(joy_y > 2047)
				joy_y = 2047;

			if(joy_y < -2048)
				joy_y = -2048;



			my_ugout(0x1d);


			if((pen_press & 1) != 0)
				my_ugout(0x41);
			else
				my_ugout(0x40);

			most_x = (joy_x >> 6) & 0x3f;
			least_x = joy_x & 0x3f;
			most_y = (joy_y >> 6) & 0x3f;
			least_y = joy_y & 0x3f;

			if(joy_x == -2048)
				x_y_6_bit = 0x00;
			else if(joy_x == -1)
				x_y_6_bit = 0x02 << 3;
			else if(joy_x < 0)
				x_y_6_bit = 0x01 << 3;
			else if(joy_x == 0)
				x_y_6_bit = 0x03 << 3;
			else if(joy_x == 1)
				x_y_6_bit = 0x05 << 3;
			else if(joy_x == 2047)
				x_y_6_bit = 0x07 << 3;
			else
				x_y_6_bit = 0x06 << 3;



			if(joy_y == -2048)
				x_y_6_bit |= 0x00;
			else if(joy_y == -1)
				x_y_6_bit |= 0x02;
			else if(joy_y < 0)
				x_y_6_bit |= 0x01;
			else if(joy_y == 0)
				x_y_6_bit |= 0x03;
			else if(joy_y == 1)
				x_y_6_bit |= 0x05;
			else if(joy_y == 2047)
				x_y_6_bit |= 0x07;
			else
				x_y_6_bit |= 0x06;


			my_ugout(most_x | 0x40);
			my_ugout(least_x | 0x40);
			my_ugout(most_y | 0x40);
			my_ugout(least_y | 0x40);
			my_ugout(x_y_6_bit | 0x40);
			my_ugout(0x0d);

			break;
		}

		case 'C' : /*sample digitizer*/
		{
			diag_num = 30;
#ifdef HAVE_DIAGS
			my_diag("sample digitizer\n");
#endif


/*
printf("SAMPLE DIGI\n");
*/
			send_digitizer_to_host();


			break;
		}

		case 'D' : /*sample pick module*/
		{
			diag_num = 31;
#ifdef HAVE_DIAGS
			my_diag("sample pick module\n");
#endif
			break;
		}

		case 'E' : /*sample control dial*/
		{
			diag_num = 32;
#ifdef HAVE_DIAGS
			my_diag("sample control dial\n");
#endif
			read_more(1);

			break;
		}

		case 'F' : /*sample switch/light*/
		{
			read_more(1);

			diag_num = 33;
#ifdef HAVE_DIAGS
			my_diag("sample switch/light\n");
#endif
			break;
		}

		default : /*error*/
		{
			sprintf(diag_string,"Error in 2nd byte of command r\n");
			error_logger(diag_string);
			break;
		}
	}

}




void  s_func(sub_count,which_subr)  /*read something*/
int *sub_count,which_subr;
{
unsigned char x_y_6_bit,most_x,most_y,least_x,least_y;
int joy_x,joy_y;
char change_disp;
char digitizer_change_disp;


	digitizer_change_disp = 'f';
	change_disp = 'f';

	read_more(1);
	a_char = megatek_data[0];


	switch(a_char & 0x7f)
	{
		case '@' : /*read keyboard*/
		{
			read_more(1);
			diag_num = 34;
#ifdef HAVE_DIAGS
			my_diag("read keyboard: ");
#endif

			break;
		}

		case 'A' : /*read function pad*/
		{
			diag_num = 35;
#ifdef HAVE_DIAGS
			my_diag("read function pad\n");
#endif
			break;
		}

		case 'B' : /*read joystick*/
		{
			read_more(1);		
			diag_num = 36;
#ifdef HAVE_DIAGS
			my_diag("READ JOYSTICK!!!!!!!!\n");
#endif
			save_my_x = my_x;
			save_my_y = my_y;
			where_is_caret = NO_CARET;

				if((megatek_data[0] & 32) != 0)
				{  /*initialize curser at screen center*/
#ifdef HAVE_DIAGS
					my_diag("init curser at screen center\n");
#endif
					mouse_x = new_x = old_x = 385;
					mouse_y = new_y = old_y = 383;
				}
				else /*initialize curser at last curser position*/
				{
#ifdef HAVE_DIAGS
					my_diag("init curser at last curser pos\n");
#endif

				}


				if((megatek_data[0] & 1) != 0)
				{ /*curser is displayed*/
#ifdef HAVE_DIAGS
					my_diag("curser is displayed\n\n");
#endif

					if(curser_displayed == 'f')
						change_disp = 't';

					curser_displayed = 't';
				}
				else /*curser is not displayed*/
				{
#ifdef HAVE_DIAGS
					my_diag("curser is not displayed\n\n");
#endif
					curser_displayed = 'f';
				}
			
				read_joystick = 't';

				if((joy_enable == 'f') || (change_disp == 't'))
				{
				    if(locate_called == 't')
				    {
					display_locate_seg(OLD_X_Y);
				    }
				}

			save_joy_x = old_x;
			save_joy_y = old_y;


			if((joy_enable == 'f') || (change_disp == 't'))
			{
				joy_enable = 't';

				set_joystick(1);
#ifdef HAVE_DIAGS
				my_diag("READ JOYSTICK DRWMOD(3)\n");
#endif

				if(curser_displayed == 't')
				{
		if((is_inside_menu(MAIN_MENU,new_y,new_x) == 'f') &&
			(is_inside_menu(LIGHTS,new_y,new_x) == 'f'))
		{
					display_the_curser(OLD_X_Y);
		}
		else
			crosshair_undrawn = 't';
				}
			}

			diag_num = -36;

			my_x = save_my_x;
			my_y = save_my_y;

			change_disp = 'f';

			break;
		}

		case 'C' : /*read digitizer*/
		{

			read_more(1);
	
			diag_num = 37;
#ifdef HAVE_DIAGS
			my_diag("read digitizer\n");
#endif

/*
printf("READ DIGI\n");
*/

			if((megatek_data[0] & 32) != 0)
			{  /*initialize curser at screen center*/
#ifdef HAVE_DIAGS
				my_diag("init curser at screen center\n");
#endif
				digitizer_x = new_x = old_x = 385;
				digitizer_y = new_y = old_y = 383;
			}
			else /*initialize curser at last curser position*/
			{
#ifdef HAVE_DIAGS
				my_diag("init curser at last curser pos\n");
#endif

			}


			if((megatek_data[0] & 1) != 0)
			{ /*curser is displayed*/
#ifdef HAVE_DIAGS
				my_diag("curser is displayed\n\n");
#endif

				if(digitizer_curser_enabled == 'f')
					digitizer_change_disp = 't';

				digitizer_curser_enabled = 't';
			}
			else /*curser is not displayed*/
			{
#ifdef HAVE_DIAGS
				my_diag("curser is not displayed\n\n");
#endif
					digitizer_curser_enabled = 'f';
			}


			digitizer_read = 't';


			if((digitizer_enabled == 'f') ||
			    (digitizer_change_disp == 't'))
			{
				if(locate_called == 't')
				{
					display_locate_seg(OLD_X_Y);
				}
			}

			save_joy_x = old_x;
			save_joy_y = old_y;


			if((digitizer_enabled == 'f') ||
			    (digitizer_change_disp == 't'))
			{
				digitizer_enabled = 't';

				set_joystick(1);


				if(digitizer_curser_enabled == 't')
				{
					display_the_curser(OLD_X_Y);
				}
			}

			diag_num = -36;

			my_x = save_my_x;
			my_y = save_my_y;

			digitizer_change_disp = 'f';


			break;
		}

		case 'D' : /*read pick module*/
		{

			read_more(1);
	
			diag_num = 38;
#ifdef HAVE_DIAGS
			my_diag("read pick module\n");
#endif

			break;
		}

		case 'E' : /*read control dial*/
		{
			diag_num = 39;
#ifdef HAVE_DIAGS
			my_diag("read control dial\n");
#endif

			read_more(2);

			break;
		}

		case 'F' : /*read switches/lights*/
		{
			read_more(3);

			diag_num = 40;
#ifdef HAVE_DIAGS
			my_diag("read switches/lights\n");
#endif
			break;
		}

		default : /*error*/
		{
			sprintf(diag_string,"error in 2nd byte of command s\n");
			error_logger(diag_string);
			break;
		}
	}

}




void  t_func(sub_count,which_subr) /*set something*/
int *sub_count,which_subr;
{

	read_more(1);
	a_char = megatek_data[0];


	diag_num = 41;	
#ifdef HAVE_DIAGS
	my_diag("set limits on ");
#endif
	diag_num = -41;

	switch(a_char & 0x7f)
	{
		case 'B' : /*set joystick limits*/
		{
#ifdef HAVE_DIAGS
			my_diag("joystick\n");
#endif
			read_more(8);

			break;
		}

		case 'C' : /*set digitizer limits*/
		{
#ifdef HAVE_DIAGS
			my_diag("digitizer\n");
#endif

			read_more(8);


			min_x_digitizer = comp_rel_coord(megatek_data[0],
						megatek_data[1]);
			min_y_digitizer = comp_rel_coord(megatek_data[2],
						megatek_data[3]);


			max_x_digitizer = comp_rel_coord(megatek_data[4],
						megatek_data[5]);
			max_y_digitizer = comp_rel_coord(megatek_data[6],
						megatek_data[7]);

/*
			fprintf(stdprn,"\n\nSET LIMITS ON DIGITIZER\n\n");
			fprintf(stdprn,"Min X = %d  ,  Min Y = %d\n",
				min_x_digitizer,min_y_digitizer);
			fprintf(stdprn,"Max X = %d  ,  Max Y = %d\n\n",
				max_x_digitizer,max_y_digitizer);
			fflush(stdprn);
*/

			break;
		}

		default : /*error*/
		{
			sprintf(diag_string,"error in 2nd byte of command t\n");
			error_logger(diag_string);
			break;
		}
	}

}




void  w_func(sub_count,which_subr) /*set something*/
int *sub_count,which_subr;
{

	read_more(1);
	a_char = megatek_data[0];

	switch(a_char & 0x7f)
	{
		case 'C' : /*set digitizer params*/
		{

			read_more(5);

				diag_num = 42;
#ifdef HAVE_DIAGS
				my_diag("set digitizer params\n");
#endif


			switch(megatek_data[0] & 7)
			{
				case 1 :
				{
					digitizer_scale = 2.0;
					number_digitizer_bits = 11;
					digitizer_offset = 0xfc0;

					break;
				}

				case 0 :
				{
					digitizer_scale = 1.0;
					number_digitizer_bits = 12;
					digitizer_offset = 0xf80;

					break;
				}

				case 3 :
				{
					digitizer_scale = 0.5;
					number_digitizer_bits = 13;
					digitizer_offset = 0xf00;

					break;
				}

				case 2 :
				{
					digitizer_scale = 0.25;
					number_digitizer_bits = 14;
					digitizer_offset = 0xe00;

					break;
				}

				case 5 :
				{
					digitizer_scale = 0.125;
					number_digitizer_bits = 15;
					digitizer_offset = 0xc00;

					break;
				}

				case 4 :
				{
					digitizer_scale = 1.0/16.0;
					number_digitizer_bits = 16;
					digitizer_offset = 0x800;

					break;
				}
			}


			digitizer_x_offset = (megatek_data[1] << 6) +
							megatek_data[2];
			digitizer_y_offset = (megatek_data[3] << 6) +
							megatek_data[4];



/*
			fprintf(stdprn,"SET DIGITIZER PARAMS\n\n");
			fprintf(stdprn,"Flags byte = %x\n",megatek_data[0]);
			fprintf(stdprn,"X offset = %d\n",digitizer_x_offset);
			fprintf(stdprn,"Y offset = %d\n",digitizer_y_offset);
			fprintf(stdprn,"digi # bits = %d\n\n",
				number_digitizer_bits);
			fflush(stdprn);
*/

			break;
		}

		case 'D' : /*select locator & pick window*/
		{

			read_more(5);

				diag_num = 43;
#ifdef HAVE_DIAGS
				my_diag("select locator & pick window\n");
#endif

			break;
		}

		case 'E' : /*set control dial params*/
		{

			read_more(13);

			diag_num = 44;
#ifdef HAVE_DIAGS
			my_diag("set control dial params\n");
#endif
			break;
		}

		case 'F' : /*set switch/light params*/
		{

			read_more(5);

				diag_num = 45;
#ifdef HAVE_DIAGS
				my_diag("set switch/light params\n");
#endif

			break;
		}

		default : /*error*/
		{
			sprintf(diag_string,"Error in 2nd byte of command w\n");
			error_logger(diag_string);
			break;
		}
	}

}






void return_sample()
{
unsigned char most_x,most_y,least_x,least_y,x_y_6_bit;
int joy_x,joy_y;
long temp;
int graphics_row,graphics_col;

	graphics_col = my_windows[GRAPHICS_WINDOW].full_size.screen_col;
	graphics_row = my_windows[GRAPHICS_WINDOW].full_size.screen_row;


	my_ugout(0x1d);

	joy_x = (int)((new_x - graphics_col)*virtual_size/actual_size) - 2048;
	joy_y = (int)((new_y - graphics_row)*virtual_size/actual_size) - 2048;


	if(joy_x > 2047)
		joy_x = 2047;

	if(joy_x < -2048)
		joy_x = -2048;

	if(joy_y > 2047)
		joy_y = 2047;

	if(joy_y < -2048)
		joy_y = -2048;



	if((pen_press & 1) != 0)
		my_ugout(0x41);
	else
		my_ugout(0x40);


			most_x = (joy_x >> 6) & 0x3f;
			least_x = joy_x & 0x3f;
			most_y = (joy_y >> 6) & 0x3f;
			least_y = joy_y & 0x3f;

			if(joy_x == -2048)
				x_y_6_bit = 0x00;
			else if(joy_x == -1)
				x_y_6_bit = 0x02 << 3;
			else if(joy_x < 0)
				x_y_6_bit = 0x01 << 3;
			else if(joy_x == 0)
				x_y_6_bit = 0x03 << 3;
			else if(joy_x == 1)
				x_y_6_bit = 0x05 << 3;
			else if(joy_x == 2047)
				x_y_6_bit = 0x07 << 3;
			else
				x_y_6_bit = 0x06 << 3;



			if(joy_y == -2048)
				x_y_6_bit |= 0x00;
			else if(joy_y == -1)
				x_y_6_bit |= 0x02;
			else if(joy_y < 0)
				x_y_6_bit |= 0x01;
			else if(joy_y == 0)
				x_y_6_bit |= 0x03;
			else if(joy_y == 1)
				x_y_6_bit |= 0x05;
			else if(joy_y == 2047)
				x_y_6_bit |= 0x07;
			else
				x_y_6_bit |= 0x06;



			my_ugout(most_x | 0x40);
			my_ugout(least_x | 0x40);
			my_ugout(most_y | 0x40);
			my_ugout(least_y | 0x40);
			my_ugout(x_y_6_bit | 0x40);
			my_ugout(0x0d);
	
}










void  joystick()
{
int ch,joy_count;


	joy_count = initial_joy_count;


read_again:

	if(check_xy() == 't')
		return;

	read_new_xy();



/*
printf("JOYSTICK() LOOP  :  new_x = %d , new_y = %d  buttons = %d\n",
	new_x,new_y,pen_press);
*/

	if((pen_press & 1) != 0)
	{
		pen2_x = old_x;
		pen2_y = old_y;
		pen_press2 = 't';
		joy_off = 't';
	}

	if((pen_press & 2) != 0)
		my_send_ext(60);

        if((pen_press & 4) != 0)
		my_send_ext(61);



/*    Removed this IF because it ruins  Drag & zoom/pan Combos
	(Can't remember why this if is Here Anyway) 

	if((locate_called == 'f') || (curser_displayed == 't'))
	{
		new_x = mouse_x;
		new_y = mouse_y;
	}

new_x,new_y  NOW SET BY READ_INPUT_DEVICE

*/




	if(((pen_press & 1) != 0) && (read_joystick == 't'))
	{
/*
printf("Read Joystick : old_x = %d , old_y = %d    new_x = %d , new_y = %d\n",
	old_x,old_y,new_x,new_y);
*/
		read_joystick = 'f';
		return_sample();
		joy_enable = 'f';
		set_joystick(0);

		if((curser_displayed == 't') &&
			(locate_called == 't'))
		{
			display_locate_seg(OLD_X_Y);
		}

		save_joy_x = old_x;
		save_joy_y = old_y;

		if(curser_displayed == 't')
		{
			display_the_curser(OLD_X_Y);
		}

		curser_displayed = 'f';
		return;
	}


	if((curser_displayed == 't') && (crosshair_undrawn == 't'))
	{
		display_the_curser(NEW_X_Y);
		crosshair_undrawn = 'f';

		if(locate_called == 't')
			display_locate_seg(NEW_X_Y);

		update_gmark();

		old_x = new_x;
		old_y = new_y;

		if(joy_count > 0)
			goto read_again;
	}
	else
	{
		if((old_x != new_x) || (old_y != new_y))
		{
/*
printf("Joystick Main : old_x = %d , old_y = %d    new_x = %d , new_y = %d\n",
	old_x,old_y,new_x,new_y);
*/

			if((curser_displayed == 't') &&
				(locate_called == 't'))
			{
				display_locate_seg(OLD_X_Y);
				display_locate_seg(NEW_X_Y);
			}

			save_joy_x = old_x;
			save_joy_y = old_y;


			if(curser_displayed == 't')
			{
				display_the_curser(OLD_X_Y);
				display_the_curser(NEW_X_Y);
			}

			update_gmark();

			old_x = new_x;
			old_y = new_y;

			joy_count = initial_joy_count - 1;
			goto read_again;
		}
		else    /* new_x == old_x && new_y == old_y */
		{
			if(joy_count == initial_joy_count)
				return;
			else
			{
				if(joy_count > 0)
				{
					joy_count--;
					goto read_again;
				}
				else
					return;
			}

		}

	}

}





void  p_func(sub_count,which_subr) /*enable something*/
int *sub_count,which_subr;
{

	read_more(1);
	a_char = megatek_data[0];


	switch(a_char & 0x7f)
	{
		case '@' : /*enable keyboard*/
		{

			read_more(1);

				diag_num = 17;
#ifdef HAVE_DIAGS
				my_diag("enable keyboard\n");
#endif

			break;
		}

		case 'A' : /*enable function pad*/
		{
			diag_num = 18;
#ifdef HAVE_DIAGS
			my_diag("enable function pad\n");
#endif
			break;
		}

		case 'B' : /*enable joystick*/
		{

			read_more(1);

				diag_num = 19;
#ifdef HAVE_DIAGS
				my_diag("enable joystick:\n");
#endif
				diag_num = -19;


				if(rotate_mode == 'm')
				{
					if((where_is_caret == IS_PAN) ||
						(where_is_caret == IS_ZOOM))
					{
						new_x = mouse_x = joy_x_sample;
						new_y = mouse_y = joy_y_sample;
					}
				}

				save_my_x = my_x;
				save_my_y = my_y;

				if((megatek_data[0] & 32) != 0)
				{  /*initialize curser at screen center*/
#ifdef HAVE_DIAGS
					my_diag("initialize curser at screen center\n");
#endif
					mouse_x = new_x = old_x = 385;
					mouse_y = new_y = old_y = 383;
				}
				else /*initialize curser at last curser position*/
				{
#ifdef HAVE_DIAGS
					my_diag("initialize curser at last curser pos.\n");
#endif

				}


				if((megatek_data[0] & 1) != 0)
				{ /*curser is displayed*/
#ifdef HAVE_DIAGS
					my_diag("curser is displayed\n\n");
#endif
					curser_displayed = 't';
				}
				else /*curser is not displayed*/
				{
#ifdef HAVE_DIAGS
					my_diag("curser is not displayed\n\n");
#endif
					curser_displayed = 'f';
				}

				if(joy_enable == 'f')
				{
					joy_enable = 't';

					set_joystick(1);

					if((curser_displayed == 't') &&
						(locate_called == 't'))
					{
						display_locate_seg(OLD_X_Y);
					}

					save_joy_x = old_x;
					save_joy_y = old_y;

					if(curser_displayed == 't')
					{
		if((is_inside_menu(MAIN_MENU,new_y,new_x) == 'f') &&
			(is_inside_menu(LIGHTS,new_y,new_x) == 'f'))
		{
						display_the_curser(OLD_X_Y);
		}
		else
			crosshair_undrawn = 't';
					}

					my_x = save_my_x;
					my_y = save_my_y;
				}


			break;
		}

		case 'C' : /*enable digitizer*/
		{

			read_more(1);
				
				diag_num = 20;
#ifdef HAVE_DIAGS
				my_diag("enable digitizer\n");
#endif


				if((megatek_data[0] & 32) != 0)
				{  /*initialize curser at screen center*/
#ifdef HAVE_DIAGS
					my_diag("initialize curser at screen center\n");
#endif
					digitizer_x = new_x = old_x = 385;
					digitizer_y = new_y = old_y = 383;
				}
				else /*initialize curser at last curser position*/
				{
#ifdef HAVE_DIAGS
					my_diag("initialize curser at last curser pos.\n");
#endif

				}


				if((megatek_data[0] & 1) != 0)
				{ /*curser is displayed*/
#ifdef HAVE_DIAGS
					my_diag("curser is displayed\n\n");
#endif
					digitizer_curser_enabled = 't';
				}
				else /*curser is not displayed*/
				{
#ifdef HAVE_DIAGS
					my_diag("curser is not displayed\n\n");
#endif
					digitizer_curser_enabled = 'f';
				}

				if(digitizer_enabled == 'f')
				{
					digitizer_enabled = 't';

					set_joystick(1);

					if((digitizer_curser_enabled == 't') &&
						(locate_called == 't'))
					{
						display_locate_seg(OLD_X_Y);
					}

					save_joy_x = old_x;
					save_joy_y = old_y;

					if(digitizer_curser_enabled == 't')
					{
						display_the_curser(OLD_X_Y);
					}

					my_x = save_my_x;
					my_y = save_my_y;
				}


			break;
		}

		case 'D' : /*enable pick module*/
		{
			read_more(1);
		

				diag_num = 21;
#ifdef HAVE_DIAGS
				my_diag("enable pick module\n");
#endif

			break;
		}

		default : /* illegal code */
		{
			sprintf(diag_string,
				"Error in 1st arg of command p: arg1 = %c , %x\n",
				a_char,a_char);
			error_logger(diag_string);
			break;
		}

					
	}

}





void  y_func(sub_count,which_subr)  /*locate something*/
int *sub_count,which_subr;
{

	read_more(1);
	a_char = megatek_data[0];

		switch(a_char & 0x7f)
		{
			case 'B' : /*locate joystick pos.*/
			{
				diag_num = 46;
#ifdef HAVE_DIAGS
				my_diag("locate joystick pos\n");
#endif

				an_element.op_code = 90;

				if(fill_sub == 't')
					insert_sub(an_element,which_sub);
				else
				{
					insert_ours(an_element,which_cseg);
					my_x = save_joy_x;
					my_y = save_joy_y;
					locate_called = 't';
					locate_seg = which_cseg;

					if(is_a_seg_open == 't')
						no_draw_flag = 't';
					else
						no_draw_flag = 'f';

					draw_mode(3);
				}

				break;
			}
	
			case 'C' : /*locate digitizer pos.*/
			{
				diag_num = 47;
#ifdef HAVE_DIAGS
				my_diag("locate digitizer pos\n");
#endif


				an_element.op_code = 90;

				if(fill_sub == 't')
					insert_sub(an_element,which_sub);
				else
				{
					insert_ours(an_element,which_cseg);
					my_x = save_joy_x;
					my_y = save_joy_y;
					locate_called = 't';
					locate_seg = which_cseg;

					if(is_a_seg_open == 't')
						no_draw_flag = 't';
					else
						no_draw_flag = 'f';

					draw_mode(3);
				}



				break;
			}

			default : /*error*/
			{
				sprintf(diag_string,
					"Error in 2nd byte of command y\n");
				error_logger(diag_string);
				break;
			}
		}
	
}






void  v_func(sub_count,which_subr) /*select something*/
int *sub_count,which_subr;
{
		read_more(3);

		diag_num = 87;

		a_subroutine = comp_rel_coord(megatek_data[1],megatek_data[2]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"select something from subroutine %d\n",
			a_subroutine);
		my_diag(diag_string);
#endif
		diag_num = -87;

		switch(megatek_data[0] & 0x07)
		{
			case 2 : /*select joystick curser*/
			{
#ifdef HAVE_DIAGS
				my_diag("select joystick curser\n");
#endif
				break;
			}

			case 3 : /*select digitizer curser*/
			{
#ifdef HAVE_DIAGS
				my_diag("select digitizer curser\n");
#endif
				break;
			}

			case 4 : /*select light pen curser*/
			{
#ifdef HAVE_DIAGS
				my_diag("select light pen curser\n");
#endif
			}
		}
	
}




