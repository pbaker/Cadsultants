/*  C:\Emulate\Common\Bios.c   my bios calls done through  "C"  */



#include "stdio.h"
#include "dos.h"



#define NORMAL_VIDEO 0
#define REVERSE_VIDEO 1




extern union REGS far my_input_regs;
extern union REGS far my_output_regs;
extern char far char_save_page[25][80];
extern char far attr_save_page[25][80];

extern int far win_x_low;
extern int far win_y_low;
extern int far win_x_up;
extern int far win_y_up;

extern int far row_curser;
extern int far col_curser;






void far read_character(the_char,the_attribute)
int far *the_char,far *the_attribute;
{
	my_input_regs.h.ah = 8;
	my_input_regs.h.bh = 0;

	int86(0x10,&my_input_regs,&my_output_regs);

	(*the_char) = my_output_regs.h.al;
	(*the_attribute) = my_output_regs.h.ah;
}







void far output_character(the_char,the_attribute)
int the_char,the_attribute;
{
	my_input_regs.h.ah = 9;
	my_input_regs.h.bh = 0;
	my_input_regs.x.cx = 1;

	my_input_regs.h.al = the_char;
	my_input_regs.h.bl = the_attribute;

	int86(0x10,&my_input_regs,&my_output_regs);
}





void far attribute_string(the_string,row,col,the_attribute)
char far the_string[];
int row,col;
unsigned int the_attribute;
{
int i;

	for(i = 0 ; the_string[i] != '\0' ; i++)
	{
		my_input_regs.h.ah = 2;
		my_input_regs.h.bh = 0;
		my_input_regs.h.dh = row;
		my_input_regs.h.dl = col + i;

		int86(0x10,&my_input_regs,&my_output_regs);

		my_input_regs.h.ah = 9;
		my_input_regs.h.al = the_string[i];
		my_input_regs.h.bh = 0;
		my_input_regs.h.bl = the_attribute;
		my_input_regs.x.cx = 1;

		int86(0x10,&my_input_regs,&my_output_regs);
	}

}








void far reverse_string(the_string,row,col)
char the_string[];
int row,col;
{
int i;

	for(i = 0 ; the_string[i] != '\0' ; i++)
	{
		my_input_regs.h.ah = 2;
		my_input_regs.h.bh = 0;
		my_input_regs.h.dh = row;
		my_input_regs.h.dl = col + i;

		int86(0x10,&my_input_regs,&my_output_regs);

		my_input_regs.h.ah = 9;
		my_input_regs.h.al = the_string[i];
		my_input_regs.h.bh = 0;
		my_input_regs.h.bl = 0x70;
		my_input_regs.x.cx = 1;

		int86(0x10,&my_input_regs,&my_output_regs);
	}

}







void far blinking_string(the_string,row,col)
char the_string[];
int row,col;
{
int i;

	for(i = 0 ; the_string[i] != '\0' ; i++)
	{
		my_input_regs.h.ah = 2;
		my_input_regs.h.bh = 0;
		my_input_regs.h.dh = row;
		my_input_regs.h.dl = col + i;

		int86(0x10,&my_input_regs,&my_output_regs);

		my_input_regs.h.ah = 9;
		my_input_regs.h.al = the_string[i];
		my_input_regs.h.bh = 0;
		my_input_regs.h.bl = 128 | 0x0f;
		my_input_regs.x.cx = 1;

		int86(0x10,&my_input_regs,&my_output_regs);
	}

}






void far normal_string(the_string,row,col)
char the_string[];
int row,col;
{
int i;

	for(i = 0 ; the_string[i] != '\0' ; i++)
	{
		my_input_regs.h.ah = 2;
		my_input_regs.h.bh = 0;
		my_input_regs.h.dh = row;
		my_input_regs.h.dl = col + i;

		int86(0x10,&my_input_regs,&my_output_regs);

		my_input_regs.h.ah = 9;
		my_input_regs.h.al = the_string[i];
		my_input_regs.h.bh = 0;
		my_input_regs.h.bl = 0x0f;
		my_input_regs.x.cx = 1;

		int86(0x10,&my_input_regs,&my_output_regs);
	}

}






void far turn_off_curser()
{
	my_input_regs.h.ah = 1;
	my_input_regs.x.cx = 0x2000;

	int86(0x10,&my_input_regs,&my_output_regs);
}





void far turn_on_curser()
{
	my_input_regs.h.ah = 1;
	my_input_regs.h.ch = 6;
	my_input_regs.h.cl = 7;

	int86(0x10,&my_input_regs,&my_output_regs);
}








void far make_win(xl,yl,xu,yu)
int xl,yl,xu,yu;
{
	win_x_low = xl;
	win_y_low = yl;
	win_x_up = xu;
	win_y_up = yu;
}





void far clear_win(mode)
int mode;
{
	my_input_regs.h.ah = 6;
	my_input_regs.h.al = 0;


	if(mode == NORMAL_VIDEO)
		my_input_regs.h.bh = 15;
	else
		my_input_regs.h.bh = 0x70;


	my_input_regs.h.ch = win_y_low;
	my_input_regs.h.cl = win_x_low;


	if(win_y_up > 24)
		my_input_regs.h.dh = 24;
	else
		my_input_regs.h.dh = win_y_up;


	if(win_x_up > 79)
		my_input_regs.h.dl = 79;
	else
		my_input_regs.h.dl = win_x_up;


	int86(0x10,&my_input_regs,&my_output_regs);
}





void far save_win()
{
int row,col;
int max_row,max_col;


	max_row = win_y_up + 2;
	if(max_row > 24)
		max_row = 24;

	max_col = win_x_up + 5;
	if(max_col > 79)
		max_col = 79;


	for(row = win_y_low ; row <= max_row ; row++)
		for(col = win_x_low ; col <= max_col ; col++)
		{
			my_input_regs.h.ah = 2;
			my_input_regs.h.bh = 0;
			my_input_regs.h.dh = row;
			my_input_regs.h.dl = col;
			int86(0x10,&my_input_regs,&my_output_regs);

			my_input_regs.h.ah = 8;
			my_input_regs.h.bh = 0;

			int86(0x10,&my_input_regs,&my_output_regs);

			char_save_page[row][col] = my_output_regs.h.al;
			attr_save_page[row][col] = my_output_regs.h.ah;
		}
}





void far restore_win()
{
int row,col;
int max_row,max_col;


	max_row = win_y_up + 2;
	if(max_row > 24)
		max_row = 24;

	max_col = win_x_up + 5;
	if(max_col > 79)
		max_col = 79;


	for(row = win_y_low ; row <= max_row ; row++)
		for(col = win_x_low ; col <= max_col ; col++)
		{
			my_input_regs.h.ah = 2;
			my_input_regs.h.bh = 0;
			my_input_regs.h.dh = row;
			my_input_regs.h.dl = col;
			int86(0x10,&my_input_regs,&my_output_regs);

			my_input_regs.h.ah = 9;
			my_input_regs.h.bh = 0;
			my_input_regs.x.cx = 1;

			my_input_regs.h.al = char_save_page[row][col];
			my_input_regs.h.bl = attr_save_page[row][col];

			int86(0x10,&my_input_regs,&my_output_regs);
		}


}




void far goto_row_col(row,col)
int row,col;
{
	my_input_regs.h.ah = 2;
	my_input_regs.h.bh = 0;
	my_input_regs.h.dh = row;
	my_input_regs.h.dl = col;

	int86(0x10,&my_input_regs,&my_output_regs);

}



void far read_curser_pos()
{
	my_input_regs.h.ah = 3;
	my_input_regs.h.bh = 0;

	int86(0x10,&my_input_regs,&my_output_regs);

	row_curser = my_output_regs.h.dh;
	col_curser = my_output_regs.h.dl;
}


void far draw_box()
{
int i;

	goto_row_col(win_y_low,win_x_low + 1);   /*top of box*/
	for(i = win_x_low + 1 ; i < win_x_up ; i++)
		putchar(205);

	goto_row_col(win_y_up,win_x_low + 1);   /*bottom of box*/
	for(i = win_x_low + 1 ; i < win_x_up ; i++)
		putchar(205);

	goto_row_col(win_y_low,win_x_low);      /*upper left corner*/
	putchar(201);	

	goto_row_col(win_y_low,win_x_up);       /*upper right corner*/
	putchar(187);

	goto_row_col(win_y_up,win_x_low);       /*lower left corner*/
	putchar(200);

	goto_row_col(win_y_up,win_x_up);        /*lower right corner*/
	putchar(188);

	for(i = win_y_low + 1 ; i < win_y_up; i++)
	{
		goto_row_col(i,win_x_low);
		putchar(186);

		goto_row_col(i,win_x_up);
		putchar(186);
	}
}




void far teletype(ch)
int ch;
{
	my_input_regs.h.ah = 14;
	my_input_regs.h.al = ch;

	int86(0x10,&my_input_regs,&my_output_regs);
}





void far scroll_up(num_lines,x1,y1,x2,y2,attribute)
int num_lines,x1,y1,x2,y2,attribute;
{

	my_input_regs.h.ah = 6;
	my_input_regs.h.al = num_lines;
	my_input_regs.h.bh = attribute;
	my_input_regs.h.ch = y1;
	my_input_regs.h.cl = x1;
	my_input_regs.h.dh = y2;
	my_input_regs.h.dl = x2;

	int86(0x10,&my_input_regs,&my_output_regs);
}








void far scroll_down(num_lines,x1,y1,x2,y2,attribute)
int num_lines,x1,y1,x2,y2,attribute;
{

	my_input_regs.h.ah = 7;
	my_input_regs.h.al = num_lines;
	my_input_regs.h.bh = attribute;
	my_input_regs.h.ch = y1;
	my_input_regs.h.cl = x1;
	my_input_regs.h.dh = y2;
	my_input_regs.h.dl = x2;

	int86(0x10,&my_input_regs,&my_output_regs);
}



