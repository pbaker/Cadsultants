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







/* January 5, 1990  Joystick():      REMOVED THE 2

	if((locate_called == 'f') || (curser_displayed == 't'))

conditions because they prevent updating of mouse variables in Drag & Zoom/Pan
Combinations
	(Can't remember why I wanted those ifs anyway)
*/






#include "my_globs.h"
#include "my_utils.h"
#include "math.h"


#define X_MIN -2048
#define X_MAX 2047
#define Y_MIN -2047
#define Y_MAX 2048



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

extern float far res_factor;

#define ZOOM_Y_OFF  comp_zoom_y_off()



extern int far keyboard_status();
extern int far current_color;
extern int far save_system_color;

int far joy_x_sample,far joy_y_sample;
int far last_mouse_x,far last_neg_mouse_y;
int far last_mouse_y;
int far where_is_caret = NO_CARET;

int far initial_joy_count = 600;


void far joystick();

int far comp_zoom_y_off()
{
	if(res_factor == 1.0)
		return(5);
	else
		return(0);
}




int far number_of_segs_visible()
{
int count,i;

	count = 0;

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if(headers_segs[i].visibility == 't')
			count++;


	return(count);
}






int far my_sign(i)
int i;
{
	if(i > 0)
		return(1);
	else if(i < 0)
		return(-1);
	else
		return(0);
}







int far alt_z_caret_position()
{
int row,col;
char the_char;

	col = 7;


	for(row = 2 ; row < 12 ; row++)
	{
	/*position curser*/
		my_input_regs.h.ah = 2;
		my_input_regs.h.bh = 0;
		my_input_regs.h.dh = row;
		my_input_regs.h.dl = col;
		int86(0x10,&my_input_regs,&my_output_regs);


	/*read the character at the curser position*/
		my_input_regs.h.ah = 8;
		my_input_regs.h.bh = 0;

		int86(0x10,&my_input_regs,&my_output_regs);

		the_char = my_output_regs.h.al;

		if(the_char == '>')
			break;
	}



	switch(row)
	{
		case 3 :
			return(IS_PAN);

		case 4 :
			return(IS_ZOOM);

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







void far read_mouse()
{
int dx,dy;
int ddx,ddy;


	my_input_regs.x.ax = 11;     /*raw mouse movements*/
	int86(0x33,&my_input_regs,&my_output_regs);

	ddx = dx = my_output_regs.x.cx;
	ddy = dy = my_output_regs.x.dx;


#ifdef rkekeflefellg

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


/*printf("read_mouse(),after raw stuff dx = %d , dy = %d , curser_ratio = %d\n",
		dx,dy,curser_ratio);
*/

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

/*printf("after scaling dx = %d , dy = %d , curser_ratio = %d\n",
		dx,dy,curser_ratio);
printf("before updating  mouse_x = %d , mouse_y = %d\n",
	mouse_x,mouse_y);
*/

	mouse_x += dx;
	mouse_y += dy;

/*printf("after updating  mouse_x = %d , mouse_y = %d\n",
	mouse_x,mouse_y);
*/

	if(mouse_x > X_MAX)
		mouse_x = X_MAX;
	else if(mouse_x < X_MIN)
		mouse_x = X_MIN;

	if(mouse_y > Y_MAX)
		mouse_y = Y_MAX;
	else if(mouse_y < Y_MIN)
		mouse_y = Y_MIN;


	my_input_regs.x.ax = 3;
	int86(0x33,&my_input_regs,&my_output_regs);

	mouse_phase = my_output_regs.x.bx;

	if(mouse_phase == 0)
	{
		if(depress_flag == 'f')
		{
			pen_press = 0;
			button_byte = 0;
		}
		else
		{
			depress_flag = 'f';
			pen_press = button_byte;
			button_byte = 0;
		}
	}
	else
	{
		depress_flag = 't';
		button_byte = mouse_phase;
	}

}





int far positive_only(i)
int i;
{
	if(i > 0)
		return(i);
	else
		return(0);
}






void far my_zoom_pan(which_one)
char which_one;
{
	read_mouse();

	new_x = mouse_x;
	new_y = mouse_y;

	if(which_one == 'z')
	{
		if(last_neg_mouse_y != 0)  /*mouse button 1 pushed*/
		{
			if(my_sign(last_neg_mouse_y) != my_sign(new_y))
			{
				if(abs(new_y) > positive_only(curser_ratio))
					mouse_y = new_y = old_y;
					
			}
		}
	}



	if((new_x != old_x) || (new_y != old_y))
	{
		if(which_one == 'p')
		{
			my_pan_box(old_x,old_y);
			my_pan_box(new_x,new_y);
		}
		else if(which_one == 'z')
		{
			my_zoom_box(old_y);
			my_zoom_box(new_y);
		}

		old_x = new_x;
		old_y = new_y;			
	}
}







void comp_sub_data(sub_num_index,sub_end,sub_num)
int *sub_num_index,*sub_end,sub_num;
{
int i;

	for(i = 0 ; (i < 20) && (sub_nums[i] != sub_num) ; i++)
		;

	*sub_num_index = i;
	*sub_end = sub_insert_ptrs[i];
}





void far compile_sub_curser(sub_num)
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
				insert_sub(an_element,NUMBER_OF_SUBS);
			}
			else
				compile_sub_curser(ptr_sub -> arg1);
		}
	}
}





