#include "display.h"
#include "dos.h"
#include "generics.h"
#include "stdio.h"
#include "stdlib.h"


#define TRUE 1
#define FALSE 0
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


extern void far restore_int9();
extern void far set_int9();


extern char far save_part_write_error;
extern char far plot_file_write_error;


/*display file variables & functions*/
char far black = FALSE;
struct a_word far an_element,far read_element;
char far write_display_error = FALSE;
char far quit_redisplay = FALSE;
extern void far insert_word(void);
extern void far get_word(void);
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



extern void far print_id();


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
char far drive_letter = 'h';
int far curser_ratio = 1;
int far mouse_phase = 0,far button_byte = 0,far my_pen_press = 0;
char far depress_flag = FALSE;

FILE far *display_file,far *out_ptr = NULL,far *display_save = NULL;
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
char far version_str[80] = "1.27    7/19/89";
int far list_size = 0;
char far print_list = FALSE;
int far time_redisplay;
int far num_pages = 2;
char far do_garbage = FALSE;


/*plot file variables*/
int far plot_x_offset = 0;
int far plot_y_offset = 0;
float far plot_scale_factor = 1.0;
extern void far make_plot_file();
char far hp_plotter_name[80];
char far parameter_string[30];




unsigned char far megatek_data[44];



void far which_icu()
{
	ugout(0x1e);
	ugout('o');
	ugout('!');
	ugout('&');
	ugout('%');
}







char far error_log_status()
{
int ch;

error_log_again:
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("   Diagnostics");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("      F1 List Errors");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("      F2 Print Errors");
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("      F3 Clear Errors");

	ch = getch();
	if(ch != 0)
		goto error_log_again;
	
	ch = getch();

	switch(ch)
	{
		case 0x3b : /*display it*/
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
			make_win(10,3,60,16);
			save_win();

			/*remove blinking "ERRORS" message*/
			normal_string("        ",24,2);
			errors_happened = 'f';

			goto error_log_again;

			break;
		}

		case 0x3c : /*print it*/
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
			make_win(10,3,60,16);
			save_win();

			/*remove blinking "ERRORS" message*/
			normal_string("        ",24,2);
			errors_happened = 'f';

			goto error_log_again;

			break;
		}

		case 0x3d : /*clear it*/
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

		case 75 : /*reject*/
		case 77 : /*entry complete*/
				return('c');

		case 80 : /*terminate*/
				return('e');

		default :
			goto error_log_again;
	}

	return('c');
}







char far generate_plot_file()
{
int ch,i;

generate_again:
	clear_win(REVERSE_VIDEO);
	draw_box();


	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("    HPGL: Generate The File");

	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("       Enter HP Plotter File's Full Pathname:");
	goto_row_col(win_y_low + 5,win_x_low + 8);


	i = 0;
	for(ch = getch() ; (ch != 13) && (ch != 0) ;
		ch = getch())
	{
		if(ch != 8)
		{
			putchar(ch);
			hp_plotter_name[i] = ch;
			i++;
		}
		else
		{
		    if(i > 0)
		    {
			i--;

			goto_row_col(win_y_low + 5,win_x_low + 8 + i);
			putchar(' ');
			goto_row_col(win_y_low + 5,win_x_low + 8 + i);

		    }
		}	
	}

	hp_plotter_name[i] = '\0';

	if(ch == 0)
	{
		ch = getch();

		if((ch == 75) || (ch == 77))
			return('c');
		else if(ch == 80)
			return('e');
	}

	hp_plotter = fopen(hp_plotter_name,"w");
			
	if(hp_plotter == NULL)
	{
		goto_row_col(win_y_low + 7,win_x_low + 1);
		printf("         Invalid Path Name");
		goto_row_col(win_y_low + 9,win_x_low + 1);
		printf("         Press Enter To Continue");
		ch = getch();

		goto generate_again;
	}

	make_plot_file();

	plot_scale_factor /= 3.628571429;

	fclose(hp_plotter);

	return('c');
}



