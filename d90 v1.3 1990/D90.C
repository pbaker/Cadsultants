#include "c:\emulate\common\display.h"
#include "dos.h"
#include "generics.h"
#include "stdio.h"
#include "stdlib.h"
#include "expanded.h"



#define TERMINATE  -1
#define ENTRY_COMPLETE  -2
#define REJECT  -3



#define TRUE  't'
#define FALSE  'f'

#define Y_MAX 767
#define Y_MIN 0
#define X_MAX 1023
#define X_MIN 0

#define ALPHANUMERIC 0
#define GRAPHIC 1
#define INTERACTIVE 2
#define SCROLL 3
#define PAGE 4
#define POINT_PLOT 5
#define INCREMENTAL_PLOT 6
#define OVERLAY 7
#define OFF_LINE 8


#define A_SIZE_SCALE 2.533


#define VT100_TERMINAL  0


extern void far my_pckb(void);


extern void far restore_int9();
extern void far set_int9();

extern void far clear_error_vars();


/* choose.c  functions */

extern void far control_window(void);
extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);



extern int far number_of_errors;
extern char far licensee[];
extern char far serial[];


extern char far save_part_write_error;
extern char far plot_file_write_error;


char far which_terminal = VT100_TERMINAL;

char far name_save_display[111];


/*display file variables & functions*/
char far black = FALSE;
struct a_word far an_element,far read_element;
char far write_display_error = FALSE;
char far quit_redisplay = FALSE;
extern void far insert_word(void);
extern void far get_word();
extern void far redisplay();
extern void far save_part_display();
extern int far keyboard_status();
extern void far clear_display_page();
extern void far close_host();


/*my BIOS call routines used here*/
extern void far make_win(int,int,int,int);
extern void far clear_win(int);
extern void far save_win(void);
extern void far restore_win(void);
extern void far goto_row_col(int,int);
extern void far read_curser_pos(void);
extern void far draw_box(void);
extern void far turn_on_curser(void);
extern void far turn_off_curser(void);



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
extern union REGS far my_input_regs,far my_output_regs;
extern int far mouse_x;
extern int far mouse_y;
extern int far pan_x;
extern int far pan_y;
extern float far new_zoom;
extern char far have_display_list;
extern char far ems_mode;

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
int far crosshair_column = 0 ,far crosshair_row = 0,line_style = 0;
int far init_baud_rate = 12;
char far drive_letter = 'e';
int far curser_ratio = 1;
int far mouse_phase = 0,far button_byte = 0,far my_pen_press = 0;
char far depress_flag = FALSE;

FILE far *out_ptr = NULL,far *display_save = NULL;

FILE far *hp_plotter;
FILE far *error_log_ptr;


char far diag_string[200];
char far path_error_log[80] = "d:\\";
char far errors_happened = 'f';

char far display_name[66];
int far temp_row_hair,far temp_col_hair;


/*bios variables*/
#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1

int far win_y_low,far win_x_low,far win_x_up,far win_y_up;
char far char_save_page[25][80],far attr_save_page[25][80];
int far row_curser,far col_curser;



/*menu variables*/
char far rotate_mode = 'a';
char far board_string[80];
char far version_str[80] = "1.30   1/29/90";
int far list_size = 0;
char far print_list = FALSE;
int far time_redisplay;
int far num_pages = 2;
char far do_garbage = FALSE;




#define X_DEFAULT_OFFSET 70
#define Y_DEFAULT_OFFSET 170


/*plot file variables*/
int far plot_x_offset = X_DEFAULT_OFFSET;
int far plot_y_offset = Y_DEFAULT_OFFSET;
float far plot_scale_factor = 1.0;
extern void far make_plot_file();
char far hp_plotter_name[80];
char far parameter_string[30];



unsigned char far megatek_data[99];



/* EMS variables */


int far handle;
int far num_unalloc_pages;
int far page_frame_segment_address;
int far num_pages_requested;
char far no_more_ems_pages = FALSE; 
short far *start_of_page_frame_0;

extern int far current_logical_page;
extern char far *current_insert_pointer;





/* fake stuff for compatability with \emulate\common's  obj's */
int far handle;
int far initial_joy_count;
int far rw_threshold;
long far max_wait_time;


void far release_version()
{

}




void far matrix_update()
{

}



int far read_a_file()
{

}


