#include "dos.h"
#include "c:\emulate\common\display.h"
#include "generics.h"
#include "stdio.h"
#include "expanded.h"
#include "string.h"
#include "c:\emulate\clipper\pw_lib.h"


/*February 3, 1989 replaced system("cls")'s with clear_display_page()'s
fixed clear_tek to clear all the bit planes 
removed "garbage collection choice from main home menu*/




#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3

#define RES_1024_768 3
#define RES_1280_1024 4

#define HARDWARE 0
#define SOFTWARE 1

#define PIXEL_TRUE 1
#define PIXEL_FALSE 0

#undef TRUE
#undef FALSE

#define TRUE  't'
#define FALSE  'f'
#define Y_MAX 3164
#define Y_MIN 0
#define X_MAX 4095
#define X_MIN 0
#define MAX_Y_OVER_4 791

#define ALPHANUMERIC 0
#define GRAPHIC 1
#define INTERACTIVE 2
#define SCROLL 3
#define PAGE 4
#define POINT_PLOT 5
#define INCREMENTAL_PLOT 6
#define OVERLAY 7
#define OFF_LINE 8
#define DL_REAL_END 0x1000


#define VT100_TERMINAL  0

char far which_terminal = VT100_TERMINAL;

/*display list variables*/
OPCODE far dummy_display[2000];
OPCODE far *current_pointer_to_dl;
char far added_dlint = FALSE;

char far which_high_res = RES_1024_768;
unsigned char far megatek_data[99];



/* choose.c menu functions */
extern void far control_window(void);
extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);


extern char far ems;
extern char far licensee[];
extern char far serial[];




/*error logging variables*/
FILE far *error_log_ptr;
char far errors_happened = 'f';
char far path_error_log[100];


/*BIOS keyboard functions & variables*/
extern int far keyboard_status();
extern void far read_keyboard();
char far type_char_read;
char far value_char_read;



extern void far hpgl_menu();
extern void far flush_text_buffer();


/*keyboard revectering routines*/
extern void far set_int9();
extern void far restore_int9();


extern void far clear_display_page();
extern void far my_pcbk();


char far diag_string[200];
extern void far error_logger(char *);


/*display file variables & functions*/
char far black = FALSE;
struct a_word far an_element;
char far write_display_error = FALSE;
char far quit_redisplay = FALSE;
char far quit_delete = FALSE;
extern void far insert_word();
extern void far redisplay();




/*my BIOS call routines used here*/
extern void far make_win();
extern void far clear_win();
extern void far save_win();
extern void far restore_win();
extern void far goto_row_col();
extern void far read_curser_pos();
extern void far draw_box();
extern void far turn_on_curser();
extern void far turn_off_curser();


extern void far save_part_display();



extern int far read_host_buffer();
extern void far horizontal_line();
extern void far vertical_line();
extern void far clear_crosshair();
extern void far open_host();
extern void far icu();
extern void far ugout();
extern void far set_joystick();
extern void far init_terminal();
extern void far display_charactor();
extern int far current_color;
extern int far syscolor_index;
extern char far erase_mode_flag;
union REGS far my_input_regs,far my_output_regs;
extern int far mouse_x;
extern int far mouse_y;
extern int far pan_x;
extern int far pan_y;
extern float far new_zoom;
extern char far all_used_up;
extern int far window_x_center;
extern int far window_y_center;

/*window's boundries at any time*/
extern int far max_x_window;
extern int far max_y_window;
extern int far min_x_window;
extern int far min_y_window;



struct seg_header
{
char is_transform,dash_enable,blink_enable,visibility,Xform_enable;
float rotate3D[4][4];
int x_origin,y_origin,z_origin,clip_x_upper,clip_x_lower,
	clip_y_upper,clip_y_lower;
char  intensity_color,blink_rate,detectability;
unsigned int dash_pattern;
float save_rotate3D[4][4];
int save_x_lower_clip,save_y_lower_clip,save_x_upper_clip,
	save_y_upper_clip;
};



struct seg_header far headers_segs[3];