char far hp_parameters()
{
int ch;

parameters_again:
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("   HPGL Parameters");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("       F1 X Offset");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("       F2 Y Offset");
	goto_row_col(win_y_low + 5,win_x_low + 1);
	printf("       F3 Scale");


	goto_row_col(win_y_low + 7,win_x_low + 1);
	printf("       Current Values Are:");
	goto_row_col(win_y_low + 9,win_x_low + 1);
	printf("          X Offset = %d",plot_x_offset);
	goto_row_col(win_y_low + 10,win_x_low + 1);
	printf("          Y Offset = %d",plot_y_offset);
	goto_row_col(win_y_low + 11,win_x_low + 1);
	printf("          Scale = %f",plot_scale_factor);

	ch = getch();
	if(ch != 0)
		goto parameters_again;

	ch = getch();

	switch(ch)
	{
		case 0x3b : /*x offset*/
		{
			clear_win(REVERSE_VIDEO);
			draw_box();

			goto_row_col(win_y_low + 5,win_x_low + 1);
			printf("       Enter X Offset: ");
			scanf("%s",parameter_string);
			plot_x_offset = atoi(parameter_string);

			goto parameters_again;
		}

		case 0x3c : /*y offset*/
		{
			clear_win(REVERSE_VIDEO);
			draw_box();

			goto_row_col(win_y_low + 5,win_x_low + 1);
			printf("       Enter Y Offset: ");
			scanf("%s",parameter_string);
			plot_y_offset = atoi(parameter_string);

			goto parameters_again;
		}


		case 0x3d : /*scale*/
		{
			clear_win(REVERSE_VIDEO);
			draw_box();

			goto_row_col(win_y_low + 5,win_x_low + 1);
			printf("       Enter Scaling Factor: ");
			scanf("%s",parameter_string);
			plot_scale_factor = atof(parameter_string);

			goto parameters_again;
		}

		case 75 :
		case 77 :
			return('c');

		case 80 : 
			return('e');

		default :
			goto parameters_again;
	}
}





char far menu_plot_file()
{
int ch;

/*set plotting parameters to their defaults*/


main_plot_again:
	clear_win(REVERSE_VIDEO);
	draw_box();

	goto_row_col(win_y_low + 1,win_x_low + 1);
	printf("    HPGL Output");
	goto_row_col(win_y_low + 3,win_x_low + 1);
	printf("        F1 Generate Output");
	goto_row_col(win_y_low + 4,win_x_low + 1);
	printf("        F2 Parameters");


	ch = getch();

	if(ch != 0)
		goto main_plot_again;

	ch = getch();

	switch(ch)
	{
		case 0x3b : /*generate output*/
		{
			if(generate_plot_file() == 'e')
				return('e');

			goto main_plot_again;
		}

		case 0x3c : /*parameters*/
		{
			if(hp_parameters() == 'e')
				return('e');

			goto main_plot_again;
		}

		case 75 :
		case 77 :
			return('c');

		case 80 :
			return('e');

		default :
			goto main_plot_again;
	}
}




