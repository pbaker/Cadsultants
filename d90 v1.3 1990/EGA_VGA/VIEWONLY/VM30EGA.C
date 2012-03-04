#include "stdio.h"
#include "display.h"
#include "graph.h"

#define INVERT_Y 352


struct a_word far read_element;

FILE far *read_file_ptr;

int far my_x,far my_y;
char far read_file_name[80];




void far my_dash_pattern(i)
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




void far my_color(i)
int i;
{
	_setcolor(i);
}



void far draw_line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
	_moveto(5*x1/32,INVERT_Y - (int)(0.1139*y1));
	_lineto(5*x2/32,INVERT_Y - (int)(0.1139*y2));
}


void far redisplay_charactor(the_char)
int the_char;
{

}


void far move_redisplay(x,y)
int x,y;
{
	my_x = x;
	my_y = y;
}


void far dot_plot(x,y)
int x,y;
{
	_setpixel(5*x/32,INVERT_Y - (int)(0.1139*y));
}



void far my_circle(x,y)
int x,y;
{

}




void far display_a_word()
{
	switch(read_element.op_code)
	{
		case 1 : /*draw a line*/
		{
			my_dash_pattern(read_element.font);
			my_color(read_element.color);
			draw_line(read_element.arg1,read_element.arg2,
				read_element.arg3,read_element.arg4);

			my_x = read_element.arg3;
			my_y = read_element.arg4;

			break;
		}

		case 2 : /*dash pattern*/
		{
			my_dash_pattern(read_element.arg1 & 0x0f);
			break;
		}

		case 3 : /*set color*/
		{
			my_color(read_element.arg1 & 0x0f);
			break;
		}

		case 4 : /*draw a charactor*/
		{
			my_dash_pattern(read_element.font);
			my_color(read_element.color);
			redisplay_charactor(read_element.a_char);

			break;
		}

		case 5 : /*plot a point*/
		{
			my_dash_pattern(read_element.font);
			my_color(read_element.color);
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
			my_dash_pattern(read_element.font);
			my_color(read_element.color);
			my_circle(read_element.arg1,read_element.arg2);
			break;
		}

		case 44 : /*ignore*/
		{
			break;
		}

		default :
		{
/*			printf("UNKNOWN OP CODE IN REDISPLAY = %d\n",
				read_element.op_code);
*/
		}

	}

}





main()
{
try_again:
	printf("\nEnter the full path name of the artist/metheus d90 save file: ");
	scanf("%s",read_file_name);
	read_file_ptr = fopen(read_file_name,"r+b");

	if(read_file_ptr == NULL)
	{
		printf("\n\nInvalid file name!!!\n");
		goto try_again;
	}

	_setvideomode(_ERESCOLOR);

	while(fread(&read_element,SIZE_A_WORD,1,read_file_ptr) > 0)
		display_a_word();

	fclose(read_file_ptr);

	getch();

	_setvideomode(_DEFAULTMODE);
}