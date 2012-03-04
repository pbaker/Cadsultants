/* MMFILTER.C  version of paul's mmfilter.4c for DTI project*/


#include "stdio.h"



#define VT100_MODE  0
#define MESS_MON  1



extern int far the_protect_color;
extern int far the_non_protect_color;
extern int far the_light_color;
extern int far the_vt100_color;



extern void far display_host_char(int);



extern void far alt_functions();
extern void far which_icu();
extern void far host_escape(int);
extern int far exit_flag;


extern void far host_clrall(void);
extern void far host_poscur(int,int);

extern void far read_curser_pos(void);
extern void far my_pckb(void);
extern int far read_host_buffer(void);
extern void far poscur(int,int);
extern void far clrnon(void);
extern void far clrall(void);
extern void far dischr(int);
extern char far char_type;
extern char far page_for_write;
extern unsigned char far megatek_data[];
extern int far row_curser;
extern int far col_curser;


extern int far vt100_mess_mon_flag;
extern char far vt100_attribute;


int far mm_row_curser,far mm_col_curser;
int far mm_row_current_pos,far mm_col_current_pos;



void far mm_read_more(number)
int number;
{
int i,j,ch;

        for(i = 0 ; i < number ; i++)
        {
                ch = read_host_buffer();

                while(ch == -1)
                {
                        for(j = 0 ; j < 10000 ; j++)
                                ;

                        my_pckb();

                        ch = read_host_buffer();
                }

                megatek_data[i] = ch & 0xff;
        }
}




void far mm_read_curser_pos()
{
        read_curser_pos();   /*my bios call*/
        mm_row_curser = row_curser;
        mm_col_curser = col_curser;
}



void far the_escape_handler()
{
int ch;

	ch = read_host_buffer();
	while(ch == -1)
		ch = read_host_buffer();


	switch(ch)
	{
		case '0' :
			return;

		case '1' :
		{
			exit_flag = 1;
			return;
		}

		case '2' :
		{
			vt100_mess_mon_flag = MESS_MON;

			alt_functions();

			vt100_attribute = the_non_protect_color;

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




void far mm_filter(the_char)
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
				vt100_mess_mon_flag = MESS_MON;

                                clrnon();

				vt100_attribute = the_non_protect_color;

                                break;
                        }

                        case 7 : /*alert*/
                        {
                                putchar(7);
                                break;
                        }

                        case 8 : /*bs*/
                        {
			/*
                                mm_read_curser_pos();
                                poscur(mm_row_curser,mm_col_curser - 1);
			*/
				display_host_char(8);

                                break;
                        }

                        case 9 : /*ht*/
                                break;


                        case 10 : /*new line*/
                        {
			/*
                                dischr(10);
                                dischr(13);
			*/

				display_host_char(10);

                                break;
                        }


                        case 11 : /*erase to eol*/
                                break;

                        case 12 : /*clear all*/
                        {
                                host_clrall();

				vt100_attribute = the_non_protect_color;

                                break;
                        }


                        case 13 : /*carriage return*/
                        {

                        /*
			        dischr(13);
			*/
				display_host_char(13);

                                break;
                        }


                        case 14 : /*start blink*/
			{
				display_host_char(14);
                                break;
			}


                        case 15 : /*end blink*/
			{
				display_host_char(15);
                                break;
			}


                        case 16 : /*position curser*/
                        {
                                mm_read_more(2);

                                host_poscur(megatek_data[1] % 24,
                                        megatek_data[0] % 80);
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
                                mm_read_curser_pos();

                                if(mm_row_curser == 0)
                                        poscur(23,mm_col_curser);
                                else
                                        poscur(mm_row_curser - 1,mm_col_curser);

                                break;
                        }

                        case 24 : /*curser right*/
                        {
                                mm_read_curser_pos();

                                if(mm_col_curser >= 79)
                                {
                                        if(mm_row_curser > 23)
                                                poscur(0,0);
                                        else
                                                poscur(mm_row_curser + 1,0);
                                }
                                else
                                        poscur(mm_row_curser,mm_col_curser + 1);

                                break;
                        }

                        case 25 : /*curser left*/
                        {
                                mm_read_curser_pos();

                                if(mm_col_curser == 0)
                                {
                                        if(mm_row_curser == 0)
                                                poscur(23,79);
                                        else
                                                poscur(mm_row_curser - 1,79);
                                }
                                else
                                        poscur(mm_row_curser,mm_col_curser - 1);
                        }

                        case 26 : /*curser down*/
                        {
                                mm_read_curser_pos();

                                if(mm_row_curser >= 23)
                                        poscur(0,mm_col_curser);
                                else
                                        poscur(mm_row_curser + 1,mm_col_curser);

                                break;
                        }

                        case 27 : /*start_escape*/
			{
				the_escape_handler();
                                break;
			}


                        case 28 : /*start protect*/
                        {
				vt100_attribute = the_protect_color;
                                char_type = the_char;
                                break;
                        }

                        case 29 : /*end protect*/
                        {
				vt100_attribute = the_non_protect_color;
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
		display_host_char(the_char);


/*
                dischr(the_char);
*/

}
