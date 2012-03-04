#include "display.h"
#include "stdio.h"
#include "math.h"

#define SVWPT_PAGE_1 4
#define TRUE 1
#define FALSE 0
#define INDIC_RADIUS 4


extern char far drive_letter;
extern int far init_baud_rate;
extern char far path_error_log[];
extern int far curser_ratio;
extern char far print_list;
extern int far list_size;
extern char far print_list;
extern int far time_redisplay;
extern char far rotate_mode;



extern void far insert_word();
extern void far insert_black_word();

extern void far release_version();

extern void far restore_int9();

extern char far black;
extern int far my_x;
extern int far my_y;
extern FILE far *out_ptr;
extern struct a_word far an_element;
extern int far num_pages;
extern char far board_string[];
extern float far new_zoom;
extern int far pan_x;
extern int far pan_y;
extern int far current_color;
extern int far line_style;



FILE far *config_file_ptr;


struct config
{
	int baud_rate,cursor_speed;
	char drive_letter,listing_size,screen_update_mode,
		print_listings,update_timer,ems_mode;
	char error_path[80];
	char screen_resolution;
};


struct config far config_info;

int far size_config_file = sizeof(struct config);




/*dummy generic calls, which do nothing (except reading config file & 
setting up the remap color array)*/


long far d90_colors[16];



void read_config_file()
{
	config_file_ptr = fopen("work.cfg","r+b");

	if(config_file_ptr == NULL)
	{
		printf("Couldn't Open WORK.CFG Configuration File\n");
		restore_int9();
		exit();
	}

	fread(&config_info,size_config_file,1,config_file_ptr);

	fclose(config_file_ptr);


	drive_letter = config_info.drive_letter;
	init_baud_rate = config_info.baud_rate;
	strcpy(path_error_log,config_info.error_path);
	curser_ratio = config_info.cursor_speed;

	if(config_info.print_listings == TRUE)
		print_list = 't';
	else
		print_list = 'f';

	if(config_info.listing_size == 'n')
		list_size = 0;
	else
		list_size = 1;

	rotate_mode = config_info.screen_update_mode;

	if(config_info.update_timer == TRUE)
		time_redisplay = 't';
	else
		time_redisplay = 'f';

}






void far close_board()
{

}



int far init_artist10()
{
	read_config_file();
	return(9);
}



/*test the artist board & fill board_string*/
void far test_board()
{
unsigned int the_port,port_byte;


	release_version();
}





/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{

}



/*locks the drawing within the rectangular clipping window*/
void far my_lock_viewport()
{

}


/*scrolls between one page and the other*/
void far my_scroll(my_which_page)
int my_which_page;
{

}


/*selects a color register*/
void far my_color(i)
int i;
{

}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	d90_colors[the_register & 0x0f] =  ((long)red) | ((long)green << 8) |
		((long)blue << 16);
}


/*sets up the dashed line pattern*/
void far my_dash_pattern(i)
unsigned int i;
{

}


/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{

}


/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{

}





/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{

}






void far redisplay_charactor(the_char)
int the_char;
{

}





void far display_charactor(the_char)
int the_char;
{

}



/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{

}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{

}


/*clear screen in fast mode*/
void far clear_screen()
{

}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
int i;
{

}






void far dot_plot(x,y)
int x,y;
{

}






void far my_circle(x,y)
int x,y;
{

}








void far draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{

}








void far move_redisplay(x,y)
int x,y;
{

}




