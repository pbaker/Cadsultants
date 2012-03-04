/* C version of paul's mmfilter.4c for DTI project*/

/* 3/28/91 removed those delay loops because they cause keyboard interference */


#include <stdio.h>
#include "my_utils.h"


#define VT100_MODE  0
#define MESS_MON  1


extern void  change_page_gmark_undraw();
extern void  clear_whole_screen();

extern void  update_the_windows();

extern void  printme();
extern void  host_poscur();
extern void  which_icu();

extern void  my_pckb();
extern void  host_escape();
extern int  read_host_buffer();
extern void  poscur();
extern void  clrnon();
extern void  clrall();
extern void  dischr();
extern void host_clrall();


extern void  display_non_protect_row();

extern void  flood_draw_gmark();

extern int  the_vt100_color;

extern int  vt100_mess_mon_flag;


extern char  buffer_non_protect;
extern char  did_host_poscur;


extern int  exit_flag;
extern char  char_type;
extern char  page_for_write;

extern unsigned char  megatek_data[];
extern int  row_curser;
extern int  col_curser;


extern int  mono_row;
extern int  mono_col;



extern char  my_test_flag;





void  mm_read_more(number)
int number;
{
int i,j,ch;

        for(i = 0 ; i < number ; i++)
        {
                ch = read_host_buffer();

                while(ch == -1)
                {
/*
                        for(j = 0 ; j < 90000 ; j++)
                                ;
*/

                        my_pckb();

                        ch = read_host_buffer();
                }

/*
		if(ch == 0x1b)
			printf("\n mm_read_more()  ESCAPE ");
		else
			printme(ch);
*/

                megatek_data[i] = ch & 0xff;
        }

}





void  the_escape_handler()
{
int ch;
int i,j;


/*
	printf("\nAN ESCAPE CHAR  ");
*/

	ch = read_host_buffer();
	while(ch == -1)
	{
/*
		for(j = 0 ; j < 90000 ; j++)
			;
*/

		ch = read_host_buffer();
	}

	ch = ch & 0x7f;

/*
	printme(ch);
printf("\n ESCAPE  %c = %x\n",ch,ch);
*/

	switch(ch)
	{
		case '0' :
			return;

		case '1' :
		{
			exit_flag = 1;

	/*
			display_non_protect_row();
			buffer_non_protect = 'f';
	
			update_the_windows();
	*/

			buffer_non_protect = 'f';

			return;
		}

		case '2' :
		{
			if(vt100_mess_mon_flag == VT100_MODE)
			{
				clear_whole_screen();
				clear_screen();       /*the graphics window*/

				vt100_mess_mon_flag = MESS_MON;
				change_page_gmark_undraw();
			}


			vt100_mess_mon_flag = MESS_MON;

			alt_functions();


			return;
		}

		case '3' :
		case '4' :
		{
			return;
		}

		case '5' :
		{
			which_icu();
			return;
		}

		default :
		{
			host_escape(ch);
			return;
		}
	}
}





void  mm_filter(the_char)
int the_char;
{
        the_char = the_char & 0xff;

        if(the_char < 32)
        {
                switch(the_char)
                {
                        case 0 : /*null*/
                                break;

                        case 1 : /*soh*/
                                break;

                        case 2 : /*curser home*/
                        {
                                poscur(0,0);
                                break;
                        }

                        case 3 : /*enable blink*/
                                break;

                        case 4 : /*disable blink*/
                                break;

                        case 5 : /*read_cursor*/
                                break;

                        case 6 : /*clear*/
                        {
				if(vt100_mess_mon_flag == VT100_MODE)
				{
					clear_whole_screen();
					clear_screen();  /* graphics window */
					vt100_mess_mon_flag = MESS_MON;
					change_page_gmark_undraw();
				}

				vt100_mess_mon_flag = MESS_MON;

                                clrnon();

                                break;
                        }

                        case 7 : /*alert*/
                        {
                                putchar(7);
                                break;
                        }

                        case 8 : /*bs*/
                        {
                                poscur(mono_row,mono_col - 1);
                                break;
                        }

                        case 9 : /*ht*/
                                break;

                        case 10 : /*new line*/
                        {
                                dischr(10);
                                dischr(13);
                                break;
                        }

                        case 11 : /*erase to eol*/
                                break;

                        case 12 : /*clear all*/
                        {
				if(vt100_mess_mon_flag == VT100_MODE)
				{
					clear_whole_screen();
					clear_screen();  /* graphics window */
					vt100_mess_mon_flag = MESS_MON;
					change_page_gmark_undraw();
					vt100_mess_mon_flag = VT100_MODE;
				}


                                clrall();

				my_test_flag = 't';
				vt100_mess_mon_flag = MESS_MON;


                                break;
                        }

                        case 13 : /*carriage return*/
                        {
                                dischr(13);
                                break;
                        }

                        case 14 : /*start blink*/
                                break;

                        case 15 : /*end blink*/
                                break;

                        case 16 : /*position curser*/
                        {
				did_host_poscur = 't';
				buffer_non_protect = 't';

                                mm_read_more(2);

                                poscur(megatek_data[1] % 24,
                                  megatek_data[0] % 80);

/*
printf("\nmmfilter position curser : row = %d , col = %d\n",
	megatek_data[1] % 24,megatek_data[0] % 80);
*/

				my_test_flag = 'f';

                                break;
                        }

                        case 17 : /*dcl*/
                                break;

                        case 18 : /*scroll enable*/
                                break;

                        case 19 : /*scroll disable*/
                                break;

                        case 20 : /*start underline*/
                                break;

                        case 21 : /*end underline*/
                                break;

                        case 22 : /*syn*/
                                break;

                        case 23 : /*curser up*/
                        {
                                if(mono_row == 0)
                                        poscur(23,mono_col);
                                else
                                        poscur(mono_row - 1,mono_col);

                                break;
                        }

                        case 24 : /*curser right*/
                        {
                                if(mono_col >= 79)
                                {
                                        if(mono_row > 23)
                                                poscur(0,0);
                                        else
                                                poscur(mono_row + 1,0);
                                }
                                else
                                        poscur(mono_row,mono_col + 1);

                                break;
                        }

                        case 25 : /*curser left*/
                        {
                                if(mono_col == 0)
                                {
                                        if(mono_row == 0)
                                                poscur(23,79);
                                        else
                                                poscur(mono_row - 1,79);
                                }
                                else
                                        poscur(mono_row,mono_col - 1);
                        }

                        case 26 : /*curser down*/
                        {
                                if(mono_row >= 23)
                                        poscur(0,mono_col);
                                else
                                        poscur(mono_row + 1,mono_col);

                                break;
                        }

                        case 27 : /*start_escape*/
			{
				the_escape_handler();
                                break;
			}

                        case 28 : /*start protect*/
                        {
                                char_type = the_char;
                                break;
                        }

                        case 29 : /*end protect*/
                        {
                                char_type = the_char;
                                break;
                        }

                        case 30 : /*rs*/
                                break;

                        case 31 : /*us*/
                                break;
                }
        }
        else if(the_char == 127)  /*del key*/
        {
/*do nothing*/
        }
        else  /*display it*/
                dischr(the_char);

}