void far write_a_file(value)
int value;
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











void far which_icu()
{
	ugout(0x1e);
	ugout('o');
	ugout('!');
	ugout('&');
	ugout('%');
}







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


/* the very 1st character code */

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
		if(((ch >= '0') && (ch <= '9')) || (ch == '-'))
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

	row = win_y_low + 4;
	col = win_x_low + 7;

start_over:
	make_win(10,3,60,17);
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 2);
	printf("%s",header_string);


	if(return_string[0] != '\0')
	{
		goto_row_col(win_y_low + 2,win_x_low + 3);
		printf("Current Value = %s",return_string);
	}


	control_window();
	make_win(10,3,60,17);
	goto_row_col(win_y_low + 4,col);

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

	switch(selection)
	{
		case 1 : /*display it*/
		{
			make_win(10,3,60,17);

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
			make_win(10,3,60,17);

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

		case REJECT : 
		case ENTRY_COMPLETE : 
			return('c');

		case TERMINATE : 
			return('e');

		default :
			goto error_log_again;
	}

	return('c');
}





char far *config_text[10] = 
{
	"     Controller:  A board \0                                  ",
	"     PC WorkWindow Version:  1.xx   xx/xx/xx \0               ",
	"     Emulation Mode:  D90  \0                                ",
	"\0                                                            ",
	"     Licensee     \0                                          ",
	"     Serial     \0                                            ",
	"\0                                                            ",
	"\0                                                            ",
	"     Display Memory Size = xxxxx \0                           ",
	"\0                                                            "
};



char far emulation_status()
{
int selection,i;
int num_ems_pages;


	sprintf(config_text[0],"    Controller: %s",board_string);
	sprintf(config_text[1],"    PC WorkWindow Version: %s",version_str);
	sprintf(config_text[2],"    Emulation Mode: D90");

	sprintf(config_text[4],"    Licensee : %s",licensee);
	sprintf(config_text[5],"    Serial : %s",serial);

	sprintf(config_text[8],"    Display Memory Size = %d  pages",
		current_logical_page + 1);


	selection = information_menu("Configuration Info",config_text,10,
		't','f');


	switch(selection)
	{
		case REJECT : 
		case ENTRY_COMPLETE : 
			return('c');

		case TERMINATE :
			return('e');
	}


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

	x_mouse = 512;
	y_mouse = 384;
	dx = dy = 0;

	my_dash_enable(0);   /*disable dashing*/
	save_current_color = current_color;
	my_color(7);



	sprintf(cursor_text[4],"    (F1 To Center Cursor)    ");
	information_menu("Crosshair Control:",cursor_text,9,'f','f');


	my_pen_press = 0;

	draw_mode(3);
	my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
	my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);

	while((my_pen_press & 2) == 0)
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


					mouse_x = 512;
					mouse_y = 384;
				}
				else if((ch == 75) || (ch == 77))
				{    /*undraw cursor speed curser*/
					my_line_xy(x_mouse - 50,y_mouse,x_mouse
						 + 50,y_mouse);
					my_line_xy(x_mouse,y_mouse - 50,x_mouse
						,y_mouse + 50);
					draw_mode(0);
					my_color(save_current_color);
					my_dash_enable(4);
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
					my_dash_enable(4);
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
		dy = my_output_regs.x.dx;

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
				my_pen_press = 0;
				button_byte = 0;
			}
			else
			{
				depress_flag = 'f';
				my_pen_press = button_byte;
				button_byte = 0;
			}
		}
		else
		{
			depress_flag = 't';
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

    /*undraw cursor speed curser*/
	my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
	my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);
	draw_mode(0);
	my_color(save_current_color);
	my_dash_enable(4);
	return('c');
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







char far generate_plot_file()
{
int ch,i;

generate_again:

	hp_plotter_name[0] = '\0';

	read_menu_string("Enter Hardcopy File's Full Pathname:",
		hp_plotter_name);

	if(hp_plotter_name[0] == '\0')
		return('c');

	hp_plotter = fopen(hp_plotter_name,"w");

			
	if(hp_plotter == NULL)
	{
		error_message(7,invalid_file,2);
		return('c');
	}

	make_plot_file();

	plot_scale_factor /= A_SIZE_SCALE;

	fclose(hp_plotter);

	return('c');
}