char far curser_speed_set()
{
int ch,save_current_color,x_mouse,y_mouse,dx,dy;

	x_mouse = 512;
	y_mouse = 384;
	dx = dy = 0;

	save_current_color = current_color;
	my_color(7);

	clear_win(REVERSE_VIDEO);
	draw_box();


	goto_row_col(win_y_low + 1,win_x_low + 5);
	printf("Crosshair Control");
	goto_row_col(win_y_low + 2,win_x_low + 5);
	printf("Press Mouse/Joystick Buttons");
	goto_row_col(win_y_low + 4,win_x_low + 8);
	printf("  1  Increase Curser Speed");
	goto_row_col(win_y_low + 5,win_x_low + 8);
	printf("  2  Decrease Curser Speed");
	goto_row_col(win_y_low + 7,win_x_low + 8);
	printf("  (F1 To Center Cursor)");

	my_pen_press = 0;

	draw_mode(3);
	my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
	my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);

	while((my_pen_press & 2) == 0)
	{
		if(keyboard_status() != -1) /*keyboard pressed*/
		{
			ch = getch();

			if(ch == 0)  /*extended code*/
			{
				ch = getch();

				if(ch == 0x3b)  /*F1*/
				{
					goto_row_col(win_y_low + 7,
						win_x_low + 10);
					printf("Cursor Will Be Reset To 0 0 Upon Return");
					crosshair_column = 512;
					crosshair_row = 384;
				}
				else if((ch == 75) || (ch == 77))
				{   /*undraw curser speed curser*/
					my_line_xy(x_mouse - 50,y_mouse,
						x_mouse + 50,y_mouse);
					my_line_xy(x_mouse,y_mouse - 50,
						x_mouse,y_mouse + 50);
					draw_mode(0);
					my_color(save_current_color);
					return('c');
				}
				else if(ch == 80)
				{  /*undraw curser speed curser*/
					my_line_xy(x_mouse - 50,y_mouse,
						x_mouse + 50,y_mouse);
					my_line_xy(x_mouse,y_mouse - 50,
						x_mouse,y_mouse + 50);
					draw_mode(0);
					my_color(save_current_color);
					return('e');
				}
			}
		}


		goto_row_col(win_y_low + 10,win_x_low + 10);
		if(curser_ratio > 0)
			printf("Curser Speed = %d            ",curser_ratio);
		else if(curser_ratio < 0)
			printf("Curser Speed = %f",-1.0/curser_ratio);
		else
			printf("Curser Speed = 1             ");


		my_input_regs.x.ax = 11;  /*raw mouse movements*/
		int86(0x33,&my_input_regs,&my_output_regs);

		dx = my_output_regs.x.cx;
		dy = my_output_regs.x.dx;

		if(curser_ratio > 0)
		{
			dx *= curser_ratio;
			dy *= curser_ratio;
		}
		else
		{
			dx /= -(curser_ratio);
			dy /= -(curser_ratio);
		}

		if((dx != 0) || (dy != 0))
		{  /*undraw curser speed curser*/
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
				curser_ratio =  -1;
		}
		
	}   /*end while*/

    /*undraw cursor speed curser*/
	my_line_xy(x_mouse - 50,y_mouse,x_mouse + 50,y_mouse);
	my_line_xy(x_mouse,y_mouse - 50,x_mouse,y_mouse + 50);
	draw_mode(0);
	my_color(save_current_color);
	return('c');

}  /*end curser speed set*/






char far our_switches()
{
int ch;
	while(9)
	{
switch_again:
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("  Switches");


		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("     F1 Printer Control     :  ");
		if(print_list == TRUE)
		{
			printf("Enabled");
		}
		else
			printf("Disabled");


		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("     F2 Update Timer        :  ");
		if(time_redisplay == TRUE)
		{
			printf("Enabled");
		}
		else
			printf("Disabled");



		ch = getch();
		if(ch != 0)  /*not an extended code*/
			goto switch_again;

		ch = getch();  /*get actual value*/

		switch(ch)
		{
			case 0x3b : /*send/don't send listings to the printer*/
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


			case 0x3c : /*time/don't time redisplays*/
			{
				if(time_redisplay == TRUE)
					time_redisplay = FALSE;
				else
					time_redisplay = TRUE;

				break;
			}

			case 75 :  /*reject*/
			case 77 :  /*entry complete*/
				return('c');

			case 80 :  /*terminate*/
				return('e');

			default :  goto switch_again;

		}  /*end switch*/

	}  /*end while*/
}






