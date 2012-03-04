#include "display.h"
#include "dos.h"
#include "generics.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "graph.h"


#define EGA_BOARD  1
#define VGA_BOARD  2



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



#define USE_COLOR_CARD  1
#define USE_MONO_CARD  0


extern void far switch_display_page(int);

extern void far restore_int9();
extern void far set_int9();

extern char far *mono_ptr;
extern char far my_text_mode;
extern char far my_test_flag;


extern void far customer_id();
extern void far clrall();


/*display file variables & functions*/
char far black = FALSE;
extern int far keyboard_status();
extern void far clear_display_page();
extern void far close_host();



extern void far make_little_crosshair();


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


extern char far graphics_board;   /* EGA or VGA */


char far my_clear_tek_flag = FALSE;




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



char far diag_string[200];


char far display_name[66];
int far temp_row_hair,far temp_col_hair;

int far name_char_count = 0;
int far view_name_col,far view_name_row;


void far which_icu()
{
	ugout(0x1e);
	ugout('o');
	ugout('!');
	ugout('&');
	ugout('%');
}







void far mouse_reset()
{
	my_input_regs.x.ax = 0;
	int86(0x33,&my_input_regs,&my_output_regs);

	if(my_output_regs.x.ax == 0)
	{
		printf("Joystick/Mouse Not Installed\n");
		printf("Press Enter To Continue\n");
		getch();
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




void far set_graphics_mode(the_mode)
int the_mode;
{
	graphics_mode_flag = the_mode;


	if(the_mode == TRUE)
		name_char_count = 0;
	else
	{
		view_name_col = my_x;
		view_name_row = my_y;
	}
}




void far open_display_file()
{

}


void far close_display_file()
{

}


void far clear_tek()
{
	my_clear_tek_flag = TRUE;

/*	clear_screen();*/

	set_graphics_mode(FALSE);
	bypass = FALSE;
	point_plot_flag = FALSE;
	
	if(crosshair_flag == TRUE)
		clear_crosshair();

	my_x = 0;
	my_y = 9;
}




void far display_indicator(x,y)
int x,y;
{
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

					my_x = x;
					my_y = y;
					pointer = 0;
				}
				else    /*move*/
				{
					move_pen_up(x,y);

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

			break;
		}

		case 'c' : /*dashed*/
		{
			line_style = 1;
			my_dash_enable(1);

			break;
		}

		case 'a' : /*phantom*/
		{
			line_style = 4;
			my_dash_enable(4);

			break;
		}

		case 'b' : /*centerline*/
		{
			line_style = 2;
			my_dash_enable(2);

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
			set_graphics_mode(FALSE);
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
				set_graphics_mode(FALSE);
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
							crosshair_column;
						temp_row_hair = 
							crosshair_row;

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

				set_graphics_mode(FALSE);
				display_charactor(command);

				break;
			}

			case 31 :
			{
				bypass = FALSE;
				point_plot_flag = FALSE;

				if(crosshair_flag == TRUE)
					clear_crosshair();
		
				set_graphics_mode(FALSE);

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
							set_graphics_mode(TRUE);
							point_plot_flag = TRUE;

							break;
						}
	
						case 30 :
						{
							set_graphics_mode(FALSE);
							break;
						}

						case 29 :
						{
							set_graphics_mode(TRUE);
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








main(argc,argv)
int argc;
char far *argv[];
{
int i,row,col;



	if(argc > 1)
	{
		if(strcmp(strupr(argv[1]),"/VGA") == 0)
			graphics_board = VGA_BOARD;
		else if(strcmp(strupr(argv[1]),"/EGA") == 0)
			graphics_board = EGA_BOARD;
		else
		{
			putchar(7);
			printf("\n\nInvalid Graphics Board Option\n\n");
			printf("EGA Mode Will Be Used\n\n");
			printf(" Press Enter To Continue\n");
			getch();
		}
	}
	else
		graphics_board = EGA_BOARD;




	FP_SEG(mono_ptr) = 0xb000;
	FP_OFF(mono_ptr) = 0;

	set_int9();

	clear_display_page();
	clrall();


	customer_id();


	test_board();
	i = init_artist10();

	my_set_viewport(0,0,1023,767,0);
	my_lock_viewport();

	mouse_reset();

	make_little_crosshair();


	init_terminal(); /*sets up phantom line style & d90 colors*/



	my_x = 0;
	my_y = 0;
	text_col = 0;
	text_row = 0;


	_setvideomode(_TEXTC80);
	my_text_mode = USE_COLOR_CARD;
	my_test_flag = FALSE;



/*
	my_input_regs.h.ah = 15;
	int86(0x10,&my_input_regs,&my_output_regs);


	putchar(7);
	printf("BEFORE ENTERING ICU(): page = %d  video mode = %d\n\n",
		my_output_regs.h.bh,my_output_regs.h.al);

*/


	open_host(init_baud_rate);
	open_host(init_baud_rate);
	icu();
}