char far *hp_params_text[3] =
{
	"X Offset  :                                       ",
	"Y Offset  :                                       ",
	"Scale     :                                       ",
};


int far my_hp_integer_temp;


char far hp_parameters()
{
int selection;


parameters_again:

	sprintf(hp_params_text[0]," X Offset  :  %d",
		plot_x_offset - X_DEFAULT_OFFSET);
	sprintf(hp_params_text[1]," Y Offset  :  %d",
		plot_y_offset - Y_DEFAULT_OFFSET);
	sprintf(hp_params_text[2]," Scale     :  %f",plot_scale_factor);


	selection = choose_menu("Hardcopy Parameters:",hp_params_text,3,0);

	switch(selection)
	{
		case 1 : /*x offset*/
		{
			my_hp_integer_temp = plot_x_offset - X_DEFAULT_OFFSET;
			read_menu_integer(" X Offset ",&my_hp_integer_temp,
				-30000,30000);

			plot_x_offset = my_hp_integer_temp + X_DEFAULT_OFFSET;

			goto parameters_again;
		}

		case 2 : /*y offset*/
		{
			my_hp_integer_temp = plot_y_offset - Y_DEFAULT_OFFSET;
			read_menu_integer(" Y Offset ",&my_hp_integer_temp,
				-30000,30000);

			plot_y_offset = my_hp_integer_temp + Y_DEFAULT_OFFSET;

			goto parameters_again;
		}


		case 3 : /*scale*/
		{
			sprintf(parameter_string," %f ",plot_scale_factor);
			read_menu_string(" Scaling Factor ",
				parameter_string);
			plot_scale_factor = atof(parameter_string);

			if(plot_scale_factor == 0.0)
				plot_scale_factor = 1.0;

			goto parameters_again;
		}


		case REJECT :
		case ENTRY_COMPLETE :
			return('c');

		case TERMINATE : 
			return('e');
	}
}



char far *no_hpgl_text[3] =
{
	"No Display List In Use",
	"    ",
	"Can't Generate Hardcopy Data"
};



char far *main_hpgl_text[2] =
{
	"Generate Output",
	"Parameters"
};





char far menu_plot_file()
{
int selection;


	
	if(have_display_list == FALSE)
	{
		selection = information_menu("Hardcopy",no_hpgl_text,3,
			TRUE,FALSE);

		if((selection == ENTRY_COMPLETE) || (selection == REJECT))
			return('c');

		if(selection == TERMINATE)
			return('e');
	}


main_plot_again:

	selection = choose_menu("Hardcopy",main_hpgl_text,2,0);	

	switch(selection)
	{
		case 1 : /*generate output*/
		{
			if(generate_plot_file() == 'e')
				return('e');

			goto main_plot_again;
		}

		case 2 : /*parameters*/
		{
			if(hp_parameters() == 'e')
				return('e');

			goto main_plot_again;
		}

		case REJECT :
		case ENTRY_COMPLETE :
			return('c');

		case TERMINATE :
			return('e');
	}
}






char far *switches_text[2] =
{
	"Printer Echo                                  ",
	"Update Timer                                  "
};