void far q_func(sub_count,which_subr)  /*disable something*/
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

					save_my_x = my_x;
					save_my_y = my_y;

					save_joy_x = old_x;
					save_joy_y = -old_y;
					draw_mode(0);
					display_a_seg(locate_seg);
					draw_mode(0);

					my_x = save_my_x;
					my_y = save_my_y;
				}

				save_joy_x = old_x;
				save_joy_y = -old_y;


				if(curser_displayed == 't')
				{
					save_my_x = my_x;
					save_my_y = my_y;
	
					my_x = old_x;
					my_y = -old_y;

					save_line_type = line_type;
					my_dash_enable(0);
				/*	scolr(0x0f);*/
					draw_mode(3);

					interp_my_sub6();

					draw_mode(0);
					my_color(save_color);
					line_type = save_line_type;
					my_dash_enable(line_type);

					my_x = save_my_x;
					my_y = save_my_y;
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






void far my_pan_menu()
{
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
}



void far my_zoom_menu()
{
	read_curser_pos();
	turn_off_curser();
	make_win(10,17,57,23);
	save_win();

	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	if(old_y < 0)
		printf("   Zoom In");
	else
		printf("   Zoom Out");	

	goto_row_col(win_y_low + 2,win_x_low + 1);
	printf("   Move Mouse Up/Down");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("   Button #1 To Update View");
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("   Button #2 To Reset View");
}



void far my_rotate_menu()
{
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
}





void far remove_xform_menu()
{
	restore_win();   /*retrieve stored screen information*/

	goto_row_col(row_curser,col_curser);
	turn_on_curser();

	make_win(10,3,60,17);  /*restore Home Menu's Window*/
}



void far fixed_rotations_message()
{
	if((where_is_caret == IS_FIXED_CONTINUOUS) ||
		(where_is_caret == IS_FIXED_CONTROLLED))
	{
		goto_row_col(win_y_low + 1,win_x_low + 35);
		printf("X = %-5d ",new_x/4);
		goto_row_col(win_y_low + 2,win_x_low + 35);
		printf("Y = %-5d ",-new_y/4);

	}
}




void far r_func(sub_count,which_subr) /*sample something*/
int *sub_count,which_subr;
{
unsigned char most_x,most_y,least_x,least_y,x_y_6_bit;
int joy_x,joy_y;
long temp;
int my_keyboard_status;
char reset_mouse_variables;


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

					my_pan_box(old_x,old_y);

					while((pen_press & 1) == 0)
					{
						my_zoom_pan('p');

						if((pen_press & 4) != 0)
						{
							reset_mouse_variables
								= 't';
							break;
						}

						my_keyboard_status = 
							keyboard_status();

						if(my_keyboard_status != -1)
							break;
					}

					my_pan_box(old_x,old_y);


					remove_xform_menu();


					if(reset_mouse_variables == 't')
					{
						mouse_x = mouse_y = 0;
						new_y = old_y = 0;
						new_x = old_x = 0;
						last_neg_mouse_y = 0;
					}


					break;
				}


				case IS_ZOOM :
				{
					my_zoom_menu();

					my_zoom_box(old_y);

					while((pen_press & 1) == 0)
					{
						my_zoom_pan('z');

						goto_row_col(win_y_low + 1,
							win_x_low + 1);
						if(old_y < 0)
							printf("   Zoom In  ");
						else
							printf("   Zoom Out ");


						if((pen_press & 4) != 0)
						{
							reset_mouse_variables
								= 't';
							break;
						}

						my_keyboard_status = 
							keyboard_status();

						if(my_keyboard_status != -1)
							break;
					}

					my_zoom_box(old_y);


					remove_xform_menu();


					if(reset_mouse_variables == 't')
					{
						mouse_y = 0;
						new_y = old_y = 0;
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
						my_keyboard_status = 
							keyboard_status();

						if(my_keyboard_status != -1)
							break;

						joystick();

						fixed_rotations_message();


						if((pen_press & 4) != 0)
						{
							mouse_x = mouse_y = 0;
							new_y = old_y = 0;
							new_x = old_x = 0;
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
				joy_x =  joy_x_sample;
				joy_y = -joy_y_sample;
			}
			else
			{
				joy_x = new_x;
				joy_y = -new_y;
			}


/*
   if mouse button 1 was pressed, update joystick coordinates,
   & update sample coordinates.  Note Y coordinate inversion
   when sent to host, but not when stored for reference.
*/

			if(my_keyboard_status == -1)
			{
				joy_x = new_x;
				joy_y = -new_y;

				joy_x_sample = new_x;
				joy_y_sample = new_y;

/*save last mouse position*/
				last_mouse_x = new_x;
				last_mouse_y = new_y;
				last_neg_mouse_y = new_y;
			}
		   }
		   else  /*auto mode*/
		   {
			joy_x = new_x;
			joy_y = -new_y;
		   }


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
			else if(joy_x == 2048)
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




void far s_func(sub_count,which_subr)  /*read something*/
int *sub_count,which_subr;
{
unsigned char x_y_6_bit,most_x,most_y,least_x,least_y;
int joy_x,joy_y;
char change_disp;

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
					mouse_x = new_x = old_x = 0;
					mouse_y = new_y = old_y = 0;
				}
				else /*initialize curser at last curser position*/
				{
				#ifdef HAVE_DIAGS
					my_diag("init curser at last curser pos\n");
				#endif

				}

				if((megatek_data[0] & 8) != 0)
				{ /*"rubber band" position displayed between screen
					origin and curser position*/
				#ifdef HAVE_DIAGS
					my_diag("\"rubber band\" positioning\n");
				#endif
				}
				else /*no positioning information displayed*/
				{
				#ifdef HAVE_DIAGS
					my_diag("no positioning info displayed\n");
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
					save_my_x = my_x;
					save_my_y = my_y;

					save_joy_x = old_x;
					save_joy_y = -old_y;
					draw_mode(0);
					display_a_seg(locate_seg);
					draw_mode(0);

					my_x = save_my_x;
					my_y = save_my_y;
				    }
				}

			save_joy_x = old_x;
			save_joy_y = -old_y;


			if((joy_enable == 'f') || (change_disp == 't'))
			{
				joy_enable = 't';

				set_joystick(1);
			#ifdef HAVE_DIAGS
				my_diag("READ JOYSTICK DRWMOD(3)\n");
			#endif

				if(curser_displayed == 't')
				{
					save_my_x = my_x;
					save_my_y = my_y;
					save_line_type = line_type;
					my_dash_enable(0);

					my_x = old_x;
					my_y = -old_y;
	
					draw_mode(3);
		
					interp_my_sub6();

					draw_mode(0);
					my_color(save_color);
					line_type = save_line_type;
					my_dash_enable(line_type);

					my_x = save_my_x;
					my_y = save_my_y;
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




void far t_func(sub_count,which_subr) /*set something*/
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




void far w_func(sub_count,which_subr) /*set something*/
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

	my_ugout(0x1d);

	joy_x = new_x;
	joy_y = -new_y;

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
			else if(joy_x == 2048)
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






void far joystick()
{
int ch,joy_count;

	joy_count = initial_joy_count;

read_again:
	read_mouse();

	if((pen_press & 1) != 0)
	{
		pen2_x = old_x;
		pen2_y = old_y;
		pen_press2 = 't';
		joy_off = 't';
	}


        if((pen_press & 4) != 0)
        {	
                button1 = 't';
                matrix_update();  /*to update display when matrix has been
                                        inserted*/
        }



/*
     Removed this IF because it ruins  Drag & zoom/pan Combos
	(Can't remember why this if is Here Anyway) 

	if((locate_called == 'f') || (curser_displayed == 't'))
	{
		new_x = mouse_x;
		new_y = mouse_y;
	}
*/

	new_x = mouse_x;
	new_y = mouse_y;


	if(((pen_press & 1) != 0) && (read_joystick == 't'))
	{
		read_joystick = 'f';
		return_sample();
		joy_enable = 'f';
		set_joystick(0);

				if((curser_displayed == 't') &&
					(locate_called == 't'))
				{

					save_my_x = my_x;
					save_my_y = my_y;

					save_joy_x = old_x;
					save_joy_y = -old_y;
					draw_mode(0);
					display_a_seg(locate_seg);
					draw_mode(0);

					my_x = save_my_x;
					my_y = save_my_y;
				}

				save_joy_x = old_x;
				save_joy_y = -old_y;


		if(curser_displayed == 't')
		{
			save_my_x = my_x;
			save_my_y = my_y;
			my_x = old_x;
			my_y = -old_y;

			save_line_type = line_type;
			my_dash_enable(0);
		/*	scolr(0x0f);*/
			draw_mode(3);

			interp_my_sub6();

			draw_mode(0);
			my_color(save_color);
			my_dash_enable(save_line_type);
			line_type = save_line_type;

			my_x = save_my_x;
			my_y = save_my_y;
		}

		return;
	}

	if((old_x != new_x) || (old_y != new_y))
	{


				if((curser_displayed == 't') &&
					(locate_called == 't'))
				{

					save_my_x = my_x;
					save_my_y = my_y;

					save_joy_x = old_x;
					save_joy_y = -old_y;
					draw_mode(0);
					display_a_seg(locate_seg);
					draw_mode(0);

					save_joy_x = new_x;
					save_joy_y = -new_y;
					display_a_seg(locate_seg);
					draw_mode(0);

					my_x = save_my_x;
					my_y = save_my_y;
				}

				save_joy_x = old_x;
				save_joy_y = -old_y;


		if(curser_displayed == 't')
		{
			save_my_x = my_x;
			save_my_y = my_y;
			save_line_type = line_type;

			my_dash_enable(0);
		/*	scolr(0x0f);*/
			draw_mode(3);

			my_x = old_x;
			my_y = -old_y;
	
			interp_my_sub6();

			my_x = new_x;
			my_y = -new_y;
	
			interp_my_sub6();


			draw_mode(0);
			my_color(save_color);
			line_type = save_line_type;
			my_dash_enable(line_type);
			

			my_x = save_my_x;
			my_y = save_my_y;
		}



/*
    Removed this IF because it ruins  Drag & zoom/pan Combos
	(Can't remember why this if is Here Anyway) 
		if((locate_called == 'f') || (curser_displayed == 't'))
		{
			old_x = new_x;
			old_y = new_y;
		}
*/

		old_x = new_x;
		old_y = new_y;


		joy_count = initial_joy_count - 1;
		goto read_again;
	}
	else
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





void far p_func(sub_count,which_subr) /*enable something*/
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

					mouse_x = new_x = old_x = 0;
					mouse_y = new_y = old_y = 0;
				}
				else /*initialize curser at last curser position*/
				{
				#ifdef HAVE_DIAGS
					my_diag("initialize curser at last curser pos.\n");
				#endif

				}

				if((megatek_data[0] & 8) != 0)
				{ /*"rubber band" position displayed between screen
					origin and curser position*/
				#ifdef HAVE_DIAGS
					my_diag("\"rubber band\" positioning\n");
				#endif
				}
				else /*no positioning information displayed*/
				{
				#ifdef HAVE_DIAGS
					my_diag("no pos. info displayed\n");
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
						save_my_x = my_x;
						save_my_y = my_x;

						save_joy_x = old_x;
						save_joy_y = -old_y;
						draw_mode(0);
						display_a_seg(locate_seg);
						draw_mode(0);

						my_x = save_my_x;
						my_y = save_my_y;
					}

					save_joy_x = old_x;
					save_joy_y = -old_y;

					if(curser_displayed == 't')
					{
						save_line_type = line_type;
						my_dash_enable(0);
					/*	scolr(0x0f);*/
						draw_mode(3);

						save_my_x = my_x;
						save_my_y = my_x;

						my_x = old_x;
						my_y = -old_y;

						interp_my_sub6();

						draw_mode(0);
						my_color(save_color);
						line_type = save_line_type;
						my_dash_enable(line_type);
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





void far y_func(sub_count,which_subr)  /*locate something*/
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






void far v_func(sub_count,which_subr) /*select something*/
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




