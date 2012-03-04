#include "stdio.h"
#include "ctype.h"
#include "c:\emulate\common\display.h"

#define SVWPT_PAGE_1 4

#define TRUE  't'
#define FALSE  'f'

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

extern void far restore_int9();


extern void far read_config_file();
extern void far read_lock();


extern void far insert_word(void);

extern void far release_version();

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

int far x,far y;
char far pen_press;

char far complement_on = 'f';
char far a_char_str[5];
int far the_color = 15;
int far pattern_num = 0;
int far ch,far drive_ok;
char far test_name[100];
char far drive_letter;
extern int far num_pages;
extern char far path_error_log[];
extern void far goto_row_col(int,int);

int far is_eof = 9;



unsigned int far m_dash_pattern = 0xffff;
char far m_dash_enable = FALSE;


short far monotronix[20] = {0x19,0x14,0x16,0x41,0x41,0x19,0x40,0x41,0xcc,
				0x0f,0,0,0,0,0,0,0,0,0,0};

int far rgb[5];

char far have_display_list;
char far ems_mode,far which_high_res;




int far my_listing_row = 0,far my_listing_col = 0;



/*1024 x 1024 version of vectrix graphics calls*/





void far clear_text_buffer()
{
int row,col;

	my_listing_row = 0;
	my_listing_col = 0;

	for(row = 0 ; row < 64 ; row++)
		for(col = 0 ; col < 128 ; col++)
			pputc(row,col,' ');
}





/*opens & inits metheus board & reads metheus.set file*/
int far init_artist10()
{
	read_config_file();
	read_lock();

	cold();
	init(&monotronix[0]);
	chr(1);

	return(0);
}



/*closes metheus board*/
void far close_board()
{

}




/*test the artist board & fill board_string*/
void far test_board()
{
	release_version();

	sprintf(board_string,"Vectrix Pepe");
}





/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
	if(my_which_page == SVWPT_PAGE_1)
	{
/*		crect(0,0,1023,767);*/
	}
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*"both pages"*/
			{
			/*	crect(0,0,1023,767);*/
			}
			/*else
				crect(x1,y1,x2,y2);*/
		}
		/*else
			crect(x1,y1,x2,y2);*/
	}
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
	color(i & 0x0f);
	the_color = i & 0x0f;
}


/*changes data in a color register*/
void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	rgb[0] = (red/4) | ((green/4) << 8);
	rgb[1] = blue/4;
	lut(the_register & 0x0f,1,&rgb[0]);
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
	switch(i)
	{
		case 0 : /*solid*/
		{
			pats(0xffff);
			break;
		}

		case 1 : /*dashed*/
		{
			pats(0xeeee);
			break;
		}

		case 2 : /*centerline*/
		{
			pats(0xfff6);
			break;
		}

		case 4 : /*phantom*/
		{
			pats(0xffb6);
			break;
		}
	}

}




/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	lines(x1,1024 - 4*y1/3,x2,1024 - 4*y2/3);
}




/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{

}








void far redisplay_charactor(the_char)
int the_char;
{
	the_char &= 0x7f;

	if((the_char == 10) || (the_char == 13))  /*newline*/
	{
		if(the_char == 13)
		{
			my_listing_col = 0;
			my_listing_row++;
		}
	}
	else
	{
		pputc(my_listing_row,my_listing_col,the_char);

		my_listing_col++;
	}

}








void far display_charactor(the_char)
int the_char;
{
int y;
long temp_y,remainder;

	the_char &= 0x7f;

	if(have_display_list == TRUE)
	{
		an_element.op_code = 4;
		an_element.a_char = the_char;
		an_element.arg1 = my_x;
		an_element.arg2 = my_y;
		an_element.color = current_color;
		an_element.font = line_style;

		insert_word();	
	}


	if((the_char == 10) || (the_char == 13))  /*newline*/
	{
		if(the_char == 13)
		{
			my_listing_col = 0;
			my_listing_row++;
		}
	}
	else
	{
		pputc(my_listing_row,my_listing_col,the_char);
		my_listing_col++;
	}

	if(out_ptr != NULL)
	{
		if((the_char == 10) || (the_char == 13))  /*newline*/
		{
			putc('\n',out_ptr);
			fflush(out_ptr);
		}
		else
			putc(the_char,out_ptr);
	}
}








/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
/*	setcsz(size,size);*/
}





/*clears a page of the display*/
void far clear_page(my_which_page)
int my_which_page;
{
	chr(0);
	clear_text_buffer();
	erase(0);
	chr(1);
}


/*clear screen in fast mode*/
void far clear_screen()
{
	chr(0);
	clear_text_buffer();
	erase(0);
	chr(1);
}



/*sets drawing mode (normal or complement)*/
void far draw_mode(i)
int i;
{
	if(i == 3)
		modes(0x03);
	else if(i == 0)
		modes(0);
}




void far draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
long remainder,temp_y;

	#ifdef HAVE_DIAGS
		printf("DRAW_LINE(%d %d %d %d)\n",x1,y1,x2,y2);
	#endif

        temp_y = (long)y1*10000L;
        remainder = temp_y%10156L;
        temp_y /= 10156L;

        y1 = (int)temp_y;

        if(remainder == 5078L)
                y1++;

        temp_y = (long)y2*10000L;
        remainder = temp_y%10156L;
        temp_y /= 10156L;

        y2 = (int)temp_y;

        if(remainder == 5078L)
                y2++;


	x1 = (int)((x1 - 2048)*new_zoom) + 2048 + pan_x;
	x2 = (int)((x2 - 2048)*new_zoom) + 2048 + pan_x;
	y1 = (int)((y1 - 1536)*new_zoom) + 1536 + pan_y;
	y2 = (int)((y2 - 1536)*new_zoom) + 1536 + pan_y;
	


	x1 /= 4;
	x2 /= 4;
	y1 /= 3;
	y2 /= 3;



	#ifdef HAVE_DIAGS
		printf("LINXY(%d %d %d %d)\n",x1,y1,x2,y2);
	#endif


	lines(x1,y1,x2,y2);
}







void far dot_plot(x,y)
int x,y;
{
long temp_y,remainder;


        temp_y = (long)y*10000L;
        remainder = temp_y%10156L;
        temp_y /= 10156L;

        y = (int)temp_y;

        if(remainder == 5078L)
                y++;
	

	x = (int)((x - 2048)*new_zoom) + 2048 + pan_x;
	y = (int)((y - 1536)*new_zoom) + 1536 + pan_y;
	

	x /= 4;
	y = y/3;


	dot(x,y);
}








void far my_circle(x,y)
int x,y;
{
long temp_y,remainder;


        temp_y = (long)y*10000L;
        remainder = temp_y%10156L;
        temp_y /= 10156L;

        y = (int)temp_y;

        if(remainder == 5078L)
                y++;



	x = (int)((x - 2048)*new_zoom) + 2048 + pan_x;
	y = (int)((y - 1536)*new_zoom) + 1536 + pan_y;
	

	
	x /= 4;
	y = y/3;

	cir(x,y,INDIC_RADIUS);
}




void far move_redisplay(x,y)
int x,y;
{

	x = (int)((x - 2048)*new_zoom) + 2048 + pan_x;
	y = (int)((y - 1536)*new_zoom) + 1536 + pan_y;
	

	my_x = x;
	my_y = y;

}

