/*October 18, 1988 9:40 Made textmode charactors twice as big*/

/*February 7, 1989 made exit graphics mode set the the color to white &
enter graphics mode set it back to the saved color*/

/*February 8, 1989 added fflush of printer during each call to scroll_func()*/

#include "my_utils.h"
#include "my_globs.h"



extern char far doing_an_alt_r;
extern char far doing_an_alt_u;
extern void far megatek_device(int);

extern int far current_color;
extern int far save_system_color;
int far save_current_color;

char far my_text_buffer[100][200];




void far clear_my_listing()
{
int row,col;


	if(num_pages == 2)
	{
        	my_set_viewport(-2048,-2048,2048,2048,which_page);
		my_lock_viewport();
	}
	else
	{
		my_set_viewport(-2048,-2048,2048,2048,0);
		my_lock_viewport();
	}


	save_current_color = current_color;
	my_color(save_system_color);

	draw_mode(3);
	my_size_chars(list_size);

	for(row = 1 ; my_text_buffer[row][0] != '\0' ; row++)
		for(col = 0 ; my_text_buffer[row][col] != '\0' ; col++)
			place_char(my_text_buffer[row][col] & 0x007f,
				(col + 1)*8*(list_size + 1),
				row*8*(list_size + 1),
				6);

	my_size_chars(0);
	draw_mode(0);
	my_color(save_current_color);


        if(num_pages == 2)
        {
        	my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
        		headers_segs[which_cseg].save_y_lower_clip,
	                headers_segs[which_cseg].save_x_upper_clip,
              		headers_segs[which_cseg].save_y_upper_clip,
		 	which_page);
        	my_lock_viewport();
    	}
        else
        {
        	my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
              		headers_segs[which_cseg].save_y_lower_clip,
              		headers_segs[which_cseg].save_x_upper_clip,
              		headers_segs[which_cseg].save_y_upper_clip,0);
        	my_lock_viewport();
        }


	for(row = 0 ; row < 100 ; row++)
		for(col = 0 ; col < 199 ; col++)
			my_text_buffer[row][col] = '\0';
}




void far l_func(sub_count,which_subr)  /* change raster color table */
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




void far cap_N_func(sub_count,which_subr) /*insert detectability*/
int *sub_count,which_subr;
{

		read_more(2);

		diag_num = 86;
	#ifdef HAVE_DIAGS
		my_diag("insert detectability\n");
	#endif
}




void far k_func(sub_count,which_subr) /*generate copy*/
int *sub_count,which_subr;
{
		read_more(1);

		diag_num = 89;
	#ifdef HAVE_DIAGS
		my_diag("generate copy\n");
	#endif
}




void far u_func()
{
	read_more(5);
#ifdef HAVE_DIAGS	
	my_diag("ITS THE UNDOCUMENTED \"U\" FUNCTION!!!!!\n");
#endif
}



 /*erase text monitor buffer*/
void far erase_func(sub_count,which_subr)
int *sub_count,which_subr;
{
	diag_num= 90;
#ifdef HAVE_DIAGS
	my_diag("erase text monitor buffer\n");
#endif



	if(doing_an_alt_r == 't')
	{
		if(doing_an_alt_u == 'f')
		{
			doing_an_alt_u = 't';
			megatek_device(4);
			doing_an_alt_u = 'f';
		}

		doing_an_alt_r = 'f';
	}




	clear_my_listing();
}



 /*scroll text monitor buffer*/
void far scroll_func(sub_count,which_subr)
int *sub_count,which_subr;
{

	diag_num = 91;
#ifdef HAVE_DIAGS
	my_diag("scroll text monitor buffer\n");
#endif

	if(text_row < 99)
	{
		my_text_buffer[text_row][text_col] = ' ';
		my_text_buffer[text_row][text_col + 1] = '\0';
	}


	text_row++;
	text_col = 0;

	if(print_list == 't')
	{
		putc('\n',out_ptr);
		fflush(out_ptr);
	}
}


 /*enter graphics mode*/
void far enter_graphics(sub_count,which_subr)
int *sub_count,which_subr;
{
int ch;

	graphics_text = GRAPHICS;
	diag_num = 92;
#ifdef HAVE_DIAGS
	my_diag("enter graphics mode\n");
#endif

	my_color(save_current_color);
        my_size_chars(0);
	my_dash_enable(4);

    if(num_pages == 2)
    {
        my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
              headers_segs[which_cseg].save_y_lower_clip,
              headers_segs[which_cseg].save_x_upper_clip,
              headers_segs[which_cseg].save_y_upper_clip,
		 which_page);
        my_lock_viewport();
    }
    else
    {
        my_set_viewport(headers_segs[which_cseg].save_x_lower_clip,
              headers_segs[which_cseg].save_y_lower_clip,
              headers_segs[which_cseg].save_x_upper_clip,
              headers_segs[which_cseg].save_y_upper_clip,0);
        my_lock_viewport();
    }
}



 /*exit graphics mode*/
void far exit_graphics(sub_count,which_subr)
int *sub_count,which_subr;
{
	graphics_text  = TEXT;
	diag_num = 93;


	    my_size_chars(list_size);

	    text_col = 0;
	    text_row = 1;

	    save_current_color = current_color;
	    my_color(save_system_color);
	    my_dash_enable(0);


	    if(num_pages == 2)
	    {
        	my_set_viewport(-2048,-2048,2048,2048,which_page);
	        my_lock_viewport();
	    }
	    else
	    {
		my_set_viewport(-2048,-2048,2048,2048,0);
		my_lock_viewport();
	    }

#ifdef HAVE_DIAGS
	my_diag("exit graphics mode\n");
#endif
}




void far delete_last_char()
{


	place_char(' ',text_col*8*(list_size + 1),
		text_row*8*(list_size + 1),6);
	if(text_col == 0)
	{
		text_col = (1024 - 16*(list_size + 1))/8/(list_size + 1);
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

	if(text_row < 99)
		my_text_buffer[text_row][text_col] = the_char;


	if(text_col == 1024/8/(list_size + 1))
	{
		text_row++;
		text_col = 0;
	}
	else
		text_col++;


	draw_mode(3);
	place_char(the_char & 0x007f,text_col*8*(list_size + 1),
		text_row*8*(list_size + 1),6);
	draw_mode(0);


	if(print_list == 't')
		putc(the_char,out_ptr);

}


