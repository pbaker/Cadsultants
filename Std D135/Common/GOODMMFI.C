/* MMFILTER.C  version of paul's mmfilter.4c for DTI project*/
#include "stdio.h"


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
                                mm_read_curser_pos();
                                poscur(mm_row_curser,mm_col_curser - 1);
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
                                clrall();
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
                                mm_read_more(2);

                                poscur(megatek_data[1] % 24,
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
                                break;

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
