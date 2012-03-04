/* MENUS.C  Home menus*/



#include "my_globs.h"
#include "my_utils.h"
#include "c:\emulate\clipper\pw_lib.h"


/*cursor limits*/
#define X_MAX 2048
#define X_MIN -2047
#define Y_MAX 2048
#define Y_MIN -2047

#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1
#define RES_1024_768 3
#define RES_1280_1024 4


extern short far text_monitor_dl[];
extern short far *text_dl_pointer;



extern void far clear_text_monitor_dl();



extern char far licensee[];
extern char far serial[];


extern void far control_window();
extern char far stop_after_matrix_sub;

extern void far clear_error_vars();


extern void far r_flag(int);
extern void far w_flag(int);
extern void far my_reset();
extern FILE far *com_save;
extern FILE far *com_read;




extern int far unity_viewing_matrix[];
extern int far init_baud_rate;
extern char far which_high_res;
extern char far ems_mode;
extern char far version_str[];
extern int far rw_threshold;
extern void far hpgl_menu(void);
extern void far print_id();
extern int far number_of_errors;



char far *invalid_file[2] =
{
	"Invalid File Name",
	"  "
};




void far enforce_integer_range(value,bottom_range,top_range)
int far *value,bottom_range,top_range;
{
	if(*value < bottom_range)
		*value = bottom_range;

	if(*value > top_range)
		*value = top_range;
}




void far read_menu_integer(header_string,value,bottom_range,top_range)
char far header_string[];
int far *value;
int bottom_range,top_range;
{
int i,ch;
char the_string[33],temp_string[80];
int row,col;

	make_win(10,3,60,17);

	sprintf(temp_string,"Enter New Value [%d..%d] : ?",bottom_range,
		top_range);

	row = win_y_low + 3;
	col = win_x_low + 1 + strlen(temp_string); 

start_over:
	make_win(10,3,60,17);
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 2);
	printf("%s = %d",header_string,*value);
	goto_row_col(win_y_low + 3,win_x_low + 2);
	printf("%s",temp_string);


	control_window();
	make_win(10,3,60,17);

	ch = getch();

	if(ch == 0)
	{
		ch = getch();

		switch(ch)
		{
			case 75 : 
			case 77 : 
			case 80 :
				return;

			default :
				goto start_over;
		}
	}
	else
	{
		if((ch >= '0') && (ch <= '9'))
		{
			goto_row_col(row,col);
			putchar(ch);
			the_string[0] = ch;
		}
		else
			goto start_over;

	}



	i = 1;
	for(ch = getch() ; (ch != 13) /*enter key*/
		 && (ch != 0) ; ch = getch())
	{
		if(ch != 8)
		{
			if((ch >= '0') && (ch <= '9'))
			{
				putchar(ch);
				the_string[i] = ch;
				i++;
			}
		}
		else
		{
			if(i > 0)
			{
				i--;

				goto_row_col(row,col + i);
				putchar(' ');
				goto_row_col(row,col + i);
			}
		}
	}


	the_string[i] = '\0';


	if(ch == 0)
	{	ch = getch();

		switch(ch)
		{
			case 75 : /*reject*/
			{
				goto start_over;
			}

			case 77 : /*entry complete*/
			{
				*value = atoi(the_string);
				enforce_integer_range(value,bottom_range,
					top_range);

				break;
			}


			case 80 : /*terminate*/
				return;
		}
	}
	else
	{
		*value = atoi(the_string);
		enforce_integer_range(value,bottom_range,top_range);

		goto start_over;
	}
}







void far read_menu_string(header_string,return_string)
char far header_string[];
char far return_string[];
{
int i,ch;
int row,col;

	make_win(10,3,60,17);

	row = win_y_low + 3;
	col = win_x_low + 7;

start_over:
	make_win(10,3,60,17);
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 2);
	printf("%s",header_string);



	control_window();
	make_win(10,3,60,17);
	goto_row_col(win_y_low + 3,col);

	ch = getch();

	if(ch == 0)
	{
		ch = getch();

		switch(ch)
		{
			case 75 : 
			case 77 : 
			case 80 :
			{
				return_string[0] = '\0';
				return;
			}

			default :
				goto start_over;
		}
	}
	else
	{
		return_string[0] = ch;
		putchar(ch);
	}



	i = 1;
	for(ch = getch() ; (ch != 13) /*enter key*/
		 && (ch != 0) ; ch = getch())
	{
		if(ch != 8)
		{
			putchar(ch);
			return_string[i] = ch;
			i++;
		}
		else
		{
			if(i > 0)
			{
				i--;

				goto_row_col(row,col + i);
				putchar(' ');
				goto_row_col(row,col + i);
			}
		}
	}


	return_string[i] = '\0';


	if(ch == 0)
	{	ch = getch();

		switch(ch)
		{
			case 75 : /*reject*/
			{
				goto start_over;
			}

			case 77 : /*entry complete*/
				break;
	
			case 80 : /*terminate*/
			{
				return_string[0] = '\0';
				break;
			}
		}
	}
}







