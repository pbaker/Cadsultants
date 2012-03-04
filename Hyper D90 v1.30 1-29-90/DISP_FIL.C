#include "stdio.h"
#include "display.h"
#include "generics.h"
#include "time.h"
#include "dos.h"
#include "math.h"
#include "expanded.h"
#include "c:\emulate\clipper\pw_lib.h"


#undef TRUE
#undef FALSE


#define TRUE  't'
#define FALSE  'f'
#define X_MAX 4096
#define Y_MAX 3168
#define X_MIN 0
#define Y_MIN 0
#define DL_REAL_END 0x1000
#define MAX_Y_OVER_2 1584
#define HARDWARE 0
#define SOFTWARE 1


extern int far errno;
extern char far my_text_buffer[];


extern char far diag_string[];
extern void far error_logger(char *);





/* choose.c menu functions */
extern void far control_window(void);
extern int far choose_menu(char far *,char far *[],int,int);
extern int far information_menu(char far *,char far *[],int,char,char);
extern void far error_message(int,char far *[],int);





/*bios calls*/
extern void far blinking_string();
extern void far normal_string(char *,int,int);
extern void far goto_row_col();
extern void far read_curser_pos();
extern void far turn_off_curser();
extern void far turn_on_curser();
extern void far clear_win();
extern void far save_win();
extern void far restore_win();
extern void far draw_box();
extern void far make_win(int,int,int,int);
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

/*handle curser speed ratio based on size of the viewing Window*/
extern float far window_speed();
extern float far window_speed_factor;

extern OPCODE far my_display[];
extern OPCODE far *current_pointer_to_dl;
extern char far added_dlint;
extern int far row_curser;
extern int far col_curser;
extern struct a_word far an_element;
extern char far write_display_error;
extern char far drive_letter;
extern char far quit_redisplay;
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
extern int far crosshair_column;
extern int far crosshair_row;
extern int far which_page;
extern int far io_masks[];

extern union REGS far my_input_regs;
extern union REGS far my_output_regs;


extern float far indicator_radius;
extern int far radius_add_on;


float far old_zoom = 1.0,far new_zoom = 1.0;
float far zoom_down_factor = 1.0;
float far zoom_up_factor = 1.0,old_zoom_up_factor = 1.0;
int far pan_x = 2048,far pan_y = MAX_Y_OVER_2;
int far mouse_x = 0,far mouse_y = 0;
char far zoom_pan_enable = FALSE;
int far x_zooms_pan = 2048,far y_zooms_pan = MAX_Y_OVER_2;
int far x_pan = 2048,far y_pan = MAX_Y_OVER_2;
int far old_x_zooms_pan = 2048,far old_y_zooms_pan = MAX_Y_OVER_2;
int far window_x_center = 2048,far window_y_center = MAX_Y_OVER_2;
char far alt_u_redisplay;
int far zoom_in_scale;
int far center_x,far center_y;


char far clear_delete[66],far string_out[80];
char far doing_redisplay = FALSE;
char far insert_char_str[9];
char far int_happened = 'f';
int far zoom_x_center = 2048,far zoom_y_center = MAX_Y_OVER_2;
char far all_used_up = FALSE;
char far reached_end_dl = FALSE;
int far int_reason;
int far last_logical_page;
int far x_zoom_up_center,y_zoom_up_center;


char far ems = SOFTWARE;
short far ems_buffer[8199];


/*set by my_window calls in this file*/
int far max_x_window = X_MAX;
int far min_x_window = X_MIN;
int far max_y_window = Y_MAX;
int far min_y_window = Y_MIN;
char far zooming = FALSE;
char far panning = FALSE;




int far the_user_font = 2;

short far indicator_dl[25];





int far minimum(a,b)
int a,b;
{
	if(a < b)
		return(a);
	else
		return(b);
}




void far my_window(x1,x2,y1,y2)
int x1,x2,y1,y2;
{
	min_x_window = x1;
	max_x_window = x2;
	min_y_window = Y_MAX - y2;
	max_y_window = Y_MAX - y1;

	window(x1,x2,y1,y2);
}





void far empty_mouse_buffer()
{
int dx,dy;

        dx = dy = 9;

        while((dx != 0) || (dy != 0))
        {
        	my_input_regs.x.ax = 11;     /*raw mouse movements*/
	        int86(0x33,&my_input_regs,&my_output_regs);

	        dx = my_output_regs.x.cx;
	        dy = my_output_regs.x.dx;
        }
}






