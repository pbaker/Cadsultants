#include "stdio.h"
#include "c:\emulate\common\display.h"
#include "generics.h"
#include "dos.h"
#include "time.h"
#include "expanded.h"



#define END_OF_PAGE 79
#define END_OF_DL  75

#define TRUE  't'
#define FALSE  'f'


#define X_MAX 1023
#define Y_MAX 767
#define X_MIN 0
#define Y_MIN 0


extern int far current_logical_page;



/*bios calls*/
extern void far blinking_string();
extern void far normal_string();
extern void far goto_row_col(int,int);
extern void far read_curser_pos(void);
extern void far turn_off_curser(void);
extern void far turn_on_curser(void);
extern void far clear_win(int);
extern void far save_win(void);
extern void far restore_win(void);
extern void far draw_box(void);
extern int far win_y_low;
extern int far win_x_low;
extern int far win_y_up;
extern int far win_x_up;
#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1


extern void far my_pckb();
extern void far read_keyboard();
extern int far keyboard_status();
extern void far horizontal_line();
extern void far vertical_line();




extern int far row_curser;
extern int far col_curser;
extern struct a_word far an_element;
extern struct a_word far read_element;
extern char far write_display_error;
extern char far drive_letter;
extern char far quit_redisplay;
extern char far quit_delete;
extern int far my_x;
extern int far my_y;
extern int far curser_ratio;
extern int far mouse_phase;
extern int far button_byte;
extern int far my_pen_press;
extern char far depress_flag;
extern int far current_color;
extern int far syscolor_index;
extern int far line_style;
extern char far time_redisplay;
extern char far type_char_read;
extern char far value_char_read;
extern char far crosshair_flag;

extern union REGS far my_input_regs;
extern union REGS far my_output_regs;


/*for error logging*/
extern char far diag_string[];
extern void far error_logger();



float far old_zoom = 1.0,far new_zoom = 1.0;
int far pan_x = 0,far pan_y = 0;
int far mouse_x = 0,far mouse_y = 0;
char far zoom_pan_enable = FALSE;
int far x_zooms_pan = 0,far y_zooms_pan = 0;
int far x_pan = 0,far y_pan = 0;
int far old_x_zooms_pan = 0,far old_y_zooms_pan = 0;
int far num_zooms_in = 0,num_zooms_out = 0;
int far center_x,far center_y;
char far alt_u_redisplay;


char far clear_delete[66],far string_out[80];
char far doing_redisplay = FALSE;


char far *ems_read_pointer;
int far ems_read_page;



void far read_mouse()
{
int dx,dy;

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


	mouse_x += dx;
	mouse_y += dy;

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

}




void far insert_word()
{
int row,col;


	if(write_display_error == FALSE)
	{
		insert_megatek();

#ifdef kerekrkrkkrkkt
		fwrite(&an_element,SIZE_A_WORD,1,display_file);
		if(ferror(display_file) != 0)
		{
			write_display_error = TRUE;

			sprintf(diag_string,
			     "*****ERROR***** DISPLAY FILE IS TOO LARGE TO FIT ON DRIVE %c:\n",
				drive_letter);
			error_logger(diag_string);

			sprintf(diag_string,"NOTHING MORE WILL BE STORED\n");
			error_logger(diag_string);

			sprintf(diag_string,"File: row = %d , col = %d\n",
				row,col);
			error_logger(diag_string);
		}
#endif
	}
}




void far get_word()
{
	memcpy(&read_element,ems_read_pointer,SIZE_A_WORD);
	ems_read_pointer = ems_read_pointer + SIZE_A_WORD;


	if(read_element.op_code == END_OF_PAGE)
	{
		ems_read_page++;
		map_handle_page(ems_read_page,0,handle);
		ems_read_pointer = (char far *)start_of_page_frame_0;

		memcpy(&read_element,ems_read_pointer,SIZE_A_WORD);
		ems_read_pointer = ems_read_pointer + SIZE_A_WORD;
	}
}










void far redisplay()
{
int save_my_x,save_my_y,y,save_row_curser,save_col_curser,row,col;
long temp_y,remainder,start_time,end_time,dummy;

	zoom_pan_enable = TRUE;

	save_my_x = my_x;
	save_my_y = my_y;
	save_row_curser = row_curser;
	save_col_curser = col_curser;
	my_x = 0;
	my_y = 0;



	read_curser_pos();
	sprintf(string_out,"Screen Update       ");
	blinking_string(string_out,24,15);


	if(time_redisplay == TRUE)
	{
		start_time = time(&dummy);
		blinking_string("TIMER              ",24,50);
	}


	doing_redisplay = TRUE;


	ems_read_page = 0;
	ems_read_pointer = (char far *)start_of_page_frame_0;
	map_handle_page(0,0,handle);
	

	for(get_word() ; read_element.op_code != END_OF_DL ; get_word())
	{
		my_pckb();
		if(quit_redisplay == TRUE)
			goto end_redisplay;



		switch(read_element.op_code)
		{
			case 1 : /*draw a line*/
			{
				draw_line(read_element.arg1,read_element.arg2,
					read_element.arg3,read_element.arg4);

				my_x = read_element.arg3;
				my_y = read_element.arg4;

				break;
			}

			case 2 : /*dash pattern*/
			{
				my_dash_enable(read_element.arg1 & 0x0f);
				break;
			}

			case 3 : /*set color*/
			{
				my_color(read_element.arg1 & 0x0f);
				break;
			}

			case 4 : /*draw a charactor*/
			{
				redisplay_charactor(read_element.a_char);

				break;
			}

			case 5 : /*plot a point*/
			{
				dot_plot(read_element.arg1,read_element.arg2);

				my_x = read_element.arg1;
				my_y = read_element.arg2;

				break;
			}

			case 6 : /*move*/
			{
				move_redisplay(read_element.arg1,read_element.arg2);

				break;
			}

			case 7 : /*display_indicator*/
			{
				my_circle(read_element.arg1,read_element.arg2);
				break;
			}


			default :
			{
				sprintf(diag_string,
				       "UNKNOWN OP CODE IN REDISPLAY = %d\n",
						read_element.op_code);
				error_logger(diag_string);
			}

		}

	}
	


end_redisplay:

	
	map_handle_page(current_logical_page,0,handle);


	doing_redisplay = FALSE;

	normal_string("                                                   ",24,4);



	if(time_redisplay == TRUE)
	{
		end_time = time(&dummy);
		normal_string("                  ",24,49);
		goto_row_col(24,50);
		printf(" %ld Seconds        ",end_time - start_time);
		putchar(0x07);
	}

	goto_row_col(row_curser,col_curser);

	my_x = save_my_x;
	my_y = save_my_y;
	row_curser = save_row_curser;
	col_curser = save_col_curser;

	zoom_pan_enable = FALSE;
}









void plus(x,y)
int x,y;
{
	my_dash_enable(0);
	my_color(7);
	draw_mode(3);
	my_line_xy(x - 12,y,x + 12,y);
	my_line_xy(x,y - 12,x,y + 12);
	draw_mode(0);
	my_color(current_color);
	my_dash_enable(line_style);
}



void far draw_zoom_box(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
int dx,dy,x_width,y_width;

	if((x1 == x2) && (y1 == y2))
		return;

	my_dash_enable(0);
	my_color(7);
	draw_mode(3);

	dx = x2 - x1;
	dy = y2 - y1;

	if(abs(dx) > abs(dy))  /*use dx for size*/
	{
		x_width = abs(dx);
		y_width = abs(dx)*3/4;
	}
	else  /*use dy for size*/
	{
		x_width = abs(dy);
		y_width = abs(dy)*3/4;
	}


	if(x1 < x2)
	{
		if(y1 < y2)
		{
			my_line_xy(x1,y1,x1 + x_width,y1);
			my_line_xy(x1,y1,x1,y1 + y_width);
			my_line_xy(x1,y1 + y_width,x1 + x_width,y1 + y_width);
			my_line_xy(x1 + x_width,y1,x1 + x_width,y1 + y_width);

			center_x = x1 + x_width/2;
			center_y = y1 + y_width/2;
		}
		else if(y1 > y2)
		{
			my_line_xy(x1,y1,x1 + x_width,y1);
			my_line_xy(x1,y1,x1,y1 - y_width);
			my_line_xy(x1,y1 - y_width,x1 + x_width,y1 - y_width);
			my_line_xy(x1 + x_width,y1,x1 + x_width,y1 - y_width);

			center_x = x1 + x_width/2;
			center_y = y1 - y_width/2;
		}
		else
		{
			my_line_xy(x1,y1,x2,y1);
			center_x = (x1 + x2)/2;
			center_y = y1;
		}
	}
	else if(x1 > x2)
	{
		if(y1 < y2)
		{
			my_line_xy(x1,y1,x1 - x_width,y1);
			my_line_xy(x1,y1,x1,y1 + y_width);
			my_line_xy(x1,y1 + y_width,x1 - x_width,y1 + y_width);
			my_line_xy(x1 - x_width,y1,x1 - x_width,y1 + y_width);

			center_x = x1 - x_width/2;
			center_y = y1 + y_width/2;
		}
		else if(y1 > y2)
		{
			my_line_xy(x1,y1,x1 - x_width,y1);
			my_line_xy(x1,y1,x1,y1 - y_width);
			my_line_xy(x1,y1 - y_width,x1 - x_width,y1 - y_width);
			my_line_xy(x1 - x_width,y1,x1 - x_width,y1 - y_width);

			center_x = x1 - x_width/2;
			center_y = y1 - y_width/2;
		}
		else
		{
			my_line_xy(x1,y1,x2,y1);
			center_x = (x1 + x2)/2;
			center_y = y1;
		}
	}
	else
	{
		my_line_xy(x1,y1,x1,y2);
		center_x = x1;
		center_y = (y1 + y2)/2;
	}


	draw_mode(0);
	my_color(current_color);
	my_dash_enable(line_style);
}




void far zoom_down_box(y_mouse)
int y_mouse;
{
float factor;
int x1,y1,x2,y2;

	factor = 20.0/(y_mouse + 20);

	x1 = (int)((1 - 512)*factor) + 512;
	y1 = (int)((1 - 384)*factor) + 384;
	x2 = (int)((1022 - 512)*factor) + 512;
	y2 = (int)((766 - 384)*factor) + 384;

	my_dash_enable(0);
	my_color(7);
	draw_mode(3);

	my_line_xy(x1,y1,x2,y1);
	my_line_xy(x1,y1,x1,y2);
	my_line_xy(x1,y2,x2,y2);
	my_line_xy(x2,y1,x2,y2);

	draw_mode(0);
	my_color(current_color);
	my_dash_enable(line_style);
}





void far pan_box(x,y)
int x,y;
{
	my_dash_enable(0);
	my_color(7);
	draw_mode(3);

	my_line_xy(x - 510,y - 382,x + 510,y - 382);
	my_line_xy(x - 510,y - 382,x - 510,y + 382);
	my_line_xy(x - 510,y + 382,x + 510,y + 382);
	my_line_xy(x + 510,y + 382,x + 510,y - 382);

	draw_mode(0);
	my_color(current_color);
	my_dash_enable(line_style);
}






void far undo_zooms()
{
	old_zoom = 1.0;
	new_zoom = 1.0;
	x_zooms_pan = 0;
	y_zooms_pan = 0;
	old_x_zooms_pan = 0;
	old_y_zooms_pan = 0;
	pan_x = x_pan = 0;
	pan_y = y_pan = 0;
	num_zooms_in = 0;
	num_zooms_out = 0;


	clear_screen();
	alt_u_redisplay = FALSE;

	if(crosshair_flag == TRUE)
	{
		horizontal_line();
		vertical_line();
	}

	redisplay();
}