void far start_recording()
{
	if(com_save == NULL)
	{
		read_menu_string("Enter Record File Name",
			out_string);
	
		if(out_string[0] == '\0')
			return;

		com_save = fopen(out_string,"w");
		if(com_save == NULL)
		{
			make_win(10,3,60,17);
			putchar(7);
			error_message(6,invalid_file,1);
		}
		else
			w_flag(1);

	}

}



void far stop_recording()
{
	if(com_save != NULL)
	{
		fclose(com_save);
		com_save = NULL;
		w_flag(0);
	}

}



void far start_playback()
{
	if(com_read == NULL)
	{
		read_menu_string("Enter Playback File Name",
			out_string);
	
		if(out_string[0] == '\0')
			return;

		com_read = fopen(out_string,"r");
		if(com_read == NULL)
		{
			make_win(10,3,60,17);
			putchar(7);
			error_message(6,invalid_file,1);
		}
		else
		{
			r_flag(1);
			my_reset();
		}

	}

}



char far *stop_recording_text[2] =
{
	"Stop Recording",
	"aaa"
};



void far recording_menu()
{
int selection;

	selection = choose_menu("Record/Playback",stop_recording_text,1,1);

	switch(selection)
	{
		case ENTRY_COMPLETE :
		case 1 : /* close save file */
		{
			stop_recording();
			break;
		}

		case REJECT :
		case TERMINATE :
			break;
	}
}




char far *playing_text[3] =
{
	"Record/Playback",
	"\0",
	"   Playback In Progress"
};


void far playing_menu()
{
	make_win(10,3,60,17);
	clear_win(REVERSE_VIDEO);
	draw_box();

	error_message(1,playing_text,3);
}



char far *start_com_text[2] =
{
	"Start Recording",
	"Playback"
};



void far start_com_menu()
{
int selection;

	selection = choose_menu("Record/Playback",start_com_text,2,0);

	switch(selection)
	{
		case 1 : /* open save file */
		{
			start_recording();
			break;
		}

		case 2 : /* read saved host file */
		{
			start_playback();
			break;
		}


		case REJECT :
		case ENTRY_COMPLETE :
		case TERMINATE :
			break;
	}

}




void far handle_com_files()
{
	if((com_save == NULL) && (com_read == NULL))
		start_com_menu();
	else if((com_save != NULL) && (com_read == NULL))
		recording_menu();
	else if((com_save == NULL) && (com_read != NULL))
		playing_menu();
	else
	{
		putchar(7); putchar(7);
		printf("\n\nIMPOSSIBLE: Playback & Record at SAME TIME!!!\n\n");
		printf("Press Enter\n\n");
		getch();
	}
}


char far *error_log_text[] = 
{
	"List Errors",
	"Print Errors",
	"Clear Errors",
	"Record/Playback"
};


char far error_log_status()
{
int selection;

error_log_again:
	selection = choose_menu("Diagnostics",error_log_text,4,0);

/* undo make_win( , , , ) used by the control window */
	make_win(10,3,60,17);


	switch(selection)
	{
		case 1 : /*display it*/
		{
			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"a");
			fprintf(error_log_ptr,"\nFile Listed:\n\n");
			fclose(error_log_ptr);

			restore_win();
			make_win(0,0,79,24);
			save_win();
			clear_display_page();
			goto_row_col(0,0);
			sprintf(diag_string,"type %serrorlog.dat | more",
				path_error_log);
			system(diag_string);
			printf("\n\nPress Enter To Continue\n");
			getch();
			restore_win();
			make_win(10,3,60,17);
			save_win();

			clear_error_vars();

			goto error_log_again;

			break;
		}

		case 2 : /*print it*/
		{
			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"a");
			fprintf(error_log_ptr,"\nFile Printed:\n\n");
			fclose(error_log_ptr);

			restore_win();
			make_win(0,0,79,24);
			save_win();
			clear_display_page();
			goto_row_col(0,0);
			sprintf(diag_string,"print %serrorlog.dat",
				path_error_log);
			system(diag_string);
			printf("\n\nPress Enter To Continue\n");
			getch();
			restore_win();
			make_win(10,3,60,17);
			save_win();

			clear_error_vars();

			goto error_log_again;

			break;
		}

		case 3 : /*clear it*/
		{
			number_of_errors = 0;

			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"w");
			fprintf(error_log_ptr,"File Cleared:\n\n");
			fclose(error_log_ptr);

			clear_error_vars();

			goto error_log_again;

			break;
		}


		case 4 : /*com files*/
		{
			handle_com_files();
			goto error_log_again;
			break;
		}


		case REJECT : 
		case ENTRY_COMPLETE : 
			return('c');

		case TERMINATE : 
			return('e');

	}


	return('c');
}





char far *config_text[10] = 
{
	"     Controller : PixelWorks Clipper  \0                      ",
	"     PC WorkWindow Version: 1.30    xx/xx/xx                 ",
	"     Emulation Mode :   D135a   \0                            ",
	"\0                                                              ",
	"     Licensee :  anyone at all                               ",
	"     Serial ID : 123454                                      ",
	"\0                                                              ",
	"     Screen Resolution = 1024  x  768                        ",
	"     Display Memory Size = 2001 pages                        ",
	"     EMS Mode = Software                                     "
};



char far emulation_status()
{
int ch,i;
int display_size;
int selection;




	sprintf(config_text[1],"     PC WorkWindow Version: %s",
		version_str);
	sprintf(config_text[4],"     Licensee :  %s",licensee);

	sprintf(config_text[5],"     Serial ID :  %s",serial);

	if(which_high_res == RES_1024_768)
		sprintf(config_text[7],
			"     Screen Resolution = 1024 x 768");
	else
		sprintf(config_text[7],
			"     Screen Resolution = 1280 x 1024");


/*compute and display the number of logical pages in use*/
	display_size = 0;

	for(i = 0 ; i < 256 ; i++)
		if(available_pages[i] != -1)
			display_size++;

	sprintf(config_text[8],"     Display Memory Size = %d Pages",
		display_size);



	if(ems_mode == SOFTWARE_EMS)
		sprintf(config_text[9],"     EMS Mode = Software");
	else
		sprintf(config_text[9],"     EMS Mode = Hardware");



	selection = information_menu("Configuration Info",config_text,10,
		't','f');


	if(selection == TERMINATE)
		return('e');
	else
		return('c');

}





char far *cursor_text[9] =
{
	"Press Mouse/Joystick Buttons  \0                          " ,
	" \0                                                       " ,
	"  1  Increase Cursor Speed  \0                            " ,
	"  2  Decrease Cursor Speed  \0                            " ,
	"  (F1 To Center Cursor)   \0                              " ,
	"\0                                                        " ,
	"\0                                                        " ,
	"\0                                                        " ,
	"\0                                                        " 
};




char far cursor_speed_set()
{
int ch,save_current_color,x_mouse,y_mouse,dx,dy;


	my_set_viewport(-2048,-2048,2048,2048,which_page);
	pushvw3();    /*save current 3d viewing matrix*/
	vwmatx3(&unity_viewing_matrix[0]);  /*set 3d viewing matrix to unity*/

	x_mouse = 0;
	y_mouse = 0;
	dx = dy = 0;

	save_current_color = current_color;
	my_color(7);

	sprintf(cursor_text[4],"    (F1 To Center Cursor)    ");
	information_menu("Crosshair Control:",cursor_text,9,'f','f');

	pen_press = 0;

	draw_mode(3);
	my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
	my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);

	while((pen_press & 2) == 0)
	{
		if(keyboard_status() != -1)  /*keyboard was touched*/
		{
			ch = getch();
			if(ch == 0)    /*its an extended code*/
			{
				ch = getch();
				if(ch == 0x3b)
				{
					sprintf(cursor_text[4],
						"   Cursor Will Be Reset To 0 0 Upon Return");
					information_menu("Crosshair Control:",
						cursor_text,9,'f','t');

					mouse_x = 0;
					mouse_y = 0;
				}
				else if((ch == 75) || (ch == 77))
				{    /*undraw cursor speed curser*/
					my_line_xy(x_mouse - 50,y_mouse,x_mouse
						 + 50,y_mouse);
					my_line_xy(x_mouse,y_mouse - 50,x_mouse
						,y_mouse + 50);
					draw_mode(0);
					my_color(save_current_color);

					popvw3();   /*restore saved matrix*/

					stop_after_matrix_sub = 't';
					display_a_seg(which_cseg);
					stop_after_matrix_sub = 'f';

					return('c');
				}
				else if(ch == 80)  /*terminate*/
				{    /*undraw cursor speed curser*/
					my_line_xy(x_mouse - 50,y_mouse,x_mouse
						 + 50,y_mouse);
					my_line_xy(x_mouse,y_mouse - 50,x_mouse
						,y_mouse + 50);
					draw_mode(0);
					my_color(save_current_color);

					popvw3();    /*restore saved matrix*/

					stop_after_matrix_sub = 't';
					display_a_seg(which_cseg);
					stop_after_matrix_sub = 'f';

					return('e');
				}
			}
		}



		if(curser_ratio > 0)
			sprintf(cursor_text[7],
				"Cursor Speed = %d            ",curser_ratio);
		else if(curser_ratio < 0)
			sprintf(cursor_text[7],
				"Cursor Speed = %f",-1.0/curser_ratio);
		else
			sprintf(cursor_text[7],
				"Cursor Speed = 1             ");

		information_menu("Crosshair Control:",cursor_text,9,'f','t');


		my_input_regs.x.ax = 11;  /*raw mouse movements*/
		int86(0x33,&my_input_regs,&my_output_regs);

		dx = my_output_regs.x.cx;
		dy = -my_output_regs.x.dx;

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


		if((dx != 0) || (dy != 0))
		{    /*undraw curser speed curser*/
			my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
			my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);
		}


		x_mouse += dx;
		y_mouse += dy;

		if(x_mouse > X_MAX)
			x_mouse = X_MAX;
		else if(x_mouse < X_MIN)
			x_mouse = X_MIN;

		if(y_mouse > Y_MAX)
			y_mouse = Y_MAX;
		else if(y_mouse < Y_MIN)
			y_mouse = Y_MIN;

		if((dx != 0) || (dy != 0))
		{   /*redraw cursor speed cursor*/
			my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
			my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);
		}



		my_input_regs.x.ax = 3;   /*regular read mouse*/
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


		if((pen_press & 1) != 0)
		{
			curser_ratio++;
			if(curser_ratio == 0)
				curser_ratio = 1;
		}
		else if((pen_press & 4) != 0)
		{
			curser_ratio--;
			if(curser_ratio == 0)
				curser_ratio = -1;
		}
			
	}   /*end while*/

    /*undraw cursor speed curser*/
	my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
	my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);
	draw_mode(0);
	my_color(save_current_color);


	popvw3();   /*restore saved 3d viewing matrix & window/viewport*/

	stop_after_matrix_sub = 't';
	display_a_seg(which_cseg);
	stop_after_matrix_sub = 'f';

	return('c');
}




char far *switches_text[4] = 
{
	"Listing Size                                             ",
	"Printer Echo                                             ",
	"Screen Update Mode                                       ",
	"Update Timer                                             "
};



char far our_switches()
{
int selection;

	while(9)
	{
		if(list_size == 0)
			sprintf(switches_text[0],
				"Listing Size        :  Normal");
		else
			sprintf(switches_text[0],
				"Listing Size        :  Double");


		if(print_list == 't')
			sprintf(switches_text[1],
				"Printer Echo        :  Enabled");
		else
			sprintf(switches_text[1],
				"Printer Echo        :  Disabled");


		if(rotate_mode == 'a')
			sprintf(switches_text[2],
				"Screen Update Mode  :  Automatic");
		else
			sprintf(switches_text[2],
				"Screen Update Mode  :  Manual");


		if(time_redisplay == 't')
			sprintf(switches_text[3],
				"Update Timer        :  Enabled");
		else
			sprintf(switches_text[3],
				"Update Timer        :  Disabled");

		

		selection = choose_menu("Switches",switches_text,4,0);


		switch(selection)
		{
			case 1 : /*set size of listing characters*/
			{
			/* undraw listing & clear listing buffer */
				pw_dl_init(text_dl_pointer,0,NULL,0);
				dlend();
				pw_dl_init(NULL,0,NULL,0);

			        my_set_viewport(-2048,-2048,2048,2048,
					which_page);
				my_size_chars(list_size);

				draw_mode(3);
				dlrun(&text_monitor_dl[0]);
				draw_mode(0);

				clear_text_monitor_dl();


				if(list_size == 0)
					list_size = 1;
				else
					list_size = 0;

				break;
			}

			case 2 : /*send/don't send listings to the printer*/
			{
				if(print_list == 't')
					print_list = 'f';
				else
					print_list = 't';

				break;
			}


                        case 3 : /*auto/manual rotation*/
                        {
				if(rotate_mode == 'a')
					rotate_mode = 'm';
				else
					rotate_mode = 'a';

                                break;
                        }

			case 4 : /*time/don't time redisplays*/
			{
				if(time_redisplay == 'f')
					time_redisplay = 't';
				else
					time_redisplay = 'f';

				break;
			}

			case REJECT : 
			case ENTRY_COMPLETE :  
				return('c');

			case TERMINATE : 
				return('e');

		}  /*end switch*/

	}  /*end while*/
}






char far *the_baud_rates[] = 
{
	"1200",
	"2400",
	"4800",
	"9600",
	"19200"
};



char far enter_baud_rate()
{
int row,selection;

baud_again:

	switch(init_baud_rate)
	{
		case 96 :
		{
			row = 1;
			break;
		}

		case 48 :
		{
			row = 2;
			break;
		}

		case 24 :
		{
			row = 3;
			break;
		}

		case 12 :
		{
			row = 4;
			break;
		}

		case 6 :
		{
			row = 5;
			break;
		}
	}


	selection = choose_menu("Select Baud Rate",the_baud_rates,5,row);

	switch(selection)
	{
		case 1 :  /*1200*/
		{
			init_baud_rate = 96;
			open_host(96);
			goto baud_again;
		}

		case 2 :  /*2400*/
		{
			init_baud_rate = 48;
			open_host(48);
			goto baud_again;
		}

		case 3 :  /*4800*/
		{
			init_baud_rate = 24;
			open_host(24);
			goto baud_again;
		}

		case 4 :  /*9600*/
		{
			init_baud_rate = 12;
			open_host(12);
			goto baud_again;
		}

		case 5 :  /*19200*/
		{
			init_baud_rate = 6;
			open_host(6);
			goto baud_again;
		}

		case REJECT : 
		case ENTRY_COMPLETE : 
			return('c');

		case TERMINATE : 
			return('e');


		default:
			goto baud_again;

	}
}




char far *home_menu_text[] =
{
	"Configuration",
	"Communications",
	"Crosshair Control",
	"Diagnostics",
	"Save Display",
	"Switches",
	"Hardcopy",
	"Update Threshold",
	"\0",
	"EXIT TO DOS"
};




void far menu()
{
int selection,i,subroutine_num,sub_end,sub_ptr,baud_rate,auto_manual,
	list_print,redis_time;
unsigned long id,displacement,temp,num_blocks;
int seg_start,ch;
char name_plot_file[100],name_save_display[100],save_temps;


	read_curser_pos();
	turn_off_curser();
	save_win();

	selection = 0;

	while(selection != 0x44)
	{
display_main_menu:

		selection = choose_menu("System Control Functions",
			home_menu_text,10,0);


		switch(selection)
		{
			case 1 : /*status information*/
			{
				if(emulation_status() == 'e')
					goto exit_main_menu;

				break;
			}


			case 2 : /*Enter baud rate*/
			{
				if(enter_baud_rate() == 'e')
					goto exit_main_menu;


				break;
			}


			case 3 : /*cursor speed control*/
			{
				if(cursor_speed_set() == 'e')
					goto exit_main_menu;

				break;
			}

			case 4 : /*error control*/
			{
				if(error_log_status() == 'e')
					goto exit_main_menu;

				break;
			}



			case 5 : /*save part display*/
			{
		/*not implemented yet (save_part_display() is totally empty) */

				break;
			}


			case 6 : /*our switches*/
			{
				if(our_switches() == 'e')
					goto exit_main_menu;

				break;
			}



			case 7 :  /*HPGL Output*/
			{
				make_win(10,3,60,17);
				hpgl_menu();

				break;
			}



			case 8 :  /*rewrite threshold*/
			{
				read_menu_integer("Update Threshold",
						&rw_threshold,0,1);

				break;
			}



			case 10 : /*exit this program*/
			{
				make_win(45,15,64,19);
				clear_win(REVERSE_VIDEO);
				draw_box();

				goto_row_col(win_y_low + 1,win_x_low + 2);
				printf("EXIT TO DOS");
				goto_row_col(win_y_low + 2,win_x_low + 4);
				printf("Are You ");
				goto_row_col(win_y_low + 3,win_x_low + 4);
				printf("Sure? [y/N]:");

				make_win(10,3,60,17);


				ch = getch();

				if((ch != 'Y') && (ch != 'y'))
				{
					selection = 0;
					break;
				}


				close_board();

				/*remove whole bottom line*/
				normal_string("                                                                            ",24,1);
				clear_display_page();
				turn_on_curser();
				close_host();  /*closes host's com port*/
				restore_int9();

				exit();
			}



			case REJECT :
			case ENTRY_COMPLETE :
			case TERMINATE :
			case 0x47 : /*return to caller*/
			{
		exit_main_menu:
				make_win(10,3,60,17);
				restore_win();
				turn_on_curser();
				goto_row_col(row_curser,col_curser);
				return;
			}

		}   /*end switch*/

	}   /*end while*/

}