void far read_mouse()
{
int dx,dy;


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
	

	mouse_x += dx;
	mouse_y += dy;

	if((zooming == FALSE) && (panning == FALSE))
	{
		if(mouse_x > max_x_window)
			mouse_x = max_x_window;
		else if(mouse_x < min_x_window)
			mouse_x = min_x_window;

		if(mouse_y > max_y_window)
			mouse_y = max_y_window;
		else if(mouse_y < min_y_window)
			mouse_y = min_y_window;
	}

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
int size_dl_buffer,i;
short far *address_of_page_frame;
long phy_address_of_insert,phy_address_of_page_frame;



	if(all_used_up == TRUE)
		return;



 /*if current logical page is full enough then goto next logical page*/

	address_of_page_frame = (short far *)physical_pages_pointer;

	phy_address_of_page_frame = ((long)FP_SEG(address_of_page_frame) << 4)
		 + (long)FP_OFF(address_of_page_frame);
	phy_address_of_insert = ((long)FP_SEG(current_pointer_to_dl) << 4) +
		(long)FP_OFF(current_pointer_to_dl);


	if(phy_address_of_insert - phy_address_of_page_frame > 16360L)
 /*goto next logical EMS page*/
	{
	/*put on a dlend()*/
		pw_dl_init(current_pointer_to_dl,0,NULL,0);
		dlend();
		pw_dl_init(NULL,0,NULL,0);

/*move to next logical page*/
		current_logical_page++;
		if(current_logical_page > num_unalloc_pages - 1)
		{
			sprintf(diag_string,"******ERROR***** YOU'VE USED UP ALL OF EXPANDED MEMORY\N");
			error_logger(diag_string);
			sprintf(diag_string,"NOTHING MORE WILL BE STORED\N");
			error_logger(diag_string);

			all_used_up = TRUE;
			return;
		}

		map_handle_page(current_logical_page,current_physical_page,
			handle);
	
		current_pointer_to_dl = (short far *)physical_pages_pointer;
	}


	added_dlint = FALSE;

	switch(an_element.op_code)
	{
		case 1 : /*draw a line*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			vect2(an_element.arg1,an_element.arg2,an_element.arg3,
				an_element.arg4);
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 2 : /*dash pattern*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			linpat(an_element.arg1 & 0x0f);
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 3 : /*set color*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			color(((an_element.arg1 & 0x0f) << 4) |
				(an_element.arg1 & 0x0f));
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 4 : /*insert a charactor string*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			move2(an_element.arg1,an_element.arg2);
			gtext(&my_text_buffer[0]);
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 5 : /*plot a point*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			move2(an_element.arg1,an_element.arg2);
			point2();
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 6 : /*move*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			move2(an_element.arg1,an_element.arg2);
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 7 : /*display indicator*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			move2(an_element.arg1,an_element.arg2);
			call(&indicator_dl[0]);
			rdlbptr(&current_pointer_to_dl);
			pw_dl_init(NULL,0,NULL,0);

			break;
		}

		case 44 : /*ignore*/
			break;

		default :
		{
			sprintf(diag_string,
				"INVALID op code in insert_word() = %d\n",
					an_element.op_code);
			error_logger(diag_string);
		}
	}



}