void far menu()
{
int selection,i,subroutine_num,sub_end,sub_ptr,baud_rate,auto_manual,
	list_print,redis_time;
unsigned long id,displacement,temp,num_blocks;
int seg_start,ch,row,col;
char name_plot_file[100],name_save_display[100],save_temps;
long file_size;

	read_curser_pos();
	turn_off_curser();
	save_win();

	selection = 0;

	while(selection != 0x44)
	{
display_main_menu:
		clear_win(REVERSE_VIDEO);
		draw_box();

		goto_row_col(win_y_low + 1,win_x_low + 1);
		printf("   System Control Functions");
		goto_row_col(win_y_low + 3,win_x_low + 1);
		printf("     F1 Configuration");
		goto_row_col(win_y_low + 4,win_x_low + 1);
		printf("     F2 Communications");
		goto_row_col(win_y_low + 5,win_x_low + 1);
		printf("     F3 Crosshair Control");
		goto_row_col(win_y_low + 6,win_x_low + 1);
		printf("     F4 Save Display");
		goto_row_col(win_y_low + 7,win_x_low + 1);
		printf("     F5 Switches");
		goto_row_col(win_y_low + 8,win_x_low + 1);
		printf("     F6 HPGL Output");
		goto_row_col(win_y_low + 9,win_x_low + 1);
		printf("     F7 Diagnostics");

		goto_row_col(win_y_low + 12,win_x_low + 1);
		printf("    F10 EXIT TO DOS");


		ch = getch();
		if(ch != 0)
		{
			continue;
		}
		
		selection = getch();

		switch(selection)
		{
			case 0x44 : /*exit this program*/
			{
				clear_win(REVERSE_VIDEO);
				draw_box();
				goto_row_col(win_y_low + 1,win_x_low + 3);
				printf("EXIT TO DOS: ");
				goto_row_col(win_y_low + 3,win_x_low + 5);
				printf("Are You Sure? [y/N]: ");

				ch = getch();

				if((ch != 'Y') && (ch != 'y'))
				{
					selection = 0;
					break;
				}


				clear_win(REVERSE_VIDEO);
				draw_box();
				goto_row_col(win_y_low + 1,win_x_low + 3);
				printf("EXIT TO DOS");
				goto_row_col(win_y_low + 3,win_x_low + 5);
				printf("Save Display Files For Diagnostics? [y/N]");
				

				save_temps = getch();


				fclose(display_file);

			/*delete the segment files*/
				if((save_temps != 'y') && (save_temps != 'Y'))
				{
					sprintf(display_name,"%c:display",
						drive_letter);

					remove(display_name);
				}

				clear_display_page();
				close_board();
				close_host();
				turn_on_curser();
				goto_row_col(0,0);

				restore_int9();

				exit(1);
			}


			case 0x3c : /*Enter baud rate*/
			{
	baud_again:
				clear_win(REVERSE_VIDEO);
				draw_box();

				goto_row_col(win_y_low + 1,win_x_low + 1);
				printf("   Select Baud Rate");
				goto_row_col(win_y_low + 3,win_x_low + 1);
				printf("      F1 1200");
				goto_row_col(win_y_low + 4,win_x_low + 1);
				printf("      F2 2400");
				goto_row_col(win_y_low + 5,win_x_low + 1);
				printf("      F3 4800");
				goto_row_col(win_y_low + 6,win_x_low + 1);
				printf("      F4 9600"); 
				goto_row_col(win_y_low + 7,win_x_low + 1);
				printf("      F5 19200");


				switch(init_baud_rate)
				{
					case 96 :
					{
						goto_row_col(win_y_low + 3,
							win_x_low + 9);
						break;
					}

					case 48 :
					{
						goto_row_col(win_y_low + 4,
							win_x_low + 9);
						break;
					}

					case 24 :
					{
						goto_row_col(win_y_low + 5,
							win_x_low + 9);
						break;
					}
					

					case 12 :
					{
						goto_row_col(win_y_low + 6,
							win_x_low + 9);
						break;
					}

					case 6 :
					{
						goto_row_col(win_y_low + 7,
							win_x_low + 9);
						break;
					}
				}


				putchar('>');


				ch = getch();
				if(ch != 0)
					goto baud_again;
				

				ch = getch();

				switch(ch)
				{
					case 0x3b :   /*1200*/
					{
						init_baud_rate = 96;
						open_host(96);
						goto baud_again;   
					}

					case 0x3c :   /*2400*/
					{
						init_baud_rate = 48;
						open_host(48);
						goto baud_again;   
					}

					case 0x3d :   /*4800*/
					{
						init_baud_rate = 24;
						open_host(24);
						goto baud_again;   
					}

					case 0x3e :   /*9600*/
					{
						init_baud_rate = 12;
						open_host(12);
						goto baud_again;   
					}

					case 0x3f :   /*19200*/
					{
						init_baud_rate = 6;
						open_host(6);
						goto baud_again;   
					}

					case 75 : /*reject*/
					case 77 : /*entry complete*/
						goto display_main_menu;

					case 80 : /*terminate*/
						goto exit_main_menu;

					default :
						goto baud_again;
				}

				break;
			}




			case 0x3b : /*status information*/
			{
				clear_win(REVERSE_VIDEO);
				draw_box();

				goto_row_col(win_y_low + 1,win_x_low + 1);
				printf("   Configuration Info");
				goto_row_col(win_y_low + 3,win_x_low + 1);
				printf("        %s Board",board_string);
				goto_row_col(win_y_low + 4,win_x_low + 1);
				printf("        Emulation Mode: D90");



				goto_row_col(win_y_low + 5,win_x_low + 1);
				printf("        Version# %s",version_str);
				print_id();

				file_size = ftell(display_file);


				goto_row_col(win_y_low + 11,win_x_low + 1);
				printf("        Display Memory Size = %ldKB",
					file_size/1024L);

		status_again:
				ch = getch();

				if(ch != 0)
					goto status_again;
				else
				{
					ch = getch();

					if((ch == 75) || (ch == 77))
						goto display_main_menu;
					else if(ch == 80)
						goto exit_main_menu;
					else
						goto status_again;
				}

				break;
			}





			case 0x3e : /*save part display*/
			{
		save_again:
				clear_win(REVERSE_VIDEO);
				draw_box();

				goto_row_col(win_y_low + 1,win_x_low + 1);
				printf("   Save Display");
				goto_row_col(win_y_low + 3,win_x_low + 1);
				printf("   Enter Save Part File's Full Path Name:");
				goto_row_col(win_y_low + 5,win_x_low + 5);

				i = 0;
				for(ch = getch() ; (ch != 13) && (ch != 0) ;
					ch = getch())
				{
					if(ch != 8)
					{
						putchar(ch);
						name_save_display[i] = ch;
						i++;
					}
					else
					{
						if(i > 0)
						{
							i--;

							goto_row_col(win_y_low
							 + 5,win_x_low + 5 + i);
							putchar(' ');
							goto_row_col(win_y_low
							 + 5,win_x_low + 5 + i);
						}
					}
				}

				name_save_display[i] = '\0';

				if(ch == 0)
				{
					ch = getch();

					if((ch == 75) || (ch == 77))
						goto display_main_menu;
					else if(ch == 80)
						goto exit_main_menu;
				}

				display_save = fopen(name_save_display,"w+b");
			
				if(display_save == NULL)
				{
					goto_row_col(win_y_low + 8,win_x_low + 1);
					printf("   Invalid Path Name");
					goto_row_col(win_y_low + 10,win_x_low + 1);
					printf("   Press Enter To Continue");
					ch = getch();

					goto save_again;
				}

				save_part_display();

				fclose(display_save);
				display_save = NULL;

				break;
			}


			case 0x3d : /*crosshair control*/
			{
				if(curser_speed_set() == 'e')
					goto exit_main_menu;

				break;
			}


			case 0x3f : /*our switches*/
			{
				if(our_switches() == 'e')
					goto exit_main_menu;

				break;
			}


			case 0x40 : /*make HP plotter file*/
			{
				if(menu_plot_file() == 'e')
					goto exit_main_menu;

				break;
			}


			case 0x41 : /*diagnostics*/
			{
				if(error_log_status() == 'e')
					goto exit_main_menu;

				break;
			}


			case 75 :
			case 77 :
			case 80 :
			case 0x47 : /*return to caller*/
			{
	exit_main_menu:
				clear_win(NORMAL_VIDEO);
				restore_win();
				turn_on_curser();
				goto_row_col(row_curser,col_curser);
				return;
			}


			default :
			{ 
				break;
			}
		}


	}
}





void far mouse_reset()
{
	my_input_regs.x.ax = 0;
	int86(0x33,&my_input_regs,&my_output_regs);

	if(my_output_regs.x.ax == 0)
		printf("Joystick/Mouse Not Installed\n");

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

	#ifdef HAVE_DISP_FILE


		fclose(display_file);
		sprintf(display_name,"%c:display",drive_letter);
		display_file = fopen(display_name,"w+b");

		write_display_error = FALSE;

	#endif

	save_part_write_error = FALSE;
	plot_file_write_error = FALSE;
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
	extra = 0;

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

	mouse_reset();

/*open and clear out display file*/
	sprintf(display_name,"%c:display",drive_letter);
	display_file = fopen(display_name,"w+b");



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