#include "display.h"
#include "generics.h"


#define TRUE  't'
#define FALSE  'f'

#define HAVE_DIAGS
#define MAXX  1023
#define MAXY  767



extern void far set_joystick();

extern char far have_display_list;

extern char far crosshair_flag;
extern int far syscolor_index;
extern int far crosshair_column;
extern int far crosshair_row;
extern int far crosshair_xmax;
extern int far crosshair_ymax;
extern char far rgb_config[];
extern int far x;
extern int far y;
extern int far config_data;
extern int far security;
extern char far repaint_mode;
extern char far controller_flag;
extern char far video_index;
extern char far red_gain;
extern char far green_gain;
extern char far blue_gain;
extern int far line_style;





/*externs for display file stuff*/
extern struct a_word far an_element;
extern struct a_word far read_element;
extern char far write_display_error;
extern char far quit_redisplay;
extern void far insert_word(void);
extern void far get_word();
extern void far redisplay();
extern char far black;



unsigned int far user_line_pattern = 0xfccc;
int far current_color = 15;
char far erase_mode_flag = FALSE;







void far set_color(the_color)
int the_color;
{
	current_color = the_color & 0xff;

	if(erase_mode_flag == FALSE)
		my_color(the_color & 0xff);

}




void far select_erase(the_command)
int the_command;
{
	if(the_command != 'y')
	{
		erase_mode_flag = TRUE;
		my_color(0);
		black = TRUE;

		if(have_display_list == TRUE)
		{
			an_element.op_code = 3;
			an_element.arg1 = 0;
			insert_word();
		}

	}
	else
	{
		erase_mode_flag = FALSE;
		my_color(current_color);
		black = FALSE;

		if(have_display_list == TRUE)
		{
			an_element.op_code = 3;
			an_element.arg1 = current_color;
			insert_word();
		}
		
	}
}




void far set_color_regs()
{
int i;

	for(i = 0 ; i <= 45 ; i += 3)
		color_reg(i/3,rgb_config[i] << 2,rgb_config[i + 1] << 2,
				rgb_config[i + 2] << 2);

}





void far init_terminal()
{
	my_dash_pattern(user_line_pattern);
	set_color_regs();
}



void far crossxy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	my_line_xy(x1,y1,x2,y2);
}




void far horizontal_line()
{
	my_color(syscolor_index);
	my_dash_enable(0);

	draw_mode(3);

	crossxy(0,crosshair_row,MAXX - 2,crosshair_row);

	my_color(current_color);

	my_dash_enable(line_style);

	draw_mode(0);
}




void far vertical_line()
{
	my_color(syscolor_index);
	my_dash_enable(0);

	draw_mode(3);

	crossxy(crosshair_column,0,crosshair_column,MAXY - 2);

	my_color(current_color);

	my_dash_enable(line_style);

	draw_mode(0);
}







void far clear_crosshair()
{
	if(crosshair_flag == TRUE)
	{
		set_joystick(0);
		crosshair_flag = FALSE;
		vertical_line();
		horizontal_line();
	}
}







void far set_video_lookup()
{
	color_reg(video_index & 0x0f,red_gain << 2,green_gain << 2,blue_gain << 2);
}