void far redisplay()
{
int save_my_x,save_my_y,y,save_row_curser,save_col_curser,row,col;
long temp_y,remainder,start_time,end_time,dummy;
int redisplay_page;
int far *the_reason;

	FP_SEG(the_reason) = 0x40;
	FP_OFF(the_reason) = 0x0004;

	zoom_pan_enable = TRUE;

	save_my_x = my_x;
	save_my_y = my_y;
	save_row_curser = row_curser;
	save_col_curser = col_curser;
	my_x = 0;
	my_y = 0;



	pw_dl_init(&indicator_dl[0],0,NULL,0);
	circle((int)(indicator_radius/new_zoom + 0.5) + radius_add_on);
	ret();
	pw_dl_init(NULL,0,NULL,0);


	read_curser_pos();
	sprintf(string_out,"Screen Update       ");
	blinking_string(string_out,24,15);

	start_time = time(&dummy);
	if(time_redisplay == TRUE)
	{
		blinking_string("TIMER              ",24,50);
	}



	doing_redisplay = TRUE;

	if(added_dlint == FALSE)  /*add dlint() to mark the real end (at this time)*/
	{
		pw_dl_init(current_pointer_to_dl,0,NULL,0);
		dlint(DL_REAL_END);
		pw_dl_init(NULL,0,NULL,0);

		added_dlint = TRUE;
	}

	which_page = !which_page;
	wrmask(io_masks[which_page]);


	int_happened = FALSE;
	redisplay_page = 0;
	int_reason = -1;
	while(redisplay_page <= current_logical_page)
	{
		map_handle_page(redisplay_page,current_physical_page,handle);

		if(ems == HARDWARE)
			dlrun((short far *)physical_pages_pointer);
		else
		{
			memcpy(&ems_buffer[0],physical_pages_pointer,16380);
			dlrun(&ems_buffer[0]);
		}


		pw_dl_wait();

		int_happened = FALSE;
		redisplay_page++;
	}

	map_handle_page(current_logical_page,current_physical_page,handle);

	rdmask(io_masks[which_page]);


	doing_redisplay = FALSE;

	normal_string("                                                   ",24,4);

	end_time = time(&dummy);

	if(time_redisplay == TRUE)
	{
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
	y = Y_MAX - y;

	my_dash_enable(0);
	my_color(7);
	draw_mode(3);
	my_line_xy(x - 48,y,x + 48,y);
	my_line_xy(x,y - 48,x,y + 48);
	draw_mode(0);
	my_color(current_color);
	my_dash_enable(line_style);
}




#ifdef yiworirorijekfok

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

#endif




void far zoom_down_box(y_mouse)
int y_mouse;
{
int x1,y1,x2,y2;


        if(min_y_window >= 0)
        	zoom_down_factor = 20.0/((y_mouse - min_y_window) + 20); 
        else
                zoom_down_factor = 1.0;


	x1 = (int)(-2044/new_zoom*zoom_down_factor) + window_x_center + 4;
	y1 = (int)(-MAX_Y_OVER_2/new_zoom*zoom_down_factor) + 4 +
                  window_y_center;
	x2 = (int)(2044*zoom_down_factor/new_zoom) + window_x_center - 4;
	y2 = (int)(MAX_Y_OVER_2/new_zoom*zoom_down_factor) - 4 + 
                  window_y_center;

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







void far zoom_up_box(y_mouse)
int y_mouse;
{
int x1,y1,x2,y2,half_box_size,temp;
char off_x_flag;
int mouse_stop;


/* stop mouse variable movements flag (to be returned & acted on)*/
	mouse_stop = FALSE;


	off_x_flag = 'f';

	half_box_size = y_mouse - (Y_MAX - y_zoom_up_center);

	if(half_box_size < 20)
		half_box_size = 20;


	temp = minimum(abs(min_y_window - (Y_MAX - y_zoom_up_center)),
		abs(max_y_window - (Y_MAX - y_zoom_up_center)));

	if(half_box_size > temp)   /*off the screen in the y direction*/
	{
		half_box_size = temp;
		mouse_y = half_box_size + Y_MAX - y_zoom_up_center;
	}



	if(x_zoom_up_center - 4*half_box_size/3 < min_x_window)  /*off the left side*/
	{
		half_box_size = 3*(x_zoom_up_center - min_x_window)/4;
		mouse_y = half_box_size + Y_MAX - y_zoom_up_center;
		off_x_flag = 't';
	}


	if(x_zoom_up_center + 4*half_box_size/3 > max_x_window)  /*off the right side*/
	{
		half_box_size = 3*(max_x_window - x_zoom_up_center)/4;
		mouse_y = half_box_size + Y_MAX - y_zoom_up_center;
		off_x_flag = 't';
	}



	if(half_box_size <= 0)
		half_box_size = 10;


	x1 = x_zoom_up_center - half_box_size*4/3;
	y1 = y_zoom_up_center - half_box_size;
	x2 = x_zoom_up_center + half_box_size*4/3;
	y2 = y_zoom_up_center + half_box_size;




	zoom_up_factor = (max_y_window - min_y_window)/2.06/half_box_size;



	if(zoom_up_factor < 0.99)
		zoom_up_factor = 1.0;


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
int x_box_size,y_box_size;

        x_box_size = (int)((max_x_window - min_x_window)/2.1);
        y_box_size = (int)((max_y_window - min_y_window)/2.1);

	my_dash_enable(0);
	my_color(7);
	draw_mode(3);

	my_line_xy(x - x_box_size,y - y_box_size,x + x_box_size,y - y_box_size);
	my_line_xy(x - x_box_size,y - y_box_size,x - x_box_size,y + y_box_size);
	my_line_xy(x - x_box_size,y + y_box_size,x + x_box_size,y + y_box_size);
	my_line_xy(x + x_box_size,y + y_box_size,x + x_box_size,y - y_box_size);

	draw_mode(0);
	my_color(current_color);
	my_dash_enable(line_style);
}






void far undo_zooms()
{
	old_zoom = new_zoom = 1.0;
        zoom_down_factor = 1.0;
	zoom_up_factor = 1.0;
	old_zoom_up_factor = 1.0;
	window_x_center = 2048;
	window_y_center = MAX_Y_OVER_2;
	pan_x = 0;
	my_window(0,4096,0,Y_MAX);

	alt_u_redisplay = FALSE;

	redisplay();
}





void far manual_zoom_pan()
{
int selection,new_x,new_y,old_x,old_y;
int save_current_color;

        /*remove host crosshair if displayed*/
        if(crosshair_flag == TRUE)
        {
                save_current_color = current_color;
                horizontal_line();
                vertical_line();
        }

        turn_off_curser();
        read_curser_pos();
        save_win();

        while(9)
        {
		make_win(10,3,60,17);
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


		control_window();
		make_win(10,3,60,17);


                selection = getch();

                if(selection != 0)
                        continue;
                else
                {
                        selection = getch();

                        switch(selection)
                        {
                                case 0x3b : /*original scale*/
                                {
                          original_scale:

                                        if((new_zoom != 1.0) ||
                                                (pan_x != 0))
                                                        undo_zooms();

                                        zooming = FALSE;
                                        panning = FALSE;

					normal_string("                                                         ",
						24,10);

                                        break;
                                }


                                case 0x3c : /*zoom in*/
                                {
                        zoom_in_again:
					normal_string("                                                         ",
						24,10);


					make_win(10,3,60,17);
                                        clear_win(REVERSE_VIDEO);
                                        draw_box();

                                        goto_row_col(win_y_low + 1,win_x_low + 5);
                                        printf("Zoom In");

                                        goto_row_col(win_y_low + 3,win_x_low + 8);
                                        printf("Select Zoom Center");

					control_window();
					make_win(10,3,60,17);


                                        mouse_x = old_x = new_x = window_x_center;
                                        mouse_y = old_y = new_y = Y_MAX -
                                                window_y_center;

                                        empty_mouse_buffer();

                                        plus(old_x,old_y);
                                        my_pen_press = 0;

                                        while(my_pen_press == 0)
                                        {
                                                if(keyboard_status() != -1)
                                                {
                                                        read_keyboard();

                                                        if((value_char_read ==
                                                                75) ||
							   (value_char_read ==
								77))
                                                        {  /*reject*/
                                                                plus(old_x,
                                                                        old_y);
                                                                goto zoom_in_end;
                                                        }
                                                        else
                                                        if(value_char_read == 
                                                                80)
                                                        {   /*terminate*/
                                                                plus(old_x,
                                                                        old_y);
                                                                goto exit_manual;
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

                                        }   /*end read mouse while*/


                                        my_pen_press = 0;
                                        plus(old_x,old_y);
                                        

				/*save zoom up center*/
					x_zoom_up_center = new_x;
					y_zoom_up_center = Y_MAX - new_y;

					mouse_y = old_y = new_y = 10000;

					zoom_up_box(mouse_y);

					if(zoom_up_factor > 100.0)
					{
						putchar(7);
						normal_string("Invalid Zoom In Center",
							24,10);

						zoom_up_box(mouse_y);

						goto zoom_in_again;
					}
					else
					{
						window_x_center = x_zoom_up_center;
						window_y_center = y_zoom_up_center;

						normal_string("                                ",
							24,10);
					}


                                        goto_row_col(win_y_low + 4,win_x_low + 8);
                                        printf("Up To Expand / Down To Reduce");

		                        goto_row_col(win_y_low + 6,win_x_low +
						 8);

					if(zoom_up_factor*old_zoom > 30.0)
						printf("Zoom Factor = 30.0       ");
					else if(zoom_up_factor*old_zoom < 1.0)
						printf("Zoom Factor = 1.0        ");
					else
						printf("Zoom Factor = %8.2f",
							zoom_up_factor*old_zoom);



					goto_row_col(win_y_low + 8,win_x_low + 10);
					printf("F1 Original Scale");
					goto_row_col(win_y_low + 9,win_x_low + 10);
					printf("F2>Zoom In");
					goto_row_col(win_y_low + 10,win_x_low + 10);
					printf("F3 Zoom Out");
					goto_row_col(win_y_low + 11,win_x_low + 10);
					printf("F4 Pan");


                                        while(my_pen_press == 0)
                                        {
                                                if(keyboard_status() != -1)
                                                {
                                                        read_keyboard();

                                                        if(type_char_read == 0) 
                                                        {
                                                                if((value_char_read
                                                                        == 75) 
									||
								 (value_char_read
									== 77))
                                                                {   /*reject*/
                                                                        zoom_up_box(
									  mouse_y);


                                                                        goto
                                                                         zoom_in_again;
                                                                }
                                                                else
                                                                if(value_char_read
                                                                        == 80)
                                                                {  /*terminate*/
									zoom_up_box(
									   mouse_y);

                                                                        goto
                                                                         zoom_in_end;
                                                                }
								else
								if(value_char_read
								       == 0x3b)
								{
                                                                        zoom_up_box(
									  mouse_y);

				                                        old_zoom 
									 = 
									  new_zoom;
                                                                        goto
									 original_scale;

								}
								else
								if(value_char_read
								       == 0x3d)
								{
                                                                        zoom_up_box(
									  mouse_y);


				                                        old_zoom
									 =
									  new_zoom;
                                                                        goto
									 zoom_out_again;
								}
								else
								if(value_char_read
								       == 0x3e)
								{
                                                                        zoom_up_box(
									  mouse_y);

				                                        old_zoom
									 =
									  new_zoom;

                                                                        goto
									 pan_again;
								}


                                                        }  /*extended code*/

                                                }  /*keyboard pressed*/


                                                read_mouse();

                                                new_x = mouse_x;
                                                new_y = mouse_y;

                                                if((new_x != old_x) ||
                                                        (new_y != old_y))
                                                {
							zoom_up_box(old_y);
							zoom_up_box(new_y);

                                                        old_x = new_x;
                                                        old_y = new_y;

		                                        goto_row_col(win_y_low
							    + 6,win_x_low + 8);

					if(zoom_up_factor*old_zoom > 30.0)
						printf("Zoom Factor = 30.0       ");
					else if(zoom_up_factor*old_zoom < 1.0)
						printf("Zoom Factor = 1.0        ");
					else
						printf("Zoom Factor = %8.2f",
							zoom_up_factor*old_zoom);

                                                }

                                        }    /*end read mouse while*/


                                        my_pen_press = 0;



                                        new_zoom = zoom_up_factor*old_zoom;


					if(new_zoom < 1.0)
						new_zoom = 1.0;


					if(new_zoom > 30.0)
						new_zoom = 30.0;

					
                                        my_window(x_zoom_up_center - (int)(
							2048/new_zoom),
						  x_zoom_up_center + (int)(
							2048/new_zoom),
						  y_zoom_up_center - (int)(
							MAX_Y_OVER_2/new_zoom),
						  y_zoom_up_center + (int)(
							MAX_Y_OVER_2/new_zoom));

                                        redisplay();


                                        old_zoom = new_zoom;



                                zoom_in_end:

                                        old_zoom = new_zoom;

                                        break;

                                }   /*end case 0x3c*/


                                case 0x3d : /*zoom out*/
                                {
                        zoom_out_again:
				
					if(new_zoom > 1.0)
						normal_string("                                                         ",
							24,10);
					else
					{
						putchar(7);

						normal_string("Cannot Zoom Out - No Zoom In Effect",
							24,10);

						break;
					}



                                        mouse_y = old_y = new_y = min_y_window;
                                
                                        empty_mouse_buffer();

					make_win(10,3,60,17);
                                        clear_win(REVERSE_VIDEO);
                                        draw_box();

                                        goto_row_col(win_y_low + 1,win_x_low +
                                             5);
                                        printf("Zoom Out");
                                        goto_row_col(win_y_low + 3,win_x_low +
                                             8);
                                        printf("Up To Expand / Down To Reduce");
                                        goto_row_col(win_y_low + 5,win_x_low +
                                             8);
                                        printf("Zoom Factor = %8.3f   ",
						old_zoom);


					goto_row_col(win_y_low + 7,win_x_low +
						10);
					printf("F1 Original Scale");
					goto_row_col(win_y_low + 8,win_x_low +
						10);
					printf("F2 Zoom In");
					goto_row_col(win_y_low + 9,win_x_low +
						10);
					printf("F3>Zoom Out");
					goto_row_col(win_y_low + 10,win_x_low 
						+ 10);
					printf("F4 Pan");


					control_window();
					make_win(10,3,60,17);

                                        my_pen_press = 0;
                                        zoom_down_box(old_y);

                                        while(my_pen_press == 0)
                                        {
                                                if(keyboard_status() != -1)
                                                {
                                                        read_keyboard();

                                                        if(type_char_read == 0)
                                                        {
                                                                if((value_char_read
                                                                        == 75)
									||
								  (value_char_read
									== 77))
                                                                {
                                                                        zoom_down_box(
                                                                         old_y);
                                                                        goto
                                                                         zoom_out_end;
                                                                }
                                                                else
                                                                if(value_char_read
                                                                        == 80)
                                                                {
                                                                        zoom_down_box(
                                                                         old_y);
                                                                        goto
                                                                         exit_manual;
                                                                }
                                                                else
                                                                if(value_char_read
                                                                       == 0x3b)
                                                                {
                                                                        zoom_down_box(
                                                                         old_y);
				                                        old_zoom
									 =
									  new_zoom;
                                                                        goto
                                                                         original_scale;
                                                                }
                                                                else
                                                                if(value_char_read
                                                                       == 0x3c)
                                                                {
                                                                        zoom_down_box(
                                                                         old_y);
				                                        old_zoom
									 =
									  new_zoom;
                                                                        goto
                                                                         zoom_in_again;
                                                                }
                                                                else
                                                                if(value_char_read
                                                                       == 0x3e)
                                                                {
                                                                        zoom_down_box(
                                                                         old_y);
				                                        old_zoom
									 =
									  new_zoom;
                                                                        goto
                                                                         pan_again;
                                                                }

                                                        }  /*extended code*/

                                                }    /*keyboard pressed*/

                                                read_mouse();

                                                new_y = mouse_y;

                                                if(new_y != old_y)
                                                {
                                                        zoom_down_box(old_y);
                                                        zoom_down_box(new_y);

                                                        goto_row_col(win_y_low
                                                                + 5,win_x_low
                                                                        + 8);

						if(zoom_down_factor*old_zoom >
						    30.0)
							printf("Zoom Factor = 30.0       ");
						else if(zoom_down_factor*
						    old_zoom < 1.0)
							printf("Zoom Factor = 1.0        ");
						else
                                                        printf("Zoom Factor = %8.3f   ",
                                                                zoom_down_factor
								*old_zoom);

                                                        old_y = new_y;
                                                }
                                        }


                                        zoom_down_box(old_y);

                                        my_pen_press = 0;

                                        new_zoom = zoom_down_factor*old_zoom;


					if(new_zoom > 30.0)
						new_zoom = 30.0;


					if(new_zoom <= 1.0)
					{
					    /*undo zoom/pan*/
					        zoom_down_factor = 1.0;
						zoom_up_factor = 1.0;
						old_zoom_up_factor = 1.0;
						window_x_center = 2048;
						window_y_center = MAX_Y_OVER_2;
						pan_x = 0;
						my_window(0,4096,0,Y_MAX);
						new_zoom = 1.0;
					}


					if(old_zoom > 1.0)
					{
                                        	my_window(window_x_center 
							- (int)(
                                                	2048/new_zoom),
							window_x_center
                                                	+ (int)(2048/new_zoom),
                                                	window_y_center 
							- (int)(
                                                	MAX_Y_OVER_2/new_zoom),
                                                	window_y_center 
							+ (int)(
                                                	MAX_Y_OVER_2/new_zoom));

	                                        redisplay();
					}


                                        old_zoom = new_zoom;


                                 zoom_out_end:

                                        old_zoom = new_zoom;

                                        break;
                                }


                                case 0x3e : /*pan*/
                                {
                        pan_again:
					make_win(10,3,60,17);
                                        clear_win(REVERSE_VIDEO);
                                        draw_box();


					if(new_zoom <= 1.01)
					{
						putchar(7);
						normal_string(
							"Cannot Pan - No Zoom In Effect                  "
								,24,10);

						break;
					}
					else
						normal_string(
                       					"                                                        "
								,24,10);



                                        empty_mouse_buffer();

                                        mouse_x = old_x = new_x = window_x_center;
                                        mouse_y = old_y = new_y = Y_MAX - 
						window_y_center;

                                        goto_row_col(win_y_low + 1,win_x_low + 8);
                                        printf("Pan");
                                        goto_row_col(win_y_low + 3,win_x_low + 8);
                                        printf("Move Input Device To Pan");

					goto_row_col(win_y_low + 5,win_x_low +
						10);
					printf("F1 Original Scale");
					goto_row_col(win_y_low + 6,win_x_low +
						10);
					printf("F2 Zoom In");
					goto_row_col(win_y_low + 7,win_x_low +
						10);
					printf("F3 Zoom Out");
					goto_row_col(win_y_low + 8,win_x_low +
						10);
					printf("F4>Pan");


					control_window();
					make_win(10,3,60,17);


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
                                                                        == 75)
									||
								  (value_char_read
									== 77))
                                                                {
                                                                        pan_box(
                                                                         old_x,
                                                                         old_y);

                                                                        goto
                                                                           pan_end;
                                                                }
                                                                else
                                                                if(value_char_read
                                                                        == 80)
                                                                {
                                                                        pan_box(
                                                                         old_x,
                                                                         old_y);

                                                                        goto
                                                                         exit_manual;
                                                                }
								else
                                                                if(value_char_read
                                                                       == 0x3b)
                                                                {
                                                                        pan_box(
                                                                         old_x,
                                                                         old_y);

                                                                        goto
									 original_scale;
								}
								else
                                                                if(value_char_read
                                                                       == 0x3c)
                                                                {
                                                                        pan_box(
                                                                         old_x,
                                                                         old_y);

                                                                        goto
									 zoom_in_again;
								}
								else
                                                                if(value_char_read
                                                                       == 0x3d)
                                                                {
                                                                        pan_box(
                                                                         old_x,
                                                                         old_y);

                                                                        goto
									 zoom_out_again;
								}
                                                                
                                                        }  /*extended code*/

                                                }  /*keyboard pressed*/

                                                read_mouse();

                                                new_x = mouse_x;
                                                new_y = Y_MAX - mouse_y;

                                                if((new_x != old_x) ||
                                                        (new_y != old_y))
                                                {
                                                        pan_box(old_x,old_y);
                                                            
                                                        pan_box(new_x,new_y);

                                                        old_x = new_x;
                                                        old_y = new_y;
                                                }
                                                
                                        }

                                        pan_box(old_x,old_y);
                                                                        

                                        window_x_center = old_x;
                                        window_y_center = old_y;

                                        pan_x = 1;

                                        
                                        my_window(window_x_center - (int)(
                                                2048/new_zoom),window_x_center
                                                + (int)(2048/new_zoom),
                                                window_y_center - (int)(
                                                MAX_Y_OVER_2/new_zoom),
                                                window_y_center + (int)(
                                                MAX_Y_OVER_2/new_zoom));

                                        redisplay();



                               
                                pan_end:

                                        break;
                                }


                                case 75 :
				case 77 : 
                                case 80 : /*return to unigraphics*/
                                {
                        exit_manual:
					normal_string("                                                         ",
						24,10);


                                        if(crosshair_flag == TRUE)
                                        { /*bring back host crosshair*/
                                                crosshair_column = 
                                                       window_x_center;
                                                crosshair_row = Y_MAX -
                                                        window_y_center;

                                                current_color =
                                                        save_current_color;
                                                my_color(current_color);

                                                horizontal_line();
                                                vertical_line();
                                        }

                                        restore_win();
                                        turn_on_curser();
                                        goto_row_col(row_curser,col_curser);

                                        return;
                                }

                        }  /*end main switch*/

                }  /*its an extended code typed*/

        }   /*end main while*/

}