void far zoom_pan()
{
int selection,new_x,new_y,old_x,old_y,x_1st,y_1st,x_2nd,y_2nd;

	old_x = old_y = new_x = new_y = 0;

	turn_off_curser();
	read_curser_pos();
	save_win();

	while(9)
	{
		clear_win(REVERSE_VIDEO);
		draw_box();	

		goto_row_col(win_y_low + 2,win_x_low + 5);
		printf("Zoom/Pan Control");
		goto_row_col(win_y_low + 4,win_x_low + 8);
		printf("F1 Original Scale");
		goto_row_col(win_y_low + 5,win_x_low + 8);
		printf("F2 Zoom In");
		goto_row_col(win_y_low + 6,win_x_low + 8);
		printf("F3 Zoom Out");
		goto_row_col(win_y_low + 7,win_x_low + 8);
		printf("F4 Pan");


		selection = getch();

		if(selection != 0)
		{
			continue;
		}
		else
		{
			selection = getch();

			switch(selection)
			{
				case 0x3e : /*pan*/
				{
					clear_win(REVERSE_VIDEO);
					draw_box();

					goto_row_col(win_y_low + 2,win_x_low + 5);
					printf("Pan");
					if(new_zoom <= 1.0)
					{
						goto_row_col(win_y_low + 4,
							win_x_low + 8);
						printf("You Must Zoom In Before Panning");
						goto_row_col(win_y_low + 6,
							win_x_low + 8);
						printf("Press Any Key To Continue");
						getch();

						break;
					}

					goto_row_col(win_y_low + 4,win_x_low + 8);
					printf("Select Pan Position By Moving Curser");

					mouse_x = old_x = new_x = 512;
					mouse_y = old_y = new_y = 384;
					my_pen_press = 0;

					pan_box(old_x,old_y);

					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if((value_char_read
									== 75) ||
								   (value_char_read
									== 80))
								{  /*get out*/
									pan_box(
									 old_x,
									 old_y);
									goto
									 pan_end;
								}
							}
						}

						read_mouse();
						new_x = mouse_x;
						new_y = mouse_y;

						if((old_x != new_x) ||
							(old_y != new_y))
						{
							pan_box(old_x,old_y);
							pan_box(new_x,new_y);

							old_x = new_x;
							old_y = new_y;
						}
					}

					pan_box(old_x,old_y);

					x_pan += (int)(4*(512 - old_x)/old_zoom);
					y_pan -= (int)(4*(384 - old_y)/old_zoom);

					pan_x += 4*(512 - old_x);
					pan_y -= 4*(384 - old_y);


					clear_screen();
					alt_u_redisplay = FALSE;

					if(crosshair_flag == TRUE)
					{
						horizontal_line();
						vertical_line();
					}

					redisplay();

				pan_end:
					break;
				}

				case 0x3c : /*zoom in*/
				{
					clear_win(REVERSE_VIDEO);
					draw_box();

					goto_row_col(win_y_low + 2,win_x_low + 5);
					printf("Zoom In");
					goto_row_col(win_y_low + 4,win_x_low + 8);
					printf("Select First Zoom Box Corner");

				first_corner:
					old_x = old_y = new_x = new_y = 0;
					plus(0,0);
					my_pen_press = 0;

					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if((value_char_read
									== 75) ||
								   (value_char_read
									== 80))
								{  /*get out*/
									plus(
									 old_x,
									 old_y);
									goto
									 zoom_in_end;
								}
							}
						}


						read_mouse();
						new_x = mouse_x;
						new_y = mouse_y;

						if((new_x != old_x) ||
							(new_y != old_y))
						{
							plus(old_x,old_y);
							plus(new_x,new_y);
							old_x = new_x;
							old_y = new_y;
						}
					}

					my_pen_press = 0;
					plus(old_x,old_y);
					x_1st = old_x;
					y_1st = old_y;

					goto_row_col(win_y_low + 6,win_x_low + 8);
					printf("Select Second Zoom Box Corner");

					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if(value_char_read
									== 75)
								{
									draw_zoom_box(
									 x_1st,
									 y_1st,
									 old_x,
									 old_y);
									goto_row_col(
									 win_y_low 
									 + 6,
									 win_x_low
									 + 8);
									printf("                                 ");
									goto
									 first_corner;
								}
								else if(value_char_read
									== 80)
								{
									draw_zoom_box(
									 x_1st,
									 y_1st,
									 old_x,
									 old_y);
									goto
									 zoom_in_end;
								}
							}
						}

						read_mouse();
						new_x = mouse_x;
						new_y = mouse_y;

						if((new_x != old_x) ||
							(new_y != old_y))
						{
							draw_zoom_box(x_1st,
								y_1st,old_x,old_y);
							draw_zoom_box(x_1st,
								y_1st,new_x,new_y);
							old_x = new_x;
							old_y = new_y;
						}
					}

					my_pen_press = 0;
					x_2nd = old_x;
					y_2nd = old_y;

					if(abs(x_2nd - x_1st) >
					    abs(y_2nd - y_1st))
						new_zoom = 1024.0/(abs(x_2nd
							- x_1st) + 17)*
								old_zoom;
					else
						new_zoom = 1024.0/(abs(y_2nd
							- y_1st) + 17)*
								old_zoom;

					if(new_zoom > 60.0)
						new_zoom = 60.0;

					x_zooms_pan += (int)((512 - center_x)
							/old_zoom);
					y_zooms_pan += (int)((-384 + center_y)
							/old_zoom);

					pan_x = (int)(x_zooms_pan*4*new_zoom +
						x_pan*new_zoom);
					pan_y = (int)(y_zooms_pan*4*new_zoom +
						y_pan*new_zoom);
				
					old_zoom = new_zoom;


					clear_screen();
					alt_u_redisplay = FALSE;

					if(crosshair_flag == TRUE)
					{
						horizontal_line();
						vertical_line();
					}

					redisplay();
			
				zoom_in_end:
					break;
				}

				case 0x3d : /*zoom out*/
				{
					old_x = old_y = new_x = new_y = 0;

					clear_win(REVERSE_VIDEO);
					draw_box();

					goto_row_col(win_y_low + 2,win_x_low + 5);
					printf("Zoom Out");

					if(new_zoom <= 1.0)
					{
						goto_row_col(win_y_low + 4,
							win_x_low + 8);
						printf("The Current Zoom Factor Is Too Small");
						goto_row_col(win_y_low + 6,
							win_x_low + 8);
						printf("Press Any Key To Continue");
						getch();
						break;
					}

					goto_row_col(win_y_low + 4,win_x_low + 8);
					printf("Select Value By Moving Up And Down");
					goto_row_col(win_y_low + 6,win_x_low + 8);
					printf("Zoom Down Factor = 1.0       ");

					zoom_down_box(0);
					mouse_y = 0;

					my_pen_press = 0;

					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if((value_char_read
									== 75) ||
								   (value_char_read
									== 80))
								{  /*get out*/
									zoom_down_box(
									 old_y);
									goto
									 zoom_out_end;
								}
							}
						}

						read_mouse();
						new_y = mouse_y;

						if(new_y != old_y)
						{
							zoom_down_box(old_y);
							zoom_down_box(new_y);

							goto_row_col(win_y_low + 6,
								win_x_low + 8);
							printf("Zoom Down Factor = %.4lf      ",
								20.0/(new_y + 
								20));

							old_y = new_y;
						}
					}

					my_pen_press = 0;

					new_zoom = 20.0/(old_y + 20)*old_zoom;

					if(new_zoom < 1.0)
						new_zoom = 1.0;

					pan_x = (int)(x_zooms_pan*4*new_zoom +
						x_pan*new_zoom);
					pan_y = (int)(y_zooms_pan*4*new_zoom +
						y_pan*new_zoom);


					old_zoom = new_zoom;


					clear_screen();
					alt_u_redisplay = FALSE;

					if(crosshair_flag == TRUE)
					{
						horizontal_line();
						vertical_line();
					}

					redisplay();

				zoom_out_end:
					break;
				}


				case 0x3b : /*undo all zooms*/
				{
					if((old_zoom != 1.0) || (new_zoom != 1.0)
					   || (pan_x != 0) || (pan_y != 0))
						undo_zooms();

					break;
				}

				case 0x50 :
				case 0x4b : /*return to unigraphics*/
				{
					restore_win();
					turn_on_curser();
					goto_row_col(row_curser,col_curser);
					return;
				}
				
			}
		}

	}
}


