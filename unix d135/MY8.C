/* MY8.C  text mode commands, etc */


/*October 18, 1988 9:40 Made textmode charactors twice as big*/

/*February 7, 1989 made exit graphics mode set the the color to white &
enter graphics mode set it back to the saved color*/

/*February 8, 1989 added fflush of printer during each call to scroll_func()*/

/*March 20, 1989 removed calls to my_set_viewport*/
/*September 11, 1989 uncommented calls to my_set_viewport*/


#include "my_utils.h"
#include "my_globs.h"
#include <pw_lib.h>


extern FILE  *listing_output_file;

extern char  printer_listings_use_cr;
extern char  display_list_enabled;
extern char  doing_an_alt_r;
extern char  doing_an_alt_u;
extern void  megatek_device();
extern int  current_color;
extern int  save_system_color;

int  save_current_color;
char  pixel_listing_buffer[200] = "\0";
short  text_monitor_dl[22222];
short  *text_dl_pointer = &text_monitor_dl[0];



void  clear_text_monitor_dl()
{
	text_dl_pointer = &text_monitor_dl[0];
	pw_dl_init(text_dl_pointer,0,NULL,0);
	dlend();
	pw_dl_init(NULL,0,NULL,0);
}




void  l_func(sub_count,which_subr)  /* change raster color table */
int *sub_count,which_subr;
{
		read_more(6);

		diag_num = 85;

		megatek_data[1] &= 0x0f;
		megatek_data[2] = (megatek_data[2] >> 1) & 0x0f;
		megatek_data[3] = (megatek_data[3] >> 1) & 0x0f;
		megatek_data[4] = (megatek_data[4] >> 1) & 0x0f;

		color_reg(megatek_data[1],(char)(megatek_data[4] * 4),
			(char)(megatek_data[3] * 4),
			(char)(megatek_data[2] * 4));	
#ifdef HAVE_DIAGS
		sprintf(diag_string,"change color table %d %d %d %d\n",
			(int)megatek_data[1],(int)megatek_data[2],(int)megatek_data[3],
			(int)megatek_data[4]);
		my_diag(diag_string);
#endif
}




void  cap_N_func(sub_count,which_subr) /*insert detectability*/
int *sub_count,which_subr;
{

		read_more(2);

		diag_num = 86;
#ifdef HAVE_DIAGS
		my_diag("insert detectability\n");
#endif
}




void  k_func(sub_count,which_subr) /*generate copy*/
int *sub_count,which_subr;
{
		read_more(1);

		diag_num = 89;
#ifdef HAVE_DIAGS
		my_diag("generate copy\n");
#endif
}




void  u_func()
{
	read_more(5);
#ifdef HAVE_DIAGS	
	my_diag("ITS THE UNDOCUMENTED \"U\" FUNCTION!!!!!\n");
#endif
}



 /*erase text monitor buffer*/
void  erase_func(sub_count,which_subr)
int *sub_count,which_subr;
{
	diag_num= 90;
#ifdef HAVE_DIAGS
	my_diag("erase text monitor buffer\n");
#endif

/*
printf("\nerase_func()\n");
*/


	if(doing_an_alt_r == 't')
	{
		if(doing_an_alt_u == 'f')
		{
			if(display_list_enabled == 't')
			{
				doing_an_alt_u = 't';
				megatek_device(4);
				doing_an_alt_u = 'f';
			}
		}

		doing_an_alt_r = 'f';
	}


	pw_dl_init(text_dl_pointer,0,NULL,0);
	dlend();
	pw_dl_init(NULL,0,NULL,0);

        my_set_viewport(-2048,-2048,2048,2048,which_page);
	my_size_chars(list_size);

	color((save_system_color << 4) + save_system_color);
	linpat(0);
	draw_mode(3);
	dlrun(&text_monitor_dl[0]);
	draw_mode(0);
	linpat(1);

	clear_text_monitor_dl();
}



 /*scroll text monitor buffer*/
void  scroll_func(sub_count,which_subr)
int *sub_count,which_subr;
{
	diag_num = 91;
#ifdef HAVE_DIAGS
	my_diag("scroll text monitor buffer\n");
#endif

/*
printf("\nscroll_func\n");
*/

		my_set_viewport(-2048,-2048,2048,2048,0);
		color((save_system_color << 4) + save_system_color);
		pixel_listing_buffer[text_col] = '\0';
		text_row++;
		text_col = 0;
		draw_mode(3);
		move2(-2000,2048 - text_row*60*(list_size + 1));
		gtext(pixel_listing_buffer);
		draw_mode(0);

		pw_dl_init(text_dl_pointer,0,NULL,0);
		color((save_system_color << 4) + save_system_color);
		move2(-2000,2048 - text_row*60*(list_size + 1));
		gtext(pixel_listing_buffer);
		pw_dl_rdptr((unsigned short **)&text_dl_pointer);
		pw_dl_init(NULL,0,NULL,0);

		pixel_listing_buffer[0] = '\0';



		if(listing_output_file != NULL)
			putc('\n',listing_output_file);

/*
		if(print_list == 't')
		{
			putc(10,stdprn);

			if(printer_listings_use_cr == 't')
				putc(13,stdprn);

			fflush(stdprn);
		}
*/
}



 /*enter graphics mode*/
void  enter_graphics(sub_count,which_subr)
int *sub_count,which_subr;
{
int ch;

	graphics_text = GRAPHICS;
	diag_num = 92;
#ifdef HAVE_DIAGS
	my_diag("enter graphics mode\n");
#endif

/*
printf("\nenter graphics mode\n");
*/

	my_color(save_current_color);
        my_size_chars(0);
	my_dash_enable(4);
}



 /*exit graphics mode*/
void  exit_graphics(sub_count,which_subr)
int *sub_count,which_subr;
{
/*
printf("\nexit graphics mode\n");
*/


	graphics_text  = TEXT;
	diag_num = 93;


	    my_size_chars(list_size);
	    text_col = 0;
	    text_row = 1;
	    save_current_color = current_color;
	    my_color(save_system_color);
	    my_dash_enable(0);

            my_set_viewport(-2048,-2048,2048,2048,which_page);


#ifdef HAVE_DIAGS
	my_diag("exit graphics mode\n");
#endif
}




void  delete_last_char()
{
/*
printf("\nDELETE LAST LISTING CHAR\n");
*/

	place_char(' ',text_col*32*(list_size + 1) - 2048,
		2048 - text_row*50*(list_size + 1),6);

	if(text_col == 0)
	{
		text_col = 126/(list_size + 1);
		text_row--;
	}
	else
	{
		text_col--;
	}
}



void print_char(the_char)
int the_char;
{

	the_char = the_char & 0x7f;
/*
printf("%c",the_char);
*/

	if((the_char >= 32) && (the_char < 128))
		pixel_listing_buffer[text_col] = the_char;
	else
		pixel_listing_buffer[text_col] = ' ';
	

	text_col++;

/*
	if(print_list == 't')
		putc(the_char,stdprn);
*/

	if(listing_output_file != NULL)
		putc(the_char,listing_output_file);
}