char far our_switches()
{
int selection;


	while(9)
	{
		if(print_list == TRUE)
			sprintf(switches_text[0],
				"  Printer Echo     :  Enabled");
		else
			sprintf(switches_text[0],
				"  Printer Echo     :  Disabled");


		if(time_redisplay == TRUE)
			sprintf(switches_text[1],
				"  Update Timer     :  Enabled");
		else
			sprintf(switches_text[1],
				"  Update Timer     :  Disabled");


		selection = choose_menu("Switches",switches_text,2,0);


		switch(selection)
		{
			case 1 : /*send/don't send listings to the printer*/
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

			case 2 : /*time/don't time redisplays*/
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




char far *save_no_dl_text[3] =
{
	"No Display List In Use",
	" \0",
	"Can't Generate Save File"
};





char far save_the_part()
{
int selection;


	if(have_display_list == FALSE)
	{
		selection = information_menu("Save Screen Image",
			save_no_dl_text,3,TRUE,FALSE);
	
		if((selection == REJECT) || (selection == ENTRY_COMPLETE))
			return('c');
		else
			return('e');
	}



	name_save_display[0] = '\0';
	read_menu_string("Enter Save File's Name",name_save_display);
	if(name_save_display[0] == '\0')
		return('c');

	display_save = fopen(name_save_display,"w+b");
			
	if(display_save == NULL)
	{
		error_message(8,invalid_file,2);
		return('c');
	}

	save_part_display();

	fclose(display_save);
	display_save = NULL;

}






char far *main_menu_text[] =
{
	"Configuration",
	"Communications",
	"Crosshair Control",
	"Diagnostics",
	"Save Screen Image",
	"Switches",
	"Hardcopy",
	"\0",
	"\0",
	"EXIT TO DOS"
};





void far menu()
{
int selection,i,subroutine_num,sub_end,sub_ptr,baud_rate,auto_manual,
	list_print,redis_time;
unsigned long id,displacement,temp,num_blocks;
int seg_start,ch,row,col;
char save_temps;
long file_size;

	read_curser_pos();
	turn_off_curser();
	save_win();

	selection = 0;

	while(99)
	{
display_main_menu:

		selection = choose_menu("System Control Functions",
			main_menu_text,10,0);



		switch(selection)
		{
			case 10 : /*exit this program*/
			{
				make_win(45,15,64,19);
				clear_win(REVERSE_VIDEO);
				draw_box();
				goto_row_col(win_y_low + 1,win_x_low + 2);
				printf("EXIT TO DOS: ");
				goto_row_col(win_y_low + 2,win_x_low + 3);
				printf("Are You");
				goto_row_col(win_y_low + 3,win_x_low + 3);
				printf("Sure? [y/N]:");

				make_win(10,3,60,17);

				ch = getch();

				if((ch != 'Y') && (ch != 'y'))
				{
					selection = 0;
					break;
				}


				if(have_display_list == TRUE)
					dealloc_pages_for_handle(handle);


			exit_stuff:
				clear_display_page();
				close_board();
				close_host();
				turn_on_curser();
				goto_row_col(0,0);

				restore_int9();

				exit(1);
			}


			case 2 : /*Enter baud rate*/
			{
				if(enter_baud_rate() == 'e')
					goto exit_main_menu;

				break;


			}




			case 1 : /*status information*/
			{
				if(emulation_status() == 'e')
					goto exit_main_menu;


				break;
			}





			case 5 : /*save part display*/
			{
				if(save_the_part() == 'e')
					goto exit_main_menu;

				break;

			}


			case 3 : /*crosshair control*/
			{
				if(cursor_speed_set() == 'e')
					goto exit_main_menu;

				break;
			}


			case 6 : /*our switches*/
			{
				if(our_switches() == 'e')
					goto exit_main_menu;

				break;
			}


			case 7 : /*make HP plotter file*/
			{
				if(menu_plot_file() == 'e')
					goto exit_main_menu;

				break;
			}


			case 4 : /*diagnostics*/
			{
				if(error_log_status() == 'e')
					goto exit_main_menu;

				break;
			}


			case REJECT :
			case ENTRY_COMPLETE :
			case TERMINATE :
			case 0x47 : /*return to caller*/
			{
	exit_main_menu:

				restore_win();
				turn_on_curser();
				goto_row_col(row_curser,col_curser);
				return;
			}

		}   /* end switch */

	}  /* end while */

}





void far mouse_reset()
{
	my_input_regs.x.ax = 0;
	int86(0x33,&my_input_regs,&my_output_regs);

	if(my_output_regs.x.ax == 0)
	{
		putchar(7);
		printf("Joystick/Mouse Not Installed\n");
	}
	
}







void far joystick()
{
int dx,dy,ch;

	my_input_regs.x.ax = 11;     /*raw mouse movements*/
	int86(0x33,&my_input_regs,&my_output_regs);

	dx = my_output_regs.x.cx;
	dy = my_output_regs.x.dx;


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
	{
		horizontal_line();
		vertical_line();
	}


	crosshair_column += dx;
	crosshair_row += dy;


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


void far clear_tek()
{
int row,col;

	clear_screen();
	graphics_mode_flag = FALSE;
	bypass = FALSE;
	point_plot_flag = FALSE;
	
	if(crosshair_flag == TRUE)
		clear_crosshair();

	my_x = 0;
	my_y = 9;

	if(have_display_list == TRUE)
	{
		current_insert_pointer = (char far *)start_of_page_frame_0;
		current_logical_page = 0;
		map_handle_page(0,0,handle);
		write_display_error = FALSE;
	}


	save_part_write_error = FALSE;
	plot_file_write_error = FALSE;
}




void far display_indicator(x,y)
int x,y;
{
	if(have_display_list == TRUE)
	{
		an_element.op_code = 7;
		an_element.arg1 = x;
		an_element.arg2 = y;

		if(erase_mode_flag == FALSE)
			an_element.color = syscolor_index;
		else
			an_element.color = current_color;

		an_element.font = line_style;

		insert_word();
	}


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

/*	extra = 0;
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


			if(point_plot_flag == FALSE)  /*vector*/
			{
				if(new_entity_flag == FALSE)   /*draw vector*/
				{
					draw_line(my_x,my_y,x,y);

					if(have_display_list == TRUE)
					{
						an_element.op_code = 1;
						an_element.arg1 = my_x;
						an_element.arg2 = my_y;
						an_element.arg3 = x;
						an_element.arg4 = y;
						an_element.color = current_color;
						an_element.font = line_style;

						insert_word();

					}

					my_x = x;
					my_y = y;
					pointer = 0;
				}
				else    /*move*/
				{
					move_pen_up(x,y);

					if(have_display_list == TRUE)
					{
						an_element.op_code = 6;
						an_element.arg1 = x;
						an_element.arg2 = y;

						insert_word();
					}


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

				if(have_display_list == TRUE)
				{
					an_element.op_code = 5;
					an_element.arg1 = x;
					an_element.arg2 = y;
					an_element.color = current_color;
					an_element.font = line_style;

					insert_word();
				}

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


			if(have_display_list == TRUE)
			{
				an_element.op_code = 2;
				an_element.arg1 = 0;

				insert_word();
			}

			break;
		}

		case 'c' : /*dashed*/
		{
			line_style = 1;
			my_dash_enable(1);

			if(have_display_list == TRUE)
			{
				an_element.op_code = 2;
				an_element.arg1 = 1;

				insert_word();
			}

			break;
		}

		case 'a' : /*phantom*/
		{
			line_style = 4;
			my_dash_enable(4);

			if(have_display_list == TRUE)
			{
				an_element.op_code = 2;
				an_element.arg1 = 4;

				insert_word();
			}

			break;
		}

		case 'b' : /*centerline*/
		{
			line_style = 2;
			my_dash_enable(2);

			if(have_display_list == TRUE)
			{
				an_element.op_code = 2;
				an_element.arg1 = 2;

				insert_word();
			}

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

						temp_col_hair = (int)
							((crosshair_column - 512
							- pan_x/4)/new_zoom) + 
								512;
						temp_row_hair = (int)
							((crosshair_row - 384
							+ pan_y/4)/new_zoom) +
								384;

						if(tek_mode_flag == FALSE)
							ugout(29);
						ugout(0xbb);

						ugout(((temp_col_hair >> 5)
							& 0x1f) | 32);
						ugout((temp_col_hair & 0x1f)
							| 32);

						temp1_y = (long)(768 - temp_row_hair)
							*10156L;
						temp_y = (int)(temp1_y/10000L);
						remainder = temp1_y%10000L;
						if(remainder >= 5078L)
							temp_y++;

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




void far megatek_update()
{

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
	test_board();
	i = init_artist10();

	my_set_viewport(0,0,1023,767,0);
	my_lock_viewport();



	if(have_display_list == TRUE)
	{
		get_board_status();
		page_frame_segment_address = get_page_frame_address();
		num_unalloc_pages = get_unalloc_page_count();
		handle = allocate_pages(num_unalloc_pages);
		start_of_page_frame_0 = (short far *)((long)
			page_frame_segment_address << 16);


		current_logical_page = 0;
		current_insert_pointer = (char far *)start_of_page_frame_0;
		map_handle_page(0,0,handle);
	}




	mouse_reset();

/*open and clear out display file

	if(have_display_list == TRUE)
	{
		sprintf(display_name,"%c:display",drive_letter);
		display_file = fopen(display_name,"w+b");
	}
*/


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




	init_terminal(); /*sets up phantom line style*/

	make_win(10,3,60,16);

	my_x = 0;
	my_y = 0;
	text_col = 0;
	text_row = 0;

	open_host(init_baud_rate);
	open_host(init_baud_rate);
	icu();
}