/*Dynamic zoom/pan routine*/
void far zoom_pan()
{
int selection,new_x,new_y,old_x,old_y,x_1st,y_1st,x_2nd,y_2nd;
int save_current_color;
char should_redisplay;


	should_redisplay = TRUE;


	/*remove host crosshair if displayed*/
	if(crosshair_flag == TRUE)
	{
		save_current_color = current_color;
		horizontal_line();
		vertical_line();
	}

	turn_off_curser();
	read_curser_pos();
	save_win();

	while(9)
	{
		make_win(10,3,60,17);
		clear_win(REVERSE_VIDEO);
		draw_box();	


		goto_row_col(win_y_low + 2,win_x_low + 5);
		printf("Zoom/Pan Control");
		goto_row_col(win_y_low + 4,win_x_low + 8);
		printf("F1 Original Scale");
		goto_row_col(win_y_low + 5,win_x_low + 8);
		printf("F2 Zoom ");
		goto_row_col(win_y_low + 6,win_x_low + 8);
		printf("F3 Pan");

		control_window();
		make_win(10,3,60,17);


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
				case 0x3d : /*pan*/
				{
			do_pan:
					make_win(10,3,60,17);
					clear_win(REVERSE_VIDEO);
					draw_box();

					goto_row_col(win_y_low + 2,win_x_low + 5);
					printf("Pan");


					if(new_zoom <= 1.00)
					{
						putchar(7);
						normal_string(
							"Cannot Pan - No Zoom In Effect                  "
								,24,10);
						goto pan_end;

						break;
					}
					else
						normal_string(
                       					"                                    "
								,24,10);



					goto_row_col(win_y_low + 4,win_x_low + 8);
					printf("Move Input Device To Pan");

					goto_row_col(win_y_low + 6,
						win_x_low + 8);
					printf("F1 Original Scale");
					goto_row_col(win_y_low + 7,
						win_x_low + 8);
					printf("F2 Zoom ");
					goto_row_col(win_y_low + 8,
					win_x_low + 8);
					printf("F3>Pan");

					control_window();
					make_win(10,3,60,17);


					mouse_x = old_x = new_x = window_x_center;
					mouse_y = old_y = new_y = window_y_center;
					my_pen_press = 0;
					panning = TRUE;

					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if((value_char_read
									== 75)
									||
								 (value_char_read
									== 77))
								{
									goto
									 pan_end;
								}
								else if(
								   value_char_read
									== 80)
								{  /*get out*/
									goto
									 my_exit_zoom_pan;
								}
								else if(
								  value_char_read
									== 0x3b)
								{
									panning
								         = FALSE;
									goto
									 original_scale;
								}
								else if(
									value_char_read
									== 0x3c)
								{
									panning
									 = FALSE;
									goto
									 do_zoom;
								}
							}
						}

						read_mouse();
						new_x = mouse_x;
						new_y = mouse_y;

						if((new_x != old_x) ||
							(new_y != old_y))
						{

							alt_u_redisplay = FALSE;

							window_x_center =
								 new_x;
							window_y_center += 
								(old_y - new_y);


							my_window(window_x_center
							   - (int)(2048/new_zoom),
							   window_x_center +
							   (int)(2048/new_zoom),
							   window_y_center -
							   (int)(MAX_Y_OVER_2
								/new_zoom),
							   window_y_center +
							   (int)(MAX_Y_OVER_2
								/new_zoom));

							redisplay();
							pan_x = 1;

							old_x = new_x;
							old_y = new_y;
						}
					}



				pan_end:
					panning = FALSE;

					break;
				}



				case 0x3c : /*zoom*/
				{
			do_zoom:
					normal_string("                                                         ",
						24,10);


					make_win(10,3,60,17);
					clear_win(REVERSE_VIDEO);
					draw_box();

					goto_row_col(win_y_low + 2,win_x_low + 5);
					printf("Zoom In/Out");

					mouse_x = old_x = new_x = window_x_center;
					mouse_y = old_y = new_y = Y_MAX -
						 window_y_center;

					goto_row_col(win_y_low + 3,win_x_low + 5);
					printf("Select Zoom Center With Input Device");


					control_window();
					make_win(10,3,60,17);


					plus(old_x,old_y);
					my_pen_press = 0;

					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if((value_char_read
									== 75)
									||
								  (value_char_read
									== 77))
								{
									plus(
									 old_x,
									 old_y);
									goto
									 zoom_in_end;
								}
								else
								if(value_char_read
									== 80)
								{  /*get out*/
									plus(
									 old_x,
									 old_y);
									goto
									 my_exit_zoom_pan;
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

					window_x_center = new_x;
					window_y_center = Y_MAX - new_y;

					pan_x = 1;  /*so original scale*/
							/*will work*/


					my_window(window_x_center  -
						 (int)(2048/new_zoom),
						  window_x_center +
						   (int)(2048/new_zoom),
						   window_y_center -
						   (int)(MAX_Y_OVER_2/
							new_zoom),
						   window_y_center +
						   (int)(MAX_Y_OVER_2/
							new_zoom));

					redisplay();

					goto_row_col(win_y_low + 4,win_x_low + 5);
					printf("Up To Expand / Down To Reduce");

					goto_row_col(win_y_low + 8,
						win_x_low + 8);
					printf("F1 Original Scale");
					goto_row_col(win_y_low + 9,
						win_x_low + 8);
					printf("F2>Zoom ");
					goto_row_col(win_y_low + 10,
					win_x_low + 8);
					printf("F3 Pan");


					new_x = mouse_x = old_x = 2048;
					new_y = mouse_y = old_y = MAX_Y_OVER_2;
					zooming = TRUE;
	
					my_pen_press = 0;
					while(my_pen_press == 0)
					{
						if(keyboard_status() != -1)
						{
							read_keyboard();
							if(type_char_read == 0)
							{
								if((value_char_read
									== 75)
									||
								  (value_char_read
									== 77))
								{
									goto
									 zoom_in_end;
								}
								else if(value_char_read
									== 80)
								{
									old_zoom
									 =
									new_zoom;
									goto
									 my_exit_zoom_pan;
								}
								else if(value_char_read
									== 0x3b)
								{
									zooming
									 = FALSE;
									goto
									 original_scale;
								}
								else if(value_char_read
									== 0x3d)
								{
									zooming
									 = FALSE;
									old_zoom
									 =
									new_zoom;
									goto
									 do_pan;
								}
							}
						}

						read_mouse();
						new_x = mouse_x;
						new_y = mouse_y;

						if((new_x != old_x) ||
							(new_y != old_y))
						{
							if(new_y > MAX_Y_OVER_2)
							       new_zoom = 40.0/
									(new_y
									 - 1542)
									*old_zoom;
							else
								new_zoom = 
									(1622
									- new_y)
									/40.0
									*old_zoom;



							if((new_zoom >= 1.0) &&
							  (new_zoom <= 30.0))
							     should_redisplay =
								TRUE;


							if(new_zoom > 30.0)
							{
								new_zoom = 30.0;

								new_y = old_y =
								mouse_y = (int)
								  (1622.0 - 
								    1200.0/
								     old_zoom);

								if(should_redisplay
								  == TRUE)
								     putchar(7);

								should_redisplay
								     = FALSE;
							}


							if(new_zoom < 1.0)
							{
								new_zoom = 1.0;

								new_y = old_y =
								mouse_y = (int)
								  (40.0*
								    old_zoom +
								      1542.0);

								if(should_redisplay
								  == TRUE)
								     putchar(7);

								should_redisplay
								     = FALSE;
							}



							goto_row_col(win_y_low 
								+ 6,win_x_low
									 + 5);
							printf("Zoom Factor = %8.4f",
								new_zoom);
							 
							
							my_window(window_x_center 
							- (int)(2048/new_zoom),
							  window_x_center +
							   (int)(2048/new_zoom),
							   window_y_center -
							   (int)(MAX_Y_OVER_2/
								new_zoom),
							   window_y_center +
							   (int)(MAX_Y_OVER_2/
								new_zoom));


							if(should_redisplay ==
							   TRUE)
								redisplay();

							old_x = new_x;
							old_y = new_y;
						}
					}


			
				zoom_in_end:
					old_zoom = new_zoom;
					zooming = FALSE;

					break;
				}



				case 0x3b : /*undo all zooms*/
				{
			original_scale:
					if((new_zoom != 1.0) ||
						(pan_x != 0))
						undo_zooms();

					zooming = FALSE;
					panning = FALSE;

					normal_string("                                                         ",
						24,10);


					break;
				}

				case 0x50 :
				case 0x4b : /*return to unigraphics*/
				{
			my_exit_zoom_pan:
					normal_string("                                                         ",
						24,10);


					zooming = panning = FALSE;

					if(crosshair_flag == TRUE)
					{ /*bring back host crosshair*/
						crosshair_column = window_x_center;
						crosshair_row = Y_MAX - 
							window_y_center;
						current_color = 
							save_current_color;
						my_color(current_color);

						horizontal_line();
						vertical_line();
					}


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