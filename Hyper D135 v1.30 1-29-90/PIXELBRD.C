/* PIXELBRD.C   generic graphics board functions */



/*March 22, 1989 made my_dash_pattern()  do nothing at all due to a dave
problem with slpat32()*/

/*march 22, 1989 removed my interrupt handler to its own file*/

/* Sept. 15, 1989:

- increased TIME OUT constant to 300,000 (from 75,000)

*/

#include "c:\emulate\clipper\pw_lib.h"
#include "my_globs.h"
#include "ctype.h"
#include "dos.h"
#include "c:\emulate\clipper\expanded.h"



#define TIME_OUT 0
#define SUCCESS 1
#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1
#define RES_1024_768 3
#define RES_1280_1024 4


extern void far exit_dealloc_pages(int);

extern char far use_the_debug_mode;

extern char far licensee[];


extern void far read_lock();
extern void far close_host();


extern void far read_curser_pos(void);
extern void far goto_row_col(int,int);
extern int far *interrupt_reason_ptr;
extern char far ems_mode;
extern void far release_version();
extern void far restore_int9();
extern void far read_config_file();
extern void far asterisk();
extern void far plus();
extern void far dot();
extern int far init_baud_rate;
extern void far clear_display_page();


/*my interrupt handler function*/
extern void inthand();


int far x,far y;
char far pen_press;
char far complement_on = 'f';
char far m_dash_enable = 'f';
int far m_dash_pattern[20];
char far a_char_str[5];
int far the_color = 15;
int far pattern_num = 0;
char far is_eof = 'f';
int far ch,far drive_ok;
char far test_name[100];
char far which_high_res;
char far have_display_list;
unsigned long far max_wait_time = 700000L;


/*1024 x 768 version of clipper graphics calls*/





int far my_pw_dl_wait()
{
unsigned long duration;
int save_row,save_col;

	duration = 0L;

	while((*interrupt_reason_ptr) < 0)
	{
		duration = duration + 1L;


		if(duration > max_wait_time)

		{
			save_row = row_curser;
			save_col = col_curser;
			read_curser_pos();
			goto_row_col(24,68);

			putchar(7);
			printf("TIME OUT   ");

			goto_row_col(save_row,save_col);
			row_curser = save_row;
			col_curser = save_col;

			return(TIME_OUT);
		}
	}


	return(SUCCESS);
}




/*opens & inits clipper board & reads NEW Work.cfg file*/
int far init_artist10()
{
int i;

	if(use_the_debug_mode == 't')
		printf("\t* Reading CUSTOMER.ID File\n");


	read_lock();

	if(use_the_debug_mode == 't')
	{
		printf("\t* Read Complete\n");
		printf("\t* Reading WORK.CFG File\n");
	}

	read_config_file();


	if(use_the_debug_mode == 't')
	{
		printf("\t* Read Complete\n");
		printf("\t* Install Clipper Interrupt Routine\n");
	}

	i = pw_setup(10,6,0x330,inthand);


	if(use_the_debug_mode == 't')
		printf("\t* Clipper Interrupt Routine Installed\n");


	if(i != 0)
	{
		exit_dealloc_pages(handle);
		printf("ERROR DURING pw_setup = %d\n",i);
		close_host();
		restore_int9();
		exit();
	}


	if(use_the_debug_mode == 't')
		printf("\t* Initializing Transform Matrices\n");


	pw_dl_init(NULL,0,NULL,0);
	mdiden2();
	mdiden3();
	mdorg2(0,0);
	mdorg3(0,0,0);
	vwmatx3(&my_3d_viewing_matrix[0]);
	vwrpt3(0,0,0);
	gtsize(40,40);
	gtstyle(2);
	gtprop(1);

	rdmask(0x0f);
	wrmask(0x0f);
	linpat(0);
	window(-2048,2048,-2048,2048);


	if(use_the_debug_mode == 't')
		printf("\t* Matrices Initialized\n");

	return(0);
}



/*closes expanded memory board*/
void far close_board()
{
	dealloc_pages_for_handle(handle);
}




/*test the artist board & fill board_string*/
void far test_board()
{
	sprintf(board_string,"Pixel Works");
}



/*sets up a rectanglular clipping window*/
void far my_set_viewport(x1,y1,x2,y2,my_which_page) 
int x1,y1,x2,y2,my_which_page;
{
	window(x1,x2,y1,y2);


	if(which_high_res == RES_1024_768)
	{
		x1 = (int)(x1/5.33333333333) + 512;
		x2 = (int)(x2/5.33333333333) + 512;
		y1 = (int)((y1 + 2048)/5.333333333333);
		y2 = (int)((y2 + 2048)/5.333333333333);
	}
	else
	{
		x1 = (int)(x1*1.25/5.33333333333) + 640;
		x2 = (int)(x2*1.25/5.33333333333) + 640;
		y1 = (int)((y1 + 2048)/5.12*1.28);
		y2 = (int)((y2 + 2048)/5.12*1.28);
	}



	if(my_which_page == SVWPT_PAGE_1)
	{
		if(which_high_res == RES_1024_768)
			viewport(128,895,0,767);
		else
			viewport(160,1120,0,1023);
	}
	else
	{
		if(num_pages == 2)
		{
			if(my_which_page == 2)    /*"both pages"*/
			{
				if(which_high_res == RES_1024_768)
					viewport(128,895,0,767);
				else
					viewport(160,1120,0,1023);
			}
			else
				viewport(x1,x2,y1,y2);
		}
		else
			viewport(x1,x2,y1,y2);
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

	color((i & 0x0f) | ((i & 0x0f) << 4));
	the_color = i & 0x0f;
}


/*changes data in a color register;
   not currently executed - causes communication data error */

void far color_reg(the_register,red,green,blue)
int the_register,red,green,blue;
{
	lut8(the_register & 0x0f,(red << 2) & 0xff,(green << 2) & 0xff,
		(blue << 2) & 0xff);
	lut8((the_register & 0x0f) << 4,(red << 2) & 0xff,(green << 2) & 0xff,
		(blue << 2) & 0xff);
}




/* routine sets up the dashed line pattern;
   not currently executed - causes communication data error */

void far my_dash_pattern(i)
unsigned int i;
{
	switch(i)
	{
		case 0xffff : /*solid*/
		{
			slpat32(1,0xffffffff);
			break;
		}

		case 0xeeee : /*dashed*/
		{
			slpat32(1,0xf0f0f0f0);
			break;
		}

		case 0xffb6 : /*phantom*/
		{
			slpat32(1,0xfff0f0f0);
			break;
		}

		case 0xfff6 : /*centerline*/
		{
			slpat32(1,0xfffff0f0);
			break;
		}
	}
}




/*determines whether lines are dashed or not*/
void far my_dash_enable(i)
int i;
{


	if(i == 0)
	{
		linpat(0);
	}
	else if(i == 4)
		linpat(1);
}




/*draws a line from (x1,y1) to (x2,y2)*/
void far my_line_xy(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	vect3(x1,y1,0,x2,y2,0);
}


/*puts a charactor on the screen with rotation = "rotate"*/
void far place_char(the_char,x,y,rotate)
int the_char,x,y,rotate;
{
	if((the_char >= 32) && (the_char <= 127))
	{
		move2(x,y);
		a_char_str[0] = the_char;
		a_char_str[1] = '\0';
		gtext(a_char_str);
	}
}



/*sets up the size of charactors*/
void far my_size_chars(size)
int size;
{
	if(size == 0)
	{
		gtsize(40,40);
	}
	else
		gtsize(80,80);
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