int far which_cseg = 0;
int far my_x,far my_y;
unsigned char far low_x = 0,far low_y = 0,far high_x = 0,far high_y = 0;
int far which_mode = ALPHANUMERIC, far char_size = 4;
int far text_col,far text_row;
int far pointer = 0;
char far tek_low_x = 0,far extra = 0,far point_plot_flag = FALSE;
char far new_entity_flag = FALSE,far tek_low_y = 0;
char far low_x_flag = FALSE,far low_y_flag = FALSE,far high_x_flag = FALSE;
char far high_y_flag = FALSE,far tek_high_x = 0,far tek_high_y = 0;
char far escape_flag = FALSE,far display_flag = FALSE,far bypass = FALSE;
int far controller_type;
char far crosshair_flag = FALSE,graphics_mode_flag = FALSE;
char far tek_mode_flag = FALSE,far cross_substitute;
int far crosshair_column = 0,far crosshair_row = 0,line_style = 0;
int far init_baud_rate = 12;
char far drive_letter = 'h';
int far curser_ratio = 1;
int far mouse_phase = 0,far button_byte = 0,far my_pen_press = 0;
char far depress_flag = FALSE;
FILE far *display_file,*out_ptr = NULL,*display_save = NULL;
FILE far *delete_file,*array_files[2][4];
char far display_name[66];
int far temp_row_hair,far temp_col_hair;
int far which_page = 0;
int far io_masks[2] = {0x0f,0xf0};
float far window_speed_factor;


/*bios variables*/
#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1

int far win_y_low,far win_x_low,far win_x_up,far win_y_up;
char far char_save_page[25][80],far attr_save_page[25][80];
int far row_curser,far col_curser;



/*menu variables*/
char far rotate_mode = 'a';
char far board_string[80];
char far version_str[] = "1.30    1/29/90";
int far list_size = 0;
char far print_list = FALSE;
int far time_redisplay = FALSE;
int far num_pages = 2;
char far do_garbage = TRUE;
char far quit_redis;


extern int far unity_2d_matrix[];





void far megatek_update()
{

}

void far matrix_update()
{

}





void far write_a_file(value)
int value;
{

}


int far read_a_file()
{

}




void far check_com_status(status_value)
int status_value;
{
int error_count;
int save_row,save_col;

	if((status_value & 30) != 0)
	{
		putchar(7);

		error_count = 0;
		save_row = row_curser;
		save_col = col_curser;

		read_curser_pos();


		if((status_value & 2) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Overrun Error");
		}


		if((status_value & 4) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Parity Error");
		}


		if((status_value & 8) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Framing Error");
		}


		if((status_value & 16) != 0)
		{
			error_count++;
			goto_row_col(24,65);
			printf("Break Detect");
		}


		if(error_count > 1)
		{
			goto_row_col(24,60);
			printf("Com Errors = %x    ",status_value & 30);
		}


		row_curser = save_row;
		col_curser = save_col;
		goto_row_col(row_curser,col_curser);
	}

}









void far my_ugout(i)
int i;
{
	ugout(i);
}



void far read_more(num_chars)
int num_chars;
{
int i,j,ttt;
int ch;


	for(i = 0 ; i < num_chars ; i++)
	{
		my_pckb();

		ch = read_host_buffer();

cntrl_p:	if((ch & 0x7f) == 0x10)
		{
			ch = read_host_buffer();

                        while(ch == -1)
                        {
                        /*        for(j = 0 ; j < 30000 ; j++)
                                        ;
			*/

				my_pckb();
                                ch = read_host_buffer();
                        }


			if((ch & 0x7f) == 0x0d)
			{
				#ifdef HAVE_DIAGS
					my_diag("SWITCH TO MESSAGE MONITOR\n");
				#endif

				message_monitor();
				ch = read_host_buffer();
			}
		}

		while(ch == -1)
		{
		/*	for(j = 0 ; j < 30000 ; j++)
				;
		*/

			my_pckb();
			ch = read_host_buffer();
		}

		

		if((ch & 0x7f) == 0x10)
			goto cntrl_p;

		megatek_data[i] = (char)(ch & 0x7f);

	}


}  /* end read_more */






float far window_speed()
{
	return(1.0);
}





void far which_icu()
{
	ugout(0x1e);
	ugout('o');
	ugout('!');
	ugout('&');
	ugout('%');
}




char far *cursor_text[8] =
{
	"Cursor Speed Setting:                          ",
	"Press Mouse/Joystick Buttons                   ",
	"\0                                             ",
	"  1  Increase Cursor Speed                     ",
	"  2  Decrease Cursor Speed                     ",
	"\0                                             ",
	"     Cursor Speed = 292992                     ",
	"\0                                             "
};



