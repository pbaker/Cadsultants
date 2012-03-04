#include "stdio.h"
#include "graph.h"
#include "dos.h"
#include "conio.h"



#define dim_chr          28
#define bright_chr       29
#define space            20
#define cr               13
#define lf               10
#define bs               8
#define bel              7



#define TRUE 1
#define FALSE 0




#define USE_COLOR_CARD  1
#define USE_MONO_CARD  0


/*
#define HAVE_DIAGS
*/


char far my_text_mode = USE_COLOR_CARD;
char far my_test_flag = FALSE;

char far *mono_ptr;

char far char_type = 29;
char far bright_attribute = 15;
char far lite_attribute = 7;
char far dim_attribute = 7;
char far menu_attribute = 7;


int far mono_row = 0,far mono_col = 0;


extern void far clear_display_page();
extern void far enter_graphics_mode();
extern void far ugout(int);


extern union REGS far my_input_regs,far my_output_regs;




void far show_graphics_mode(a_string)
char far a_string[];
{
#ifdef HAVE_DIAGS
	my_input_regs.h.ah = 15;
	int86(0x10,&my_input_regs,&my_output_regs);


	fprintf(stdprn,"%s : page = %d  video mode = %d\n\n",
		a_string,
		my_output_regs.h.bh,my_output_regs.h.al);
#endif
}




void far bios_teletype(a_char)
int a_char;
{
	my_input_regs.h.ah = 14;
	my_input_regs.h.al = a_char;
	my_input_regs.h.bh = 0;

	int86(0x10,&my_input_regs,&my_output_regs);
}




void far goto_mono_row_col(row,col)
int row,col;
{
int real_position;

	real_position = 80*row + col;

	outp(0x3b4,14);     /*high part*/
	outp(0x3b5,(real_position >> 8) & 0xff);

	outp(0x3b4,15);     /*low part*/
	outp(0x3b5,real_position & 0xff);
}





void far poscur(row,col)
int row,col;
{
	mono_row = row;
	mono_col = col;
	goto_mono_row_col(row,col);
}





void far host_poscur(row,col)
int row,col;
{
#ifdef HAVE_DIAGS
fprintf(stdprn,"HOST POSCUR() Before changing:  my_test_flag =  ");
if(my_test_flag == TRUE)
	fprintf(stdprn,"TRUE\n");
else
	fprintf(stdprn,"FALSE\n");

show_graphics_mode("host poscur()  ");
fflush(stdprn);

#endif

	my_test_flag = FALSE;

	mono_row = row;
	mono_col = col;
	goto_mono_row_col(row,col);
}







void far clrall()
{
int row,col;


	poscur(0,0);

	for(row = 0 ; row < 24 ; row++)
		for(col = 0 ; col < 80 ; col++)
		{
			*(mono_ptr + row*160 + (col << 1)) = ' ';
			*(mono_ptr + row*160 + (col << 1) + 1) = 15;
		}
}






void far host_clrall()
{
int row,col;

#ifdef HAVE_DIAGS
fprintf(stdprn,"HOST CLRALL() before changing: text_mode =  ");
if(my_text_mode == USE_COLOR_CARD)
	fprintf(stdprn,"COLOR CARD\n");
else
	fprintf(stdprn,"MONO CARD\n");

show_graphics_mode("Host clrall() ");
fflush(stdprn);

#endif

	my_test_flag = TRUE;


	if(my_text_mode == USE_COLOR_CARD)
	{
		clear_display_page();
		my_text_mode = USE_MONO_CARD;
		enter_graphics_mode();
	}


	poscur(0,0);


	for(row = 0 ; row < 24 ; row++)
		for(col = 0 ; col < 80 ; col++)
		{
			*(mono_ptr + row*160 + (col << 1)) = ' ';
			*(mono_ptr + row*160 + (col << 1) + 1) = 15;
		}
}



void far clrnon()
{
int row,col;

	poscur(0,0);

	for(row = 0 ; row < 18 ; row++)
		for(col = 0 ; col < 57 ; col++)
		{
			*(mono_ptr + row*160 + (col << 1)) = ' ';
			*(mono_ptr + row*160 + (col << 1) + 1) = 15;
		}

}




void far my_teletype(the_char)
int the_char;
{

        the_char &= 0xff;


        if(the_char == cr)
                poscur(mono_row,0);
        else if(the_char == lf)
        {
                if(mono_row >= 24)
                {
			clrall();
                        poscur(0,0);
                }
                else
                        poscur(mono_row + 1,mono_col);
        }
        else
        {
		*(mono_ptr + mono_row*160 + (mono_col << 1)) = the_char;
		*(mono_ptr + mono_row*160 + (mono_col << 1) + 1) = 15;

                poscur(mono_row,mono_col + 1);
        }

}





void far dischr(the_char)
char the_char;
{

	the_char = the_char & 0xff;

	if(my_test_flag == TRUE)
	{
#ifdef HAVE_DIAGS

fprintf(stdprn,"HOST DISCHR() before changing:  my_test_flag == TRUE   text_mode =  ");
if(my_text_mode == USE_COLOR_CARD)
	fprintf(stdprn,"COLOR CARD\n");
else
	fprintf(stdprn,"MONO CARD\n");
show_graphics_mode("Dischr(): inside the test_flag if\n");
fflush(stdprn);

#endif

		ugout(19);
		_setvideomode(_TEXTC80);
		ugout(17);

		my_text_mode = USE_COLOR_CARD;
		my_test_flag = FALSE;
	}


	if(my_text_mode == USE_COLOR_CARD)
	{
		bios_teletype(the_char);
		return;
	}


	if(char_type == dim_chr)
	{
		switch(the_char)
		{
			case space :
				goto bright;

			case cr    :
				goto bright;

			case lf    :
				goto bright;

			case bs    :
				goto bright;

			case bel   :
				goto bright;
		}

		*(mono_ptr + mono_row*160 + (mono_col << 1)) = the_char;
		*(mono_ptr + mono_row*160 + (mono_col << 1) + 1) = 7;

		if(mono_col == 79)
			poscur(mono_row + 1,0);
		else
			poscur(mono_row,mono_col + 1);

	}
	else
	{
bright:
		if(mono_row >= 24)
		{
			clrall();
			poscur(0,0);
		}

		my_teletype(the_char);
	}
	
}

