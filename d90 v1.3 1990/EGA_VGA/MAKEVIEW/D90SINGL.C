#include "stdio.h"
#include "display.h"
#include "graph.h"

#define INVERT_Y 482


extern void far make_win(int,int,int,int);
extern void far save_win(void);
extern void far restore_win(void);
extern void far turn_off_curser(void);
extern void far read_curser_pos(void);
extern void far turn_on_curser(void);
extern void far goto_row_col(int,int);


struct a_word far read_a_word;
extern FILE far *display_file;
extern int far row_curser;
extern int far col_curser;


extern long far d90_colors[];



void far single_dash_pattern(i)
int i;
{
	switch(i)
	{
		case 0 : /*solid*/
		{
			_setlinestyle(0xffff);
			break;
		}

		case 1 : /*dashed*/
		{
			_setlinestyle(0xcccc);
			break;
		}

		case 2 : /*centerline*/
		{
			_setlinestyle(0xff1c);
			break;
		}

		case 4 : /*phantom*/
		{
			_setlinestyle(0xfccc);
			break;
		}
	}
}




void far single_color(i)
int i;
{
	_setcolor(i);
}



void far single_draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	_moveto(5*x1/32,INVERT_Y - 5*y1/32);
	_lineto(5*x2/32,INVERT_Y - 5*y2/32);
}



void far single_character(the_char)
int the_char;
{

}


void far single_move(x,y)
int x,y;
{

}



void far single_dot_plot(x,y)
int x,y;
{
	_setpixel(5*x/32,INVERT_Y - 5*y/32);
}





void far single_circle(x,y)
int x,y;
{

}




void far display_a_word()
{
	switch(read_a_word.op_code)
	{
		case 1 : /*draw a line*/
		{
			single_dash_pattern(read_a_word.font);
			single_color(read_a_word.color);
			single_draw_line(read_a_word.arg1,read_a_word.arg2,
				read_a_word.arg3,read_a_word.arg4);

			break;
		}

		case 2 : /*dash pattern*/
		{
			single_dash_pattern(read_a_word.arg1 & 0x0f);
			break;
		}

		case 3 : /*set color*/
		{
			single_color(read_a_word.arg1 & 0x0f);
			break;
		}

		case 4 : /*draw a charactor*/
		{
			single_dash_pattern(read_a_word.font);
			single_color(read_a_word.color);
			single_character(read_a_word.a_char);

			break;
		}

		case 5 : /*plot a point*/
		{
			single_dash_pattern(read_a_word.font);
			single_color(read_a_word.color);
			single_dot_plot(read_a_word.arg1,read_a_word.arg2);


			break;
		}

		case 6 : /*move*/
		{
			single_move(read_a_word.arg1,read_a_word.arg2);

			break;
		}

		case 7 : /*display_indicator*/
		{
			single_dash_pattern(read_a_word.font);
			single_color(read_a_word.color);
			single_circle(read_a_word.arg1,read_a_word.arg2);
			break;
		}

		case 44 : /*ignore*/
		{
			break;
		}

		default :
		{
/*			printf("UNKNOWN OP CODE IN REDISPLAY = %d\n",
				read_a_word.op_code);
*/
		}

	}

}




void far single_redisplay()
{
int row,col;

	turn_off_curser();
	read_curser_pos();
	make_win(0,0,79,24);
	save_win();

	_setvideomode(_VRES16COLOR);

	_remapallpalette(d90_colors);


	fseek(display_file,0L,SEEK_SET);


	while(fread(&read_a_word,SIZE_A_WORD,1,display_file) > 0)
		display_a_word();


	fseek(display_file,0L,SEEK_END);
	


/*wait for key press before restoring Host menu*/
	if(getch() == 0)   /*if its an extended code*/
		getch();

	_setvideomode(_DEFAULTMODE);

	turn_off_curser();

	restore_win();

	make_win(10,3,60,16);

	goto_row_col(row_curser,col_curser);

	turn_on_curser();
}
