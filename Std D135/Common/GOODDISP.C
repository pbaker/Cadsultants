/* DISPLAY.C version of paul's display.4c */


#include "dos.h"   /*for int86() & union REGS definition*/


extern void far goto_row_col(int,int);
extern void far turn_off_curser(void);
extern void far turn_on_curser(void);
extern int far mm_row_current_pos;
extern int far mm_col_current_pos;
extern int far mm_row_curser;
extern int far mm_col_curser;
extern union REGS far my_input_regs;
extern union REGS far my_output_regs;


#define dim_chr          28
#define bright_chr       29
#define space            20
#define cr               13
#define lf               10
#define bs               8
#define bel              7


char far char_type = 29;
char far page_for_write = 0;
char far page_for_display = 0;
char far bright_attribute = 15;
char far lite_attribute = 7;
char far dim_attribute = 7;
char far menu_attribute = 7;


void far mm_goto_row_col(row,col)
int row,col;
{
        mm_row_current_pos = row;
        mm_col_current_pos = col;
        goto_row_col(row,col);
}


void far mm_turn_off_curser()
{
        turn_off_curser();
}


void far mm_turn_on_curser()
{
        turn_on_curser();
}



void far poscur(row,col)
int row,col;
{
        mm_turn_off_curser();
        mm_goto_row_col(row,col);
        mm_turn_on_curser();
}



void far clrall()
{
        poscur(0,0);

        my_input_regs.h.ah = 6;  /*scroll up bios routine*/

        my_input_regs.h.al = 0;
        my_input_regs.h.bh = bright_attribute;
        my_input_regs.h.ch = 0;
        my_input_regs.h.cl = 0;
        my_input_regs.h.dh = 23;
        my_input_regs.h.dl = 79;

        int86(0x10,&my_input_regs,&my_output_regs);
}



void far clrnon()
{
        poscur(0,0);

        my_input_regs.h.ah = 6;   /*scroll up bios routine*/

        my_input_regs.h.al = 0;
        my_input_regs.h.bh = bright_attribute;
        my_input_regs.h.ch = 0;
        my_input_regs.h.cl = 0;
        my_input_regs.h.dh = 17;
        my_input_regs.h.dl = 56;

        int86(0x10,&my_input_regs,&my_output_regs);
}



void far mminit()
{
        my_input_regs.h.ah = 0;    /*set display mode bios call*/
        my_input_regs.h.al = 3;
        int86(0x10,&my_input_regs,&my_output_regs);

        clrall();

        char_type = bright_chr;
        page_for_write = 0;
}




void far new_page()
{

}




void far dischr(the_char)
int the_char;
{
int bl;

        the_char &= 0xff;

        if(char_type != dim_chr)
        {
bright:
                bl = 0;
/* no changing display pages in my version!!!!!!!!! */

                mm_read_curser_pos();

                if(mm_row_curser >= 24)    /*scroll it*/
                {
                        my_input_regs.h.ah = 6;  /*scroll up bios routine*/

                        my_input_regs.h.al = 1;
                        my_input_regs.x.cx = 0;
                        my_input_regs.h.dh = 23;
                        my_input_regs.h.dl = 79;
                        my_input_regs.h.bh = bright_attribute;

                        int86(0x10,&my_input_regs,&my_output_regs);

                        mm_goto_row_col(mm_row_curser - 1,mm_col_curser);

                        goto no_scroll;

                }
                else    /*no scrolling*/
                {
        no_scroll:

	                turn_off_curser();  /*just added this*/

        	        my_input_regs.h.ah = 14;  /*teletype mode bios call*/
	                my_input_regs.h.al = the_char;
                	my_input_regs.h.bl = bright_attribute;
        	        my_input_regs.h.bh = 0;

	                int86(0x10,&my_input_regs,&my_output_regs);

        	        mm_read_curser_pos();

                	turn_on_curser();  /*just added this*/
                }
        }
        else   /*its a dim character*/
        {
                switch(the_char)
                {
                        case space : 
                                goto bright;

                        case cr :
                                goto bright;

                        case lf :
                                goto bright;

                        case bs : 
                                goto bright;

                        case bel :
                                goto bright;
                }

                turn_off_curser();

                my_input_regs.h.ah = 9;  /*write character and attribute*/
                my_input_regs.h.al = the_char;
                my_input_regs.h.bl = dim_attribute;
                my_input_regs.h.bh = 0;
                my_input_regs.x.cx = 1;

                int86(0x10,&my_input_regs,&my_output_regs);

                mm_read_curser_pos();

                if(mm_col_curser == 79)
                        poscur(mm_row_curser + 1,0);
                else
                        poscur(mm_row_curser,mm_col_curser + 1);

                turn_on_curser();
        }
}