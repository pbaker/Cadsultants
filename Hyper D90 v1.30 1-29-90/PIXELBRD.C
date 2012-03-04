#include "dos.h"
#include "stdio.h"
#include "ctype.h"
#include "display.h"
#include "c:\emulate\clipper\pw_lib.h"

#define SVWPT_PAGE_1 4


#define INDIC_RADIUS 4
#define DL_REAL_END 0x1000
#define IS_EMPTY  0
#define IS_NOT_EMPTY  1


int far initial_joy_count;
int far rw_threshold;
unsigned long far max_wait_time;






char far my_text_buffer[111];
char far text_buffer_status = IS_EMPTY;
int far text_x,text_y;
int far text_buffer_index = 0;




/*my own pixelworks style interrupt handler*/
extern void inthand(int);



extern void far read_lock();
extern void far read_config_file();


extern char far which_high_res;


extern int far curser_ratio;
extern char far rotate_mode;
extern int far list_size;
extern char far print_list;
extern int far time_redisplay;
extern int far init_baud_rate;
extern char far ems;

extern char far path_error_log[];

extern void far restore_int9();


extern void far insert_word();
extern void far insert_black_word();

extern char far black;
extern int far my_x;
extern int far my_y;
extern FILE far *out_ptr;
extern struct a_word far an_element;
extern int far init_baud_rate;
extern void far clear_display_page();
extern char far board_string[];
extern float far new_zoom;
extern int far pan_x;
extern int far pan_y;
extern char far zoom_pan_enable;
extern int far current_color;
extern int far line_style;
extern char far int_happened;
extern char far reached_end_dl;
extern int far int_reason;

int far x,far y;
char far pen_press;

char far complement_on = 'f';
char far a_char_str[5];
int far the_color = 15;
int far pattern_num = 0;
int far is_eof = 'f';
int far ch;
char far drive_ok,far test_name[100];
char far drive_letter;
extern int far num_pages;
int far unity_2d_matrix[9] =
	{
		0x1000,0,0,
		0,0x1000,0,
		0,0,1
	};


int far level_of_recursion = 0;
char far ems_mode;
char far have_display_list;



extern short far indicator_dl[];
float far indicator_radius = 11.0;
int far radius_add_on = 1;


/*1024 x 768 version of d90 clipper graphics calls*/



void far flush_text_buffer()
{
int i;

	if(text_buffer_status == IS_EMPTY)
		return;

	text_buffer_status = IS_EMPTY;
	an_element.op_code = 4;
	an_element.arg1 = text_x;
	an_element.arg2 = text_y;

	insert_word();

	move2(text_x,text_y);
	gtext(my_text_buffer);


/*clear out text buffer*/
	for(i = 0 ; i < 110 ; i++)
		my_text_buffer[i] = '\0';

}




/*opens & inits metheus board & reads metheus.set file*/
int far init_artist10()
{
int i;

	read_lock();
	read_config_file();
	ems = ems_mode;

	i = pw_setup(10,6,0x330,inthand);

	if(i != 0)
	{
		printf("ERROR DURING pw_setup = %d\n",i);
		restore_int9();
		exit();
	}



	pw_dl_init(&indicator_dl[0],0,NULL,0);
	circle((int)indicator_radius + radius_add_on);
	ret();
	pw_dl_init(NULL,0,NULL,0);


	gtstyle(2);

	return(0);
}



/*closes metheus board*/
void far close_board()
{

}




/*test the artist board & fill board_string*/
void far test_board()
{
	sprintf(board_string,"Pixelworks Clipper");
}



/*for compatability with the common my13 file*/
my_set_viewport(x1,y1,x2,y2,which_page)
int x1,y1,x2,y2,which_page;
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
	color(((i & 0x0f) << 4) | (i & 0x0f));
	the_color = i & 0x0f;
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	lut8(the_register & 0x0f,red << 2,green << 2,blue << 2);
	lut8((the_register & 0x0f) << 4,red << 2,green << 2,blue << 2);
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
	linpat(i);
}




/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	vect2(x1,y1,x2,y2);
}




/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
/*
	a_char_str[0] = the_char;
	a_char_str[1] = '\0';
	move2(x,y);
	gtext(a_char_str);
*/
}





void far display_charactor(the_char)
int the_char;
{
int y;
long temp_y,remainder;

	the_char &= 0x7f;

	if(text_buffer_status == IS_EMPTY)
	{
		text_buffer_status = IS_NOT_EMPTY;
		text_x = my_x;
		text_y = my_y;
		text_buffer_index = 0;
	}


	if(the_char >= 32)
	{
		my_text_buffer[text_buffer_index] = the_char;
		text_buffer_index++;
	}

#ifdef fkjoefwokfo

	#ifdef HAVE_DISP_FILE
		an_element.op_code = 4;
		an_element.a_char = the_char;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.color = current_color;
		an_element.font = line_style;	
	#endif	



	if((the_char == 10) || (the_char == 13))  /*newline*/
	{
		my_x = 0;
		my_y += 32;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.a_char = ' ';
	}
	else
	{
		place_char(the_char,my_x,my_y,6);

		my_x += 32;
	}


	insert_word();
#endif


	if(out_ptr != NULL)
	{
		if((the_char == 10) || (the_char == 13))  /*newline*/
			putc('\n',out_ptr);
		else
			putc(the_char,out_ptr);
	}
}








/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
	gtsize((size + 1)*40,(size + 1)*40);
}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	flood(0);
}


/*clear screen in fast mode*/
void far clear_screen()
{
	flood(0);
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
int i;
{
	if(i == 3)
	{
		linfun(11);
	}
	else if(i == 0)
		linfun(12);
}




void far draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
long remainder,temp_y;

	#ifdef HAVE_DIAGS
		printf("DRAW_LINE(%d %d %d %d)\n",x1,y1,x2,y2);
	#endif

	vect2(x1,y1,x2,y2);
}







void far dot_plot(x,y)
int x,y;
{
long temp_y,remainder;

	move2(x,y);
	point();

}








void far my_circle(x,y)
int x,y;
{
long temp_y,remainder;

	move2(x,y);

 /* circle indicator (to screen only) */
	circle((int)(indicator_radius/new_zoom + 0.5) + radius_add_on);

/* must match computation in redisplay()  circle indicator sub fill code*/
}




void far move_redisplay(x,y)
int x,y;
{
	my_x = x;
	my_y = y;

}