char far curser_speed()
{
int dx,dy;

	crosshair_row = Y_MAX - window_y_center;
	crosshair_column = window_x_center;
	my_color(7);
	horizontal_line();
	vertical_line();



	if(curser_ratio > 0)
		sprintf(cursor_text[6],
			"      Cursor Speed = %d             ",curser_ratio);
	else if(curser_ratio < 0)
		sprintf(cursor_text[6],
			"      Cursor Speed = %f        ",-1.0/curser_ratio);
	else
		sprintf(cursor_text[6],"      Cursor Speed = 1             ");


	information_menu("Crosshair Control:",cursor_text,7,FALSE,FALSE);


	my_pen_press = 0;

	while((my_pen_press & 2) == 0)
	{
		if(keyboard_status() != -1)
		{
			read_keyboard();

			if(type_char_read == 0)
			{
				if((value_char_read == 75) || 
					(value_char_read == 77))
				{
					horizontal_line();
					vertical_line();
					my_color(current_color);

					return('c');
				}
				else if(value_char_read == 80)
				{
					horizontal_line();
					vertical_line();
					my_color(current_color);

					return('e');
				}
			}
		}



		if(curser_ratio > 0)
			sprintf(cursor_text[6],
				"      Cursor Speed = %d             ",
					curser_ratio);
		else if(curser_ratio < 0)
			sprintf(cursor_text[6],
				"      Cursor Speed = %f          ",
					-1.0/curser_ratio);
		else
			sprintf(cursor_text[6],
				"      Cursor Speed = 1               ");


		information_menu("Crosshair Control:",cursor_text,7,FALSE,
			TRUE);


		my_input_regs.x.ax = 11;  /*raw mouse movements*/
		int86(0x33,&my_input_regs,&my_output_regs);

		dx = my_output_regs.x.cx;
		dy = my_output_regs.x.dx;



		window_speed_factor = window_speed();


		if(curser_ratio > 0)
		{
			dx = (int)(dx*curser_ratio*window_speed_factor);
			dy = (int)(dy*curser_ratio*window_speed_factor);
		}
		else if(curser_ratio < 0)
		{
			dx = (int)(dx*window_speed_factor/(-curser_ratio));
			dy = (int)(dy*window_speed_factor/(-curser_ratio));
		}


		if((dx != 0) || (dy != 0))
		{
			horizontal_line();
			vertical_line();
		}


		crosshair_column += dx;
		crosshair_row += dy;

/*make crosshair stay inside of current window*/
		if(crosshair_column > max_x_window)
			crosshair_column = max_x_window;
		else if(crosshair_column < min_x_window)
			crosshair_column = min_x_window;
	
		if(crosshair_row > max_y_window)
			crosshair_row = max_y_window;
		else if(crosshair_row < min_y_window)
			crosshair_row = min_y_window;

/*make crosshair stay inside of limits of all drawing*/
		if(crosshair_column > X_MAX)
			crosshair_column = X_MAX;
		else if(crosshair_column < X_MIN)
			crosshair_column = X_MIN;

		if(crosshair_row > Y_MAX)
			crosshair_row = Y_MAX;
		else if(crosshair_row < Y_MIN)
			crosshair_row = Y_MIN;
	

		if((dx != 0) || (dy != 0))
		{
			horizontal_line();
			vertical_line();
		}



		my_input_regs.x.ax = 3;   /*regular read mouse*/
		int86(0x33,&my_input_regs,&my_output_regs);

		mouse_phase = my_output_regs.x.bx;

		if(mouse_phase == 0)
		{
			if(depress_flag == FALSE)
			{
				my_pen_press = 0;
				button_byte = 0;
			}
			else
			{
				depress_flag = FALSE;
				my_pen_press = button_byte;
				button_byte = 0;
			}
		}
		else
		{
			depress_flag = TRUE;
			button_byte = mouse_phase;
		}



		if((my_pen_press & 1) != 0)
		{
			curser_ratio++;
			if(curser_ratio == 0)
				curser_ratio = 1;
		}
		else if((my_pen_press & 4) != 0)
		{
			curser_ratio--;
			if(curser_ratio == 0)
				curser_ratio = -1;
		}
		
	}   /*end while*/



	horizontal_line();
	vertical_line();
	my_color(current_color);

	return('c');
}




char far *error_text[3] =
{

	"List Errors",
	"Print Errors",
	"Clear Errors"
};





char far error_log_status()
{
int selection;


error_log_again:

	selection = choose_menu("Diagnostics",error_text,3,0);

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

			/*remove blinking "ERRORS" message*/
			normal_string("        ",24,2);
			errors_happened = 'f';

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

			/*remove blinking "ERRORS" message*/
			normal_string("        ",24,2);
			errors_happened = 'f';

			goto error_log_again;

			break;
		}

		case 3 : /*clear it*/
		{
			sprintf(diag_string,"%serrorlog.dat",path_error_log);
			error_log_ptr = fopen(diag_string,"w");
			fprintf(error_log_ptr,"File Cleared:\n\n");
			fclose(error_log_ptr);

			/*remove blinking "ERRORS" message*/
			normal_string("        ",24,2);
			errors_happened = 'f';

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




char far *switches_text[4] =
{
	"                                                    ",
	"                                                    ",
	"                                                    ",
	"                                                    "
};




char far our_switches()
{
int selection;


	while(9)
	{
switch_again:

		sprintf(switches_text[0],"Listing Size");


		if(print_list == TRUE)
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


		if(time_redisplay == TRUE)
			sprintf(switches_text[3],
			       "Update Timer        :  Enabled");
		else
			sprintf(switches_text[3],
			       "Update Timer        :  Disabled");


		selection = choose_menu("Switches",switches_text,4,0);


		switch(selection)
		{
			case 2 : /*send/don't send listings to the printer*/
			{
				if(print_list == TRUE)
				{
					if(out_ptr != NULL)
					{
						fclose(out_ptr);
						out_ptr = NULL;
					}

					print_list = FALSE;
				}
				else
				{
					if(out_ptr == NULL)
						out_ptr = fopen("lpt1","w");

					print_list = TRUE;
				}

				break;
			}


			case 3 : /*screen dynamics*/
			{
				if(rotate_mode == 'a')
					rotate_mode = 'm';
				else
					rotate_mode = 'a';

				break;
			}


			case 4 : /*time/don't time redisplays*/
			{
				if(time_redisplay == TRUE)
					time_redisplay = FALSE;
				else
					time_redisplay = TRUE;

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





char far *status_text[10] =
{
	"     The Board                                             ",
	"     Emulation Mode                                        ",
	"     Version                                               ",
	"\0                                                         ",
	"     licensee                                              ",
	"     Cereal ID : Rice Crispies                             ",
	"\0                                                         ",
	"     Number Ems pages                                      ",
	"     Screen Resolution                                     ",
	"     Ems mode                                              "
};





char far emulation_status()
{
int selection;




	sprintf(status_text[0],"    %s Board",board_string);
	sprintf(status_text[1],"    Emulation Mode: D90");
	sprintf(status_text[2],"    Version# %s",version_str);

	sprintf(status_text[4],"    Licensee : %s",licensee);
	sprintf(status_text[5],"    Serial ID : %s",serial);

	sprintf(status_text[7],"    Number Of Expanded Memory Pages Used = %d",
			current_logical_page + 1);


	if(which_high_res == RES_1024_768)
		sprintf(status_text[8],"    Screen Resolution  =  1024 x 768");
	else
		sprintf(status_text[8],"    Screen Resolution  =  1280 x 1024");


	if(ems == SOFTWARE)
		sprintf(status_text[9],"    EMS Mode  =  Software");
	else
		sprintf(status_text[9],"    EMS Mode  =  Hardware");



	selection = information_menu("Configuration Info",status_text,10,
		TRUE,FALSE);


	switch(selection)
	{
		case ENTRY_COMPLETE :
		case REJECT :
			return('c');


		case TERMINATE :
			return('e');
	}

}




char far *baud_rate_text[5] =
{
	"1200",
	"2400",
	"4800",
	"9600",
	"19200"
};




char far set_baud_rate()
{
int selection,current_index;


baud_again:	


	switch(init_baud_rate)
	{
		case 96 :
		{
			current_index = 1;
			break;
		}

		case 48 :
		{
			current_index = 2;
			break;
		}

		case 24 :
		{
			current_index = 3;
			break;
		}

		case 12 :
		{
			current_index = 4;
			break;
		}

		case 6 :
		{
			current_index = 5;
			break;
		}
	}


	selection = choose_menu("Select Baud Rate",baud_rate_text,5,
		current_index);


	switch(selection)
	{
		case 1 :   /*1200*/
		{
			init_baud_rate = 96;
			open_host(96);  
			goto baud_again; 
		}

		case 2 :   /*2400*/
		{
			init_baud_rate = 48;
			open_host(48);
			goto baud_again;
		}

		case 3 :   /*4800*/
		{
			init_baud_rate = 24;
			open_host(24);
			goto baud_again;
		}

		case 4 :   /*9600*/
		{
			init_baud_rate = 12;
			open_host(12);
			goto baud_again;
		}

		case 5 :   /*19200*/
		{
			init_baud_rate = 6;
			open_host(6);
			goto baud_again;
		}


		case TERMINATE :
			return('e');

		case REJECT :
		case ENTRY_COMPLETE :
			return('c');
	}

}





char far *main_menu_text[10] =
{
	"Configuration",
	"Communications",
	"Crosshair Control",
	"Diagnostics",
	"Save Display",
	"Switches",
	"Hardcopy",
	"\0",
	"\0",
	"EXIT TO DOS"
};




void menu()
{
int selection,i,subroutine_num,sub_end,sub_ptr,baud_rate,auto_manual,
	list_print,redis_time;
unsigned long id,displacement,temp,num_blocks;
int seg_start,ch,row,col;
char name_plot_file[100],name_save_display[100],del_temps;
int save_crosshair_column,save_crosshair_row,dx,dy;


/*save & remove crosshair if displayed*/
	if(crosshair_flag == TRUE)
	{
		save_crosshair_column = crosshair_column;
		save_crosshair_row = crosshair_row;
		horizontal_line();
		vertical_line();
	}

	read_curser_pos();
	turn_off_curser();
	save_win();


	while(9)
	{
display_main_menu:

		selection = choose_menu("System Control Functions",
			main_menu_text,10,0);


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
				if(set_baud_rate() == 'e')
				goto exit_main_menu;

				break;
			}



			case 3 : /*curser speed control*/
			{
				if(curser_speed() == 'e')
					goto exit_main_menu;


				break;
	
			}



			case 4 : /*diagnostics*/
			{
				if(error_log_status() == 'e')
					goto exit_main_menu;

				break;
			}


			case 5 : /*save the display*/
			{


				break;
			}



			case 6 : /*switches*/
			{
				if(our_switches() == 'e')
					goto exit_main_menu;

				break;
			}


			case 7 : /*hardcopy*/
			{
				hpgl_menu();
				break;
			}



			case 10 : /*exit this program*/
			{
				make_win(45,15,64,19);
				clear_win(REVERSE_VIDEO);
				draw_box();

				goto_row_col(win_y_low + 1,win_x_low + 2);
				printf("Exit To DOS: ");
				goto_row_col(win_y_low + 2,win_x_low + 3);
				printf("Are You");
				goto_row_col(win_y_low + 3,win_x_low + 3);
				printf("Sure? (y/n): ");

				make_win(10,3,60,17);

				ch = getch();

				if((ch != 'Y') && (ch != 'y'))
				{
					selection = 0;
					break;
				}


				close_board();
				turn_on_curser();
			/*release expanded memory pages used*/
				dealloc_pages_for_handle(handle); 
				clear_display_page();

				restore_int9();

				exit();
			}



			case TERMINATE :
			case ENTRY_COMPLETE :
			case REJECT :
			case 0x47 : /*return to caller*/
			{
		exit_main_menu:
				if(crosshair_flag == TRUE)
				{  /*put crosshair back up*/
					crosshair_row = save_crosshair_row;
					crosshair_column = save_crosshair_column;
					my_color(current_color);
					horizontal_line();
					vertical_line();
				}


				make_win(10,3,60,17);
				restore_win();
				turn_on_curser();
				goto_row_col(row_curser,col_curser);
				return;
			}



		}   /*end switch(selection)*/


	}	/*end while()*/

}











void far mouse_reset()
{
	my_input_regs.x.ax = 0;
	int86(0x33,&my_input_regs,&my_output_regs);

	if(my_output_regs.x.ax == 0)
	{
		putchar(7);
		sprintf(diag_string,"Joystick/Mouse Not Installed\n");
		error_logger(diag_string);
	}

	if(my_output_regs.x.bx == 2)
		curser_ratio = 1;
	else if(my_output_regs.x.bx == 3)
		curser_ratio = 4;
		
}







void far joystick()
{
int dx,dy,ch;

	my_input_regs.x.ax = 11;     /*raw mouse movements*/
	int86(0x33,&my_input_regs,&my_output_regs);

	dx = my_output_regs.x.cx;
	dy = my_output_regs.x.dx;


	window_speed_factor = window_speed();


	if(curser_ratio > 0)
	{
		dx = (int)(dx*curser_ratio*window_speed_factor);
		dy = (int)(dy*curser_ratio*window_speed_factor);
	}
	else if(curser_ratio < 0)
	{
		dx = (int)(dx*window_speed_factor/(-curser_ratio));
		dy = (int)(dy*window_speed_factor/(-curser_ratio));
	}


	if((dx != 0) || (dy != 0))
	{
		horizontal_line();
		vertical_line();
	}


	crosshair_column += dx;
	crosshair_row += dy;

/*make crosshair stay inside of current window*/
	if(crosshair_column > max_x_window)
		crosshair_column = max_x_window;
	else if(crosshair_column < min_x_window)
		crosshair_column = min_x_window;

	if(crosshair_row > max_y_window)
		crosshair_row = max_y_window;
	else if(crosshair_row < min_y_window)
		crosshair_row = min_y_window;

/*make crosshair stay inside of limits of all drawing*/
	if(crosshair_column > X_MAX)
		crosshair_column = X_MAX;
	else if(crosshair_column < X_MIN)
		crosshair_column = X_MIN;

	if(crosshair_row > Y_MAX)
		crosshair_row = Y_MAX;
	else if(crosshair_row < Y_MIN)
		crosshair_row = Y_MIN;



	my_input_regs.x.ax = 3;
	int86(0x33,&my_input_regs,&my_output_regs);

	mouse_phase = my_output_regs.x.bx;

	if(mouse_phase == 0)
	{
		if(depress_flag == FALSE)
		{
			my_pen_press = 0;
			button_byte = 0;
		}
		else
		{
			depress_flag = FALSE;
			my_pen_press = button_byte;
			button_byte = 0;
		}
	}
	else
	{
		depress_flag = TRUE;
		button_byte = mouse_phase;
	}


	if((my_pen_press & 1) != 0)
		ugout(32);



	if((dx != 0) || (dy != 0))
	{
		vertical_line();
		horizontal_line();
	}
}








void far move_pen_up(x,y)
int x,y;
{
	my_x = x;
	my_y = y;
}




void far open_display_file()
{

}


void far close_display_file()
{

}


void far clear_tek()	/*clears d90 emulation*/
{
int row,col;

	pw_dl_init(NULL,0,NULL,0);  /*for good luck*/

/*clear all bit planes in pixel board*/
	wrmask(0xff);
	rdmask(0xff);
	flood(0);
/*go back to "page" 0*/
	which_page = 0;
	wrmask(0x0f);
	rdmask(0x0f);

	graphics_mode_flag = FALSE;
	bypass = FALSE;
	point_plot_flag = FALSE;
	
	if(crosshair_flag == TRUE)
		clear_crosshair();

	my_x = 0;
	my_y = 9;

/*set "insert pointer" back to start of logical page 0*/
	current_pointer_to_dl = (short far *)physical_pages_pointer;
	current_logical_page = 0;
	current_physical_page = 0;
	all_used_up = FALSE;  /*expanded memory is not used up*/
	map_handle_page(current_logical_page,current_physical_page,handle);

/*insert flood(0) in first page of the display list*/
	pw_dl_init((short far *)physical_pages_pointer,0,NULL,0);
	flood(0);
	rdlbptr(&current_pointer_to_dl);
	pw_dl_init(NULL,0,NULL,0);

	added_dlint = FALSE;  /*haven't redisplayed at all yet, so there's
no dlint() or dlend() to mark the end of the last page in the part*/
}




void far display_indicator(x,y)
int x,y;
{
	#ifdef HAVE_DISP_FILE
		an_element.op_code = 7;
		an_element.arg1 = x;
		an_element.arg2 = y;
		if(erase_mode_flag == FALSE)
			an_element.color = syscolor_index;
		else
			an_element.color = current_color;
		an_element.font = line_style;


		insert_word();
	#endif


	if(erase_mode_flag == FALSE)
		my_color(syscolor_index);


	my_circle(x,y);


	if(erase_mode_flag == FALSE)
		my_color(current_color);
}






void far valid(command)
int command;
{
int x,y;

	pointer++;

/*
	extra = 0;
*/

	if((command & 64) != 0)  /*low part of x or y*/
	{
		if((command & 32) == 0)   /*low part of x*/
		{
			pointer = 4;
			tek_low_x = command & 0xff;

			x = (high_x << 7) | ((command & 0x1f) << 2) | 
				(extra & 0x03);
			y = (high_y << 7) | (low_y << 2) | ((extra >> 2) & 0x03);

			low_y_flag = FALSE;


/*
			extra = 0;
*/

			if(point_plot_flag == FALSE)  /*vector*/
			{
				if(new_entity_flag == FALSE)   /*draw vector*/
				{
					draw_line(my_x,my_y,x,y);

					#ifdef HAVE_DISP_FILE
						an_element.op_code = 1;
						an_element.arg1 = my_x;
						an_element.arg2 = my_y;
						an_element.arg3 = x;
						an_element.arg4 = y;
						an_element.color = current_color;
						an_element.font = line_style;

						insert_word();

					#endif

					my_x = x;
					my_y = y;
					pointer = 0;
				}
				else    /*move*/
				{
					move_pen_up(x,y);

					#ifdef HAVE_DISP_FILE
						an_element.op_code = 6;
						an_element.arg1 = x;
						an_element.arg2 = y;

						insert_word();
					#endif

					my_x = x;
					my_y = y;
					new_entity_flag = FALSE;
					pointer = 0;
				}
			}
			else    /*plot a point*/
			{
				move_pen_up(x,y);
				dot_plot(x,y);

				#ifdef HAVE_DISP_FILE
					an_element.op_code = 5;
					an_element.arg1 = x;
					an_element.arg2 = y;
					an_element.color = current_color;
					an_element.font = line_style;

					insert_word();
			
				#endif

				my_x = x;
				my_y = y;
				pointer = 0;
			}

		}
		else  /*low part of y*/
		{
			if(command > 127)
				return;

			pointer = 2;

			if(low_y_flag == FALSE)  /*low y*/
			{
				tek_low_y = command & 0xff;
				low_y = command & 0x1f;
				low_y_flag = TRUE;
			}
			else  /*extra byte*/
			{
				extra = low_y;
				tek_low_y = command & 0xff;
				low_y = command & 0x1f;
				low_y_flag = TRUE;
			}
		}
	}
	else    /*high x or y*/
	{
		if(pointer == 1)  /*high_y*/
		{
			tek_high_y = command & 0xff;
			high_y = command & 0x1f;
		}
		else   /*high_x*/
		{
			tek_high_x = command & 0xff;
			high_x = command & 0x1f;
		}
	}

}




void far escape_functions(command)
int command;
{
int temp_y;
long temp1_y,remainder;
int i;



	escape_flag = FALSE;

	switch(command & 0xff)
	{
		case 96 :  /*solid*/
		{
			line_style = 0;
			my_dash_enable(0);


			#ifdef HAVE_DISP_FILE
				an_element.op_code = 2;
				an_element.arg1 = 0;

				insert_word();
			#endif

			break;
		}

		case 'c' : /*dashed*/
		{
			line_style = 1;
			my_dash_enable(1);

			#ifdef HAVE_DISP_FILE
				an_element.op_code = 2;
				an_element.arg1 = 1;

				insert_word();
			#endif

			break;
		}

		case 'a' : /*phantom*/
		{
			line_style = 4;
			my_dash_enable(4);

			#ifdef HAVE_DISP_FILE
				an_element.op_code = 2;
				an_element.arg1 = 4;

				insert_word();
			#endif

			break;
		}

		case 'b' : /*centerline*/
		{
			line_style = 2;
			my_dash_enable(2);

			#ifdef HAVE_DISP_FILE
				an_element.op_code = 2;
				an_element.arg1 = 2;

				insert_word();
			#endif

			break;
		}

		case 12 :
		{
			if(controller_type != 2)
			{
				clear_tek();
			}
			else if(display_flag == TRUE)
			{
				clear_tek();
			}
			else
			{
				close_display_file();
				open_display_file();
				clear_tek();
			}

			break;
		}

		case 26 : 
		{
			bypass = TRUE;
			crosshair_flag = TRUE;
			set_joystick(1);
			vertical_line();
			horizontal_line();
			point_plot_flag = FALSE;
			graphics_mode_flag = FALSE;
			if(tek_mode_flag == TRUE)
			{
				clear_crosshair();
				bypass = FALSE;
				crosshair_flag = FALSE;
				horizontal_line();
				vertical_line();

				goto crosshair_encode;
			}
			else
				break;
		}


		default :
		{
/*			if(command == cross_substitute)
			{
				bypass = TRUE;
				clear_crosshair();

				crosshair_flag = TRUE;
				point_plot_flag = FALSE;
				graphics_mode_flag = FALSE;
			}
			else
			{
*/
				if(command == 5)
				{
					if(crosshair_flag == FALSE)
					{
						ugout(29);  /*alpha encode*/
						ugout(13);
					}
					else
					{
			crosshair_encode :

						temp_col_hair = 
							crosshair_column/4;
							
						temp_row_hair = MAX_Y_OVER_4 - 
							crosshair_row/4;
							

						if(tek_mode_flag == FALSE)
							ugout(29);
						ugout(0xbb);

						ugout(((temp_col_hair >> 5)
							& 0x1f) | 32);
						ugout((temp_col_hair & 0x1f)
							| 32);


						temp_y = temp_row_hair;
						ugout(((temp_y >> 5) & 0x1f)
							| 32);
						ugout((temp_y & 0x1f) | 32);
						
						ugout(13);
					}
				}
				else  /*ef3*/
				{
					if(command  == 23)
					{
						bypass = FALSE;
						if(crosshair_flag == TRUE)
						{
							clear_crosshair();

						}
					}
				}
		/*	}
		*/
		}
	}
}





/*same idea as megatek_device*/

void far d90_device(command)
int command;
{
	command &= 0xff;

	if(escape_flag == TRUE)
	{
		escape_functions(command);
	}
	else if(command == 27)
		escape_flag = TRUE;
	else if(command < 32)
		switch(command)
		{
			case 13 :
			{
				bypass = FALSE;
				point_plot_flag = FALSE;

				if(crosshair_flag == TRUE)
					clear_crosshair();

				graphics_mode_flag = FALSE;
				display_charactor(command);

				break;
			}

			case 31 :
			{
				bypass = FALSE;
				point_plot_flag = FALSE;

				if(crosshair_flag == TRUE)
					clear_crosshair();
		
				graphics_mode_flag = FALSE;

				break;
			}

			case 7 :
			{
				bypass = FALSE;

				if(crosshair_flag == TRUE)
					clear_crosshair();

				display_charactor(command);

				break;
			}

			default :
			{
				if(bypass == FALSE)
					switch(command)
					{
						case 28 :
						{
							graphics_mode_flag = TRUE;
							point_plot_flag = TRUE;

							break;
						}
	
						case 30 :
						{
							graphics_mode_flag = FALSE;
							break;
						}

						case 29 :
						{
							flush_text_buffer();

							graphics_mode_flag = TRUE;
							new_entity_flag = TRUE;

							break;
						}

						case 10 :
						{
							display_charactor(command);
							break;
						}
					}
		
			}
		}

	else if(command < 128)
	{
		if(bypass == FALSE)
		{
			if((point_plot_flag == TRUE) || 
				(graphics_mode_flag == TRUE))
			{
				valid(command & 0x7f);
			}
			else
				display_charactor(command);
		}
	}	

}







main()
{
int i,row,col;

	set_int9();


	clear_display_page();

/*set up expanded memory*/
	current_logical_page = 0;
	current_physical_page = 0;
	all_used_up = FALSE;

	get_board_status();
	page_frame_segment_address = get_page_frame_address();
	num_unalloc_pages = get_unalloc_page_count();
	handle = allocate_pages(num_unalloc_pages);
	physical_pages_pointer = (int far *)((long)page_frame_segment_address 
		<< 16);
	map_handle_page(current_logical_page,current_physical_page,handle);



	test_board();
	i = init_artist10();


/*open error log file if wanted (path has been tested in init_artist10())*/
        if(strcmp(strupr(path_error_log),"NONE") == 0)
                error_log_ptr = NULL;
        else
        {
                sprintf(diag_string,"%serrorlog.dat",path_error_log);
                error_log_ptr = fopen(diag_string,"w");
                fprintf(error_log_ptr,"No Errors:\n\n");
                fclose(error_log_ptr);
        }




	pw_dl_init(&dummy_display[0],0,NULL,0);
	mdiden2();
	mdorg2(0,0);
	vwmatx2(&unity_2d_matrix[0]);
	vwrpt3(0,0,0);
	gtsize(35,30);


	if(which_high_res == RES_1024_768)
		viewport(0,1019,0,767);
	else
		viewport(0,1279,0,1023);


	window(0,4095,0,Y_MAX);
	prmfill(0);  /*makes circles hollow*/

	init_terminal(); /*sets up the 3 line styles & all the lut8's*/

	rdmask(0x0f);
	wrmask(0x0f);
	dlend();

	pw_dl_exec(PIXEL_TRUE);
	pw_dl_init(NULL,0,NULL,0);

	mouse_reset();
	make_win(10,3,60,17);

	my_x = 0;
	my_y = 0;
	text_col = 0;
	text_row = 0;


	current_pointer_to_dl = (short far *)physical_pages_pointer;
	pw_dl_init((short far *)physical_pages_pointer,0,NULL,0);
	flood(0);
	rdlbptr(&current_pointer_to_dl);
	pw_dl_init(NULL,0,NULL,0);

	open_host(init_baud_rate);
	open_host(init_baud_rate);
	icu();
}

