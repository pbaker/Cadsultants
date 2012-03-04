/* MY7.C  Host segment commands, Host subroutine commands, redisplay stuff */



/*October 18, 1988 11:00 made megatek_update and matrix_update only switch
pages when num_pages == 2*/

/*October 20, 1988 changed open segment so it copies the header data from
segment 0's file to the header of the new file, same with clear segment,
added header in-the-file commands to interp_seg_command(), and removed
header array matrix to hcrst copying from display_segs() and display_a_seg()*/

/*October 21, 1988 made void  get_next_non44() to skip over "words" with
an op code of 44 and made interp_seg_command() us it instead of get_our_value()
*/

/*November 1, 1988 made show time taken by redisplay (display_segs()) use
my bios routines for positioning of the message*/

/*November 2, 1988 made display_segs() (when should display time) put up
"TIMER" in blink mode and then "%d Seconds" in normal mode*/

/*November 3, 1988 replaced    if(kbhit() != 0) 
			       {
				   .
				   .
				   .
				   .
			       }

in display_segs() with a call to my_pckb() */

/*November 8, 1988 added curser undraw/redraw code to megatek_update()*/

/*November 15, 1988 made changes to use generic graphics board calls*/

/*January 31, 1989 made changes to send the error messages to the error file*/

/*February 8, 1989 changed interp_seg_command() so the character string part
can handle an empty last word*/

/*February 24, 1989 fixed character string case in interp_seg_command()
so my_x gets incremented for each call to my_char()*/

/*February 24, 1989 added code to call sub to look for rewrite of sub call
over a color*/

/*March 22, 1989 fixed call to my_set_viewport() in megatek_update so it uses
the right variables*/


/*March 22, 1989 put rdmask() and wrmask() into matrix_update() and
megatek_update() to switch bit planes when redisplaying*/

/*March 23, 1989 fixed close segment so it only adds the end_of_segment_interrupt
when no segment rewrite happened before closing*/

/*March 24, 1989 added stuff to use non-interpreted subroutines*/

/*March 27, 1989 added set viewing matrix to unity in display_segs() at the
start of each segment*/

/*March 28, 1989 added code to execute the change-something header when
needed (open segment & display_segs() )*/


/*March 30, 1989 added code to set up the pixel board matrix
during append segment & created display_part_seg() code for doing partial
redisplays after segment rewrites*/


/*March 31, 1989 d:\newpixel\my7v13.c added code to close segment & append
segment to call display_part_seg() after a segment rewrite*/



#include "my_utils.h"
#include "my_globs.h"
#include <time.h>
#include "expanded.h"
#include <pw_lib.h>
#include "mywindows.h"



#define BLACK_SUB0 0
#define BLACK_SUB1 1



#define HARDWARE_EMS 0
#define SOFTWARE_EMS 1
#define TIME_OUT 0
#define SUCCESS 1

#define OLD_X_Y  0
#define NEW_X_Y  1

#define GRAPHICS_EDGE_OFFSET 20


extern char *blt_page_frame_ptr;
extern void partial_map_handle();


extern int last_map_handle_page;

extern char do_not_dlrun_flag;

extern int  graphics_window_x1,  graphics_window_x2,
  graphics_window_y1,  graphics_window_y2;


extern char  do_ugouts_only;
extern char  is_there_graphics;

extern void  change_page_gmark_undraw();
extern void  clear_graphics_window();
extern void  my_flood();

extern int  my_pw_dl_wait();
extern void  clear_text_monitor_dl();
extern void  display_the_curser();
extern OPCODE  unity_viewing_matrix[];
extern long  rw_position_in_file;
extern int  rw_file_number;

/*for rewrite with sub call test*/
extern int  op_code_to_overwrite;
extern long  save_host_id;
extern long  save_host_displacement;

/*tells for each segment, its current change of visibility state.
     used by a_visibility_switched_to_on() */
extern char  visibility_changed_to_on[];

extern char  doing_an_alt_u;
extern short  *start_of_page_frame_0;
extern char  no_more_ems_pages;

extern void  flood_draw_gmark();



int my_interrupt_reason = -1;


char  ems_mode = SOFTWARE_EMS;
OPCODE  io_masks[2] = {0x0f,0xf0};
short  temp2_dl[50];
char  a_segment_deleted[NUMBER_OF_SEGS];
char  a_segment_cleared[NUMBER_OF_SEGS];

int  rw_threshold = 40;
long  megatek_rw_positions[555];
int  rw_segment[555];
char  black_flags[555];
int  rw_counter = 0;
int  last_subroutine_run = 999;
char  the_flood_flag = 'f';
char  stop_after_matrix_sub = 'f';





char  flood_flag_update()
{
int i,visi_count;


	visi_count = 0;

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if((segment_visibility[i] == 't') ||
			(visibility_changed_to_on[i] == 't'))
				visi_count++;


	if(visi_count > 2)    /* more than 1 view is visible */
		return('t');
	else
		return('f');

}





void  reason_message(the_reason,where_called,page_you_think)
int the_reason;
char  where_called[];
int page_you_think;
{
/*
	sprintf(diag_string,"%s   Reason = %d   think the page is  %d\n",
		where_called,the_reason,page_you_think);
	error_logger(diag_string);

	sprintf(diag_string,"But its Really page %d %d %d\n",
		software_ems_buffer[8187],
		software_ems_buffer[8188],
		software_ems_buffer[8189]);
	error_logger(diag_string);
*/
}





void  make_our_subs()
{
int i,temp;

/* called in main() at start up (my2funcs.c) */

/*initialize "megatek" sub calling table*/
	for(i = 0 ; i < 64 ; i++)
	{
		pw_dl_init(&(sub_struct_ptr ->
			sub_calling_table[i][0]),0,NULL,0);
		ret();
		pw_dl_init(NULL,0,NULL,0);
	}


/*make our "black" version of sub 0 (NEEDED for partials in BLACK)*/

	temp = return_window_color(GRAPHICS_WINDOW,BACKGROUND);

	pw_dl_init(&(sub_struct_ptr ->
		our_versions_of_subs[BLACK_SUB0][0]),0,NULL,0);
	color((temp << 4) | temp);
	ret();
	pw_dl_init(NULL,0,NULL,0);

/*make our "black" version of sub 1*/

	pw_dl_init(&(sub_struct_ptr ->
		our_versions_of_subs[BLACK_SUB1][0]),0,NULL,0);
	color((temp << 4) | temp);
	linpat(1);
	slpat32(1,0xffffffff);
	ret();
	pw_dl_init(NULL,0,NULL,0);


/*make our page 1 version of sub 1*/

	pw_dl_init(&(sub_struct_ptr ->
		our_versions_of_subs[PAGE1_SUB1][0]),0,NULL,0);
	wrmask(0x0f);
	linpat(1);
	slpat32(1,0xffffffff);
	ret();
	pw_dl_init(NULL,0,NULL,0);

/*make our page 2 version of sub 1*/

	pw_dl_init(&(sub_struct_ptr ->
		our_versions_of_subs[PAGE2_SUB1][0]),0,NULL,0);
	wrmask(0xf0);
	linpat(1);
	slpat32(1,0xffffffff);
	ret();
	pw_dl_init(NULL,0,NULL,0);

/*make our no_draw_sub 1*/

	pw_dl_init(&(sub_struct_ptr ->
		our_versions_of_subs[NO_DRAW_SUB1][0]),0,NULL,0);
	wrmask(0);
	linpat(1);
	slpat32(1,0xffffffff);
	ret();
	pw_dl_init(NULL,0,NULL,0);

}





void  initialize_sub_wrmask()
{
	if(sub_nums[which_sub] == 0)  /*turn off drawing*/
	{
		pw_dl_init(&(sub_struct_ptr ->
			sub_display_lists[0][0]),0,NULL,0);
		wrmask(0);
		pw_dl_rdptr((unsigned short **)&current_sub_pointer);
		pw_dl_init(NULL,0,NULL,0);
	}
	else if(sub_nums[which_sub] == 1)  /*which page mask*/
	{
		pw_dl_init(&(sub_struct_ptr ->
			sub_display_lists[1][0]),0,NULL,0);
		wrmask(0x0f);
		pw_dl_rdptr((unsigned short **)&current_sub_pointer);
		pw_dl_init(NULL,0,NULL,0);
	}
	else if(sub_nums[which_sub] == 2)  /*which page mask*/
	{
		pw_dl_init(&(sub_struct_ptr ->
			sub_display_lists[2][0]),0,NULL,0);
		wrmask(0x0f);
		pw_dl_rdptr((unsigned short **)&current_sub_pointer);
		pw_dl_init(NULL,0,NULL,0);
	}
	else if(sub_nums[which_sub] == 5)  /*which page mask*/
	{
		pw_dl_init(&(sub_struct_ptr ->
			sub_display_lists[5][0]),0,NULL,0);
		wrmask(0x0f);
		pw_dl_rdptr((unsigned short **)&current_sub_pointer);
		pw_dl_init(NULL,0,NULL,0);
	}
	else
		current_sub_pointer = &(sub_struct_ptr -> sub_display_lists[
			sub_nums[which_sub]][0]);
}



void  set_sub_calling_table()
{
	if((a_subroutine == 3) || (a_subroutine == 6) ||
		(a_subroutine == 7))
	{
	/* its an interpreted sub */
		pw_dl_init(&(sub_struct_ptr ->
			sub_calling_table[a_subroutine][0]),0,NULL,0);
		dlint((short)(INTERP_A_SUB_TIME + a_subroutine));
		ret();
		pw_dl_init(NULL,0,NULL,0);
	}
	else  /*its a non-interpreted sub */
	{
		pw_dl_init(&(sub_struct_ptr ->
			sub_calling_table[a_subroutine][0]),0,NULL,0);
		call(&(sub_struct_ptr -> sub_display_lists[a_subroutine][0]));
		ret();
		pw_dl_init(NULL,0,NULL,0);
	}
}






void  install_black_sub0()
{
	pw_dl_init(&(sub_struct_ptr -> sub_calling_table[0][0]),0,NULL,0);
	call(&(sub_struct_ptr -> our_versions_of_subs[BLACK_SUB0][0]));
	pw_dl_init(NULL,0,NULL,0);
}




void  restore_host_sub0()
{
	pw_dl_init(&(sub_struct_ptr -> sub_calling_table[0][0]),0,NULL,0);
	call(&(sub_struct_ptr -> sub_display_lists[0][0]));
	pw_dl_init(NULL,0,NULL,0);
}





void  black_out_colors()
{
int row,temp;

	temp = return_window_color(GRAPHICS_WINDOW,BACKGROUND);


	for(row = 0 ; row < 20 ; row++)
	{
		pw_dl_init(&(sub_struct_ptr ->
			color_sub_table[row][0]),0,NULL,0);
		color((temp << 4) | temp);
		ret();
		pw_dl_init(NULL,0,NULL,0);
	}
}



void  restore_colors()
{
int row;

	for(row = 0 ; row < 16 ; row++)
	{
		pw_dl_init(&(sub_struct_ptr -> 
			color_sub_table[row][0]),0,NULL,0);
		color(row | (row << 4));
		ret();
		pw_dl_init(NULL,0,NULL,0);
	}
}



char  segment_deleted_update()
{
char flag;
int i;

	flag = 'f';

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if(a_segment_deleted[i] == 't')
			flag = 't';

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		a_segment_deleted[i] = 'f';

	return(flag);
}





char  a_visibility_switched_to_on()
{
int i;
char flag;

	flag = 'f';

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if(visibility_changed_to_on[i] == 't')
		{
			flag = 't';
			segment_visibility[i] = 't';
		}


	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		visibility_changed_to_on[i] = 'f';

	return(flag);
}







/*called in open segment, close segment, append segment, rewrite segment,
  and every time in insert_ours*/
void  update_current_segments_end()
{
	if(current_word[current_logical_page] + current_offset 
		 > last_word[which_cseg])
	{
		last_word[which_cseg] = current_word[current_logical_page] +
			current_offset;

		end_segment_ptrs[which_cseg].logical_page_num = 
			current_logical_page;
		end_segment_ptrs[which_cseg].offset = current_offset;

		pw_dl_init(current_pointer_to_dl,0,NULL,0);
		dlint((short)(END_OF_SEGMENT_INTERRUPT + which_cseg));
		pw_dl_init(NULL,0,NULL,0);
	}

}



void  update_last_word() 
{
	if(current_word[current_logical_page] + current_offset 
		 > last_word[which_cseg])
		last_word[which_cseg] = current_word[current_logical_page] +
			current_offset;
}





void  do_the_partials()
{
int save_which_cseg,i;

	save_which_cseg = which_cseg;


	save_map_info();

/*
printf("\nsave_map_info()  was called here\n");
*/

	for(i = 0 ; i < rw_counter ; i++)
	{
		rw_position_in_file = megatek_rw_positions[i];
		black = black_flags[i];

		if(which_cseg != rw_segment[i])  /*switch megatek files*/
			which_cseg = rw_segment[i];


		display_part_seg();
	}

	rw_counter = 0;
	which_cseg = save_which_cseg;
	black = 'f';


	restore_map_info();

/*
printf("\n restore_map_info() was called here\n");
*/
}





void  set_sub_wrmasks()
{
/*put correct call of our sub 1 into sub calling table*/
	    pw_dl_init(&(sub_struct_ptr -> sub_calling_table[1][0]),0,NULL,0);

	    if(which_page == 0)
		call(&(sub_struct_ptr -> our_versions_of_subs[PAGE1_SUB1][0]));
	    else
		call(&(sub_struct_ptr -> our_versions_of_subs[PAGE2_SUB1][0]));

	    pw_dl_init(NULL,0,NULL,0);
			


/*put correct write mask into sub 2*/
	    pw_dl_init(&(sub_struct_ptr -> sub_display_lists[2][0]),0,NULL,0);

	    if(which_page == 0)
		    wrmask(0x0f);
	    else
		    wrmask(0xf0);

	    pw_dl_init(NULL,0,NULL,0);


/*put correct write mask into sub 5*/
	    pw_dl_init(&(sub_struct_ptr -> sub_display_lists[5][0]),0,NULL,0);

	    if(which_page == 0)
		    wrmask(0x0f);
	    else
		    wrmask(0xf0);

	    pw_dl_init(NULL,0,NULL,0);

}







void  matrix_update()
{
int save_which_cseg,my_pause,save_x_origin,save_y_origin,save_z_origin;


	if(do_not_dlrun_flag == 't')
		return;


/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


        if(ins_matrix_flag == 'f')
                button1 = 'f';




	if(ins_matrix_flag == 't')
	{

/*
fprintf(stdprn,"Matrix_update() UPDATES\n");
fflush(stdprn);
*/


  /* turn on segment_visibility[i] so display_segs() sees the changes */
	    a_visibility_switched_to_on();


	    my_size_chars(0);
	    clear_text_monitor_dl();
            save_which_cseg = which_cseg;

	    if(rotate_mode == 'm')
	    {
		 if(curser_displayed == 't')
			display_the_curser(OLD_X_Y);
	    }


            if((rotate_mode == 'a') && (num_pages == 2))
 	    {
		if((old_x < graphics_window_x1) || (old_x > graphics_window_x2
		   - GRAPHICS_EDGE_OFFSET) || (old_y < graphics_window_y1
			+ GRAPHICS_EDGE_OFFSET*2) ||
			(old_y > graphics_window_y2))
				change_page_gmark_undraw();

		which_page = !which_page;
	    }	
		 

	    my_set_viewport(-2048,-2048,2048,2048,which_page);


            if(num_pages == 2)
            {
                if(rotate_mode == 'a')
                {
			wrmask(io_masks[which_page]);
	        	clear_graphics_window();
                }


                if(rotate_mode == 'm')
                {
			wrmask(io_masks[which_page]);
	        	clear_graphics_window();
			rdmask(io_masks[which_page]);
                }
           
            }
            else
            {
			wrmask(io_masks[0]);
			rdmask(io_masks[0]);
                	clear_graphics_window();
	    }


	    if(curser_displayed == 't')
		  display_the_curser(OLD_X_Y);


	    set_sub_wrmasks();

	    display_segs();


            if(num_pages == 2)
            {
                if(rotate_mode == 'a')
		{
			rdmask(io_masks[which_page]);
			change_page_gmark_undraw();
		}
            }
            else
		rdmask(io_masks[0]);


		which_redis = 3;
                button1 = 'f';
                which_cseg = save_which_cseg;





 /* ADDED 2/7/90 to prevent extra redisplays Hopefully (NO EFFECT) */
		segment_deleted_update();  




		ins_matrix_flag = 'f';
		the_flood_flag = 'f';
        }

}








void  megatek_update()
{
int save_which_cseg;


	if(do_not_dlrun_flag == 't')
		return;


	if(update_level > 0)
		return;

	update_level++;


/*
	while(pw_dl_busy() == TRUE)
		;
*/
	pw_dl_wait();


/*
printf("MEGATEK_UPDATE() Entered\n");
*/


	if(      (ins_matrix_flag == 't') ||
		 (a_visibility_switched_to_on() == 't') ||

/*
		 (segment_deleted_update() == 't') || 
		 ((the_flood_flag == 't') && (flood_flag_update() == 't')) || 
*/

		 ((rw_threshold > 0) && (rw_counter >= rw_threshold))
	  )

	{


/*
fprintf(stdprn,"Megatek_update()  UPDATES\n");
fflush(stdprn);
*/

		rw_counter = 0;
		my_size_chars(0);
		clear_text_monitor_dl();
		save_which_cseg = which_cseg;


		if(rotate_mode == 'm')
		{ 
		        if(curser_displayed == 't')
				display_the_curser(OLD_X_Y);
		}


		if(num_pages == 2)
		{
		if((old_x < graphics_window_x1) || (old_x > graphics_window_x2
		   - GRAPHICS_EDGE_OFFSET) || (old_y < graphics_window_y1
			+ GRAPHICS_EDGE_OFFSET*2) ||
			(old_y > graphics_window_y2))
				change_page_gmark_undraw();

			which_page = !which_page;
		}


	     my_set_viewport(-2048,-2048,2048,2048,which_page);

	     if(num_pages == 2)
	     {
		if(rotate_mode == 'a')
		{
			wrmask(io_masks[which_page]);
			clear_graphics_window();
		}
	
		if(rotate_mode == 'm')
		{
			wrmask(io_masks[which_page]);
			clear_graphics_window();  	/*both pages*/
			rdmask(io_masks[which_page]);
		}
	    }
	    else
	    {
			wrmask(io_masks[0]);
			rdmask(io_masks[0]);
			clear_graphics_window();
	    }



	    if(curser_displayed == 't')
		   display_the_curser(OLD_X_Y);


	    set_sub_wrmasks();

	    display_segs();

	    if(num_pages == 2)
	    {
		if(rotate_mode == 'a')
		{
			rdmask(io_masks[which_page]);
			change_page_gmark_undraw();
		}
	    }
	    else
		rdmask(io_masks[0]);


		which_redis = 3;
		ins_matrix_flag = 'f';
		which_cseg = save_which_cseg;

		rw_counter = 0;
	}
	else if(rw_threshold > 0)
	{
		if((rw_counter > 0) && (rw_counter < rw_threshold))
			do_the_partials();

		rw_counter = 0;
	}



	rw_counter = 0;
	ins_matrix_flag = 'f';
	the_flood_flag = 'f';

	update_level--;
}






void  cap_D_func(sub_count,which_subr)  /* close segment */
int *sub_count,which_subr;
{
	is_there_graphics = 't';
	restore_graphics_if_needed();


	
		diag_num = 72;
#ifdef HAVE_DIAGS
		my_diag("close segment\n");
#endif


		update_current_segments_end();  /*before closing it out*/
		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;


		is_a_seg_open = 'f';

		if(rewrite_flag == 't')
		{
			rewrite_flag = 'f';

                        if((rw_sub1_0_flag == 't') || (rewrite_color == 't'))
			{
				if(rw_threshold == 0)
				{
					display_part_seg();
					rw_counter = 0;
					save_map_handle_page = 'f';
				/*
					last_map_handle_page = 50;
				*/
				}
				else
					save_map_handle_page = 't';


				rw_sub1_0_flag = 'x';
                                rewrite_color = 'f';
				num_op_code_4s = 0;
				black = 'f';
			}
			else
				save_map_handle_page = 't';
		}  
		else   /*else not closing after a rewrite (put on end marker)*/
		{
			pw_dl_init(current_pointer_to_dl,0,NULL,0);
			dlint((short)(END_OF_SEGMENT_INTERRUPT + which_cseg));
			pw_dl_init(NULL,0,NULL,0);

			save_map_handle_page = 't';
		}

         	rw_sub1_0_flag = 'f';


		current_logical_page = end_segment_ptrs[0].logical_page_num;
		current_offset = end_segment_ptrs[0].offset;

		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);
		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;


		which_cseg = 0;
		goto_megatek_end();
}




void  cap_C_func(sub_count,which_subr)  /* delete segment */
int *sub_count,which_subr;
{
int i,j,k;
long num_words_seg,num_blocks_in_seg;
int save_which_cseg,temp;


	is_there_graphics = 't';
	restore_graphics_if_needed();

		read_more(2);

		diag_num = 73;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"delete segment %d\n",a_segment);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in delete segment\n",
				a_segment);
			error_logger(diag_string);
			return;
		}

		pw_dl_init(NULL,0,NULL,0);  /*leave pixel insert mode*/

		change_page_gmark_undraw();

		my_set_viewport(-2048,-2048,2048,2048,0);
		vwmatx3(&unity_viewing_matrix[0]);

		black_out_colors();
		stop_after_matrix_sub = 't';
		display_a_seg(i);
		stop_after_matrix_sub = 'f';
		restore_colors();


		temp = return_window_color(GRAPHICS_WINDOW,BACKGROUND);


		if(i == 3)   /* could be a drag segment */
		{
			if(megatek_ends[4] > 4L)  /* segment 4 exists */
			{
				my_flood((temp << 4) | temp);

				change_page_gmark_undraw();
		
				my_set_viewport(-2048,-2048,2048,2048,0);

				black_out_colors();
				stop_after_matrix_sub = 't';
				display_a_seg(i);
				stop_after_matrix_sub = 'f';
				restore_colors();

			}
			else	  /* IT IS a Drag Segment */
			{
				change_page_gmark_undraw();

				my_set_viewport(-2048,-2048,2048,2048,0);
	
			/*draw segment in black*/
				black_out_colors();
				display_a_seg(which_cseg);
				restore_colors();
			}
		}
		else
		{
			my_flood((temp << 4) | temp);

			change_page_gmark_undraw();

			my_set_viewport(-2048,-2048,2048,2048,0);

			black_out_colors();
			stop_after_matrix_sub = 't';
			display_a_seg(i);
			stop_after_matrix_sub = 'f';
			restore_colors();
		}




/* turn off segment i's visibility */
		segment_visibility[i] = 'f';
		segment_calls[i] = -1;

		if(segment_nums[locate_seg] == a_segment)
		{
			locate_1st_draw = 'u';
			locate_called = 'f';
		}


		if(which_cseg != i)   /*empty a closed megatek segment*/
		{
			for(k = 0 ; megatek_locate[i][k] != 0xff ; k++)
				megatek_locate[i][k] = 0xff;

			megatek_ends[i] = 0L;
		}
		else
		{
			for(k = 0 ; megatek_locate[which_cseg][k] != 0xff ; 
			     k++)
				megatek_locate[which_cseg][k] = 0xff;

			megatek_ends[which_cseg] = 0L;

		/*switch to segment 0*/
			save_which_cseg = which_cseg;
			which_cseg = 0;
			goto_megatek_end();
			which_cseg = save_which_cseg;
		}


		if(which_cseg > 0)
			segment_nums[i] = -1;

		if(which_cseg == i)
		{
			which_cseg = 0;
			current_logical_page = end_segment_ptrs[0].logical_page_num;
			current_offset = end_segment_ptrs[0].offset;
			save_map_handle_page = 't';
			map_handle_page(current_logical_page,current_physical_page,
				handle);
			current_pointer_to_dl = physical_pages_pointer + 
				current_offset;
		}

		rw_sub1_0_flag = 'f';


/* deallocate segment's logical pages, etc */
	for(j = 0 ; j < 256 ; j++)
		if(available_pages[j] == i)  /*the page is for this segment*/
		{
/*zero out each page of this segment*/
			save_map_handle_page = 't';
			map_handle_page(j,current_physical_page,handle);

			for(k = 0 ; k < 3000 ; k++)
				*(physical_pages_pointer + k) = 0;


			current_word[j] = 0;
			available_pages[j] = -1;
		}

	last_word[i] = 0;

	start_segment_ptrs[i].logical_page_num = 0;
	start_segment_ptrs[i].offset = 0;
	end_segment_ptrs[i].logical_page_num = 0;
	end_segment_ptrs[i].offset = 0;


	a_segment_deleted[i] = 't';


/*deallocate megatek pages for this segment*/
	for(j = 0 ; j < 256 ; j++)
		if(available_pages[j] == i + 100)  /*page is for this segment*/
			available_pages[j] = -1;

	save_map_handle_page = 't';
	map_handle_page(current_logical_page,current_physical_page,handle);
}




void  cap_B_func(sub_count,which_subr)  /* clear segment */
int *sub_count,which_subr;
{
int i,j;
long num_blocks_in_seg,num_words_seg;
int save_which_cseg;
int temp;


	is_there_graphics = 't';
	restore_graphics_if_needed();


	read_more(2);

		diag_num = 74;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"clear segment %d\n",a_segment);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in clear segment\n",
				a_segment);
			error_logger(diag_string);
			return;
		}



		a_segment_cleared[i] = 't';



/*get out of insert mode for current segment*/
		pw_dl_init(NULL,0,NULL,0);
		which_cseg = i;

		segment_calls[which_cseg] = -1;

		change_page_gmark_undraw();

		my_set_viewport(-2048,-2048,2048,2048,0);
		vwmatx3(&unity_viewing_matrix[0]);

		black_out_colors();
		stop_after_matrix_sub = 't';
		display_a_seg(which_cseg);
		stop_after_matrix_sub = 'f';
		restore_colors();


		temp = return_window_color(GRAPHICS_WINDOW,BACKGROUND);


		if(which_cseg % 2 == 0)   /*even segment number*/
		{
		/*draw segment in black*/
			change_page_gmark_undraw();
			my_set_viewport(-2048,-2048,2048,2048,0);
			black_out_colors();
			display_a_seg(which_cseg);
			restore_colors();
		}
		else if(which_cseg == 3)   /*clear segment 3*/
		{
			if(megatek_ends[4] > 4L)
			/*something in segment 4*/
			{
				my_flood((temp << 4) | temp);

				change_page_gmark_undraw();
				my_set_viewport(-2048,-2048,2048,2048,0);
				black_out_colors();
				stop_after_matrix_sub = 't';
				display_a_seg(which_cseg);
				stop_after_matrix_sub = 'f';
				restore_colors();

				the_flood_flag = 't';
			}
			else   /*its a drag segment*/
			{
				change_page_gmark_undraw();
				my_set_viewport(-2048,-2048,2048,2048,0);
			/*draw segment in black*/
				black_out_colors();
				display_a_seg(which_cseg);
				restore_colors();
			}
		}
		else   /*odd segment number NOT equal to 3 (flood(0) segment)*/
		{
			my_flood((temp << 4) | temp);

			change_page_gmark_undraw();
			my_set_viewport(-2048,-2048,2048,2048,0);
			black_out_colors();
			stop_after_matrix_sub = 't';
			display_a_seg(which_cseg);
			stop_after_matrix_sub = 'f';
			restore_colors();


			the_flood_flag = 't';
		}



/* restore viewport because display_a_seg() brings it inside the border */
		my_set_viewport(-2048,-2048,2048,2048,which_page);

/* restore matrix so when it inserts call sub 11+  the borders draw right */
		vwmatx3(&unity_viewing_matrix[0]);



                if(segment_calls[which_cseg] == ins_matrix_sub)
                        ins_matrix_flag = 'f';


		rw_sub1_0_flag = 'f';


/*deallocate megatek pages for the segment*/
	for(j = 0 ; j < 256 ; j++)
		if(available_pages[j] == i + 100)  /*the page is for this segment*/
			available_pages[j] = -1;


/* clear out the EMS megatek variables */
		for(j = 0 ; megatek_locate[i][j] != 0xff ; j++)
			megatek_locate[i][j] = 0xff;


		current_megatek_page = get_next_free_page(MEGATEK);
		megatek_locate[i][0] = current_megatek_page;
		megatek_ends[i] = 0L;
		current_megatek_insert_pos = 0L;
		megatek_map_handle(current_megatek_page,handle);


/*put 14 null words into file's header*/
		dl_pointer_element.op_code = 44;

		for(j = 0 ; j < size_header ; j++)
			insert_megatek();


/* deallocate segment's logical pages, etc */
	for(j = 0 ; j < 256 ; j++)
		if(available_pages[j] == i)  /*the page is for this segment*/
		{
			current_word[j] = 0;
			available_pages[j] = -1;
		}

	last_word[i] = 0;


/*allocate it 1 page*/
	current_logical_page = get_next_free_page(DISPLAY_LIST);
	current_offset = 0;

	save_map_handle_page = 't';
	map_handle_page(current_logical_page,current_physical_page,handle);
	current_pointer_to_dl = physical_pages_pointer;

	start_segment_ptrs[i].logical_page_num = current_logical_page;
	start_segment_ptrs[i].offset = 0;
	end_segment_ptrs[i].logical_page_num = current_logical_page;
	end_segment_ptrs[i].offset = 0;


	/*fill segment i's  Xform header  (SHOULD COPY IT FROM SEGMENT 0) */
		pw_dl_init(current_pointer_to_dl,0,NULL,0);

		my_set_viewport(-2048,-2048,2048,2048,0);
		vwmatx3(&unity_viewing_matrix[0]);
		slpat32(1,0xffffffff);
		linpat(1);

		pw_dl_rdptr((unsigned short **)&current_pointer_to_dl);

		dlint((short)(END_OF_SEGMENT_INTERRUPT + i));
		pw_dl_init(NULL,0,NULL,0);


	/* run the header data */
		my_set_viewport(-2048,-2048,2048,2048,0);
		vwmatx3(&unity_viewing_matrix[0]);
		slpat32(1,0xffffffff);
		linpat(1);


		current_offset = end_segment_ptrs[i].offset = 
		    current_pointer_to_dl -
			physical_pages_pointer;

}




void  cap_F_func(sub_count,which_subr)  /* rename segment */
int *sub_count,which_subr;
{
int i;
	read_more(4);
	
		diag_num = 76;

		old_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
		new_segment = comp_rel_coord(megatek_data[2],megatek_data[3]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"rename segment from %d to %d\n",old_segment,
				new_segment);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == old_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in rename segment\n",
					old_segment);
			error_logger(diag_string);
			return;
		}


		current_logical_page = end_segment_ptrs[0].logical_page_num;
		current_offset = end_segment_ptrs[0].offset;
		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);
		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;
		segment_nums[i] = new_segment;
		which_sub = -1;

		which_cseg = 0;
		goto_megatek_end();
}




void  single_quote(sub_count,which_subr) /* open subroutine */
int *sub_count,which_subr;
{
int i;


	read_more(2);
	
		diag_num = 77;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);	
#ifdef HAVE_DIAGS
		sprintf(diag_string,"open subroutine %d\n",a_subroutine);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i != NUMBER_OF_SUBS)
		{
			which_sub = i;
			fill_sub = 't';
			which_cseg = 0;
			sub_insert_ptrs[i] = -1;

		current_logical_page = end_segment_ptrs[0].logical_page_num;
		current_offset = end_segment_ptrs[0].offset;
		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);
		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;
			which_cseg = 0;
			goto_megatek_end();

			which_sub = i;
			fill_sub = 't';

			initialize_sub_wrmask();

			sub_ends[sub_nums[which_sub]] = current_sub_pointer;
			an_element.op_code = 44;
			insert_sub(an_element,which_sub);

/*set the entry in the sub calling table for this re-opened sub*/
			set_sub_calling_table();

			return;

		}


		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == -1)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no more subroutine segments available in open subroutine\n");
			error_logger(diag_string);
			which_sub = NUMBER_OF_SUBS + 3;
			fill_sub = 't';
			which_cseg = 0;
			goto_megatek_end();
		}
		else
		{
		current_logical_page = end_segment_ptrs[0].logical_page_num;
		current_offset = end_segment_ptrs[0].offset;
		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);
		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;

			which_sub = i;
			sub_nums[i] = a_subroutine;
			fill_sub = 't';

			which_cseg = 0;	
			goto_megatek_end();

			initialize_sub_wrmask();

			sub_ends[sub_nums[which_sub]] = current_sub_pointer;

			an_element.op_code = 44;
			insert_sub(an_element,which_sub);

/*set the entry in the sub calling table for this newly opened sub*/

			set_sub_calling_table();
		}

}

	


void  c_func(sub_count,which_subr)  /* delete subroutine */
int *sub_count,which_subr;
{
int i;




	read_more(2);
	
		diag_num = 78;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"delete subroutine %d\n",a_subroutine);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in delete subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}

		sub_nums[i] = -1;

		if(which_sub == i)
		{
			fill_sub = 'f';
			which_sub = -1;
		}

		sub_insert_ptrs[a_subroutine] = -1;
		pw_dl_init(&(sub_struct_ptr ->
			sub_display_lists[a_subroutine][0]),0,NULL,0);
		ret();
		pw_dl_rdptr((unsigned short **)&sub_ends[a_subroutine]);
		pw_dl_init(NULL,0,NULL,0);

}




void  b_func(sub_count,which_subr)  /* clear subroutine */
int *sub_count,which_subr;
{
int i;




		read_more(2);

		diag_num = 79;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"*******************************clear subroutine %d\n",a_subroutine);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in clear subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}

		which_sub = i;
		fill_sub = 't';


		if((a_subroutine == 3) || (a_subroutine == 6)
			|| (a_subroutine == 7))
			change_sub3_6_7 = 't';

		sub_insert_ptrs[a_subroutine] = -1;
		initialize_sub_wrmask();
		sub_ends[sub_nums[which_sub]] = current_sub_pointer;
		pw_dl_init(current_sub_pointer,0,NULL,0);
		ret();
		pw_dl_init(NULL,0,NULL,0);

		an_element.op_code = 44;
		insert_sub(an_element,which_sub);
	
}



void  a_func(sub_count,which_subr)  /* append subroutine */
int *sub_count,which_subr;
{
int i;



		read_more(2);
	
		diag_num = 80;

		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"append subroutine %d\n",a_subroutine);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in append subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}

		which_sub = i;
		fill_sub = 't';

		current_sub_pointer = sub_ends[sub_nums[which_sub]] - 1;
}



void  f_func(sub_count,which_subr)  /* rename subroutine */
int *sub_count,which_subr;
{
int i;

		read_more(4);

		diag_num = 82;

		old_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
		new_subroutine = comp_rel_coord(megatek_data[2],megatek_data[3]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"rename subroutine from %d to %d\n",
				old_subroutine,new_subroutine);
		my_diag(diag_string);
#endif

		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == old_subroutine)
				break;
	
		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in rename subroutine\n",
					old_subroutine);
			error_logger(diag_string);
			return;
		}

		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == new_subroutine)
				break;

		if(i < NUMBER_OF_SUBS)
		{
			return;
		}


		current_logical_page = end_segment_ptrs[0].logical_page_num;
		current_offset = end_segment_ptrs[0].offset;
		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);

		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;

		sub_nums[i] = new_subroutine;
		which_cseg = 0;
		goto_megatek_end();

		which_sub = -1;
		fill_sub = 'f';
	
}





void  at_func(sub_count,which_subr)  /* open segment */
int *sub_count,which_subr;
{
int i,j,temp;
long num_blocks_total;
char are_there_segments;


	read_more(3);

	is_there_graphics = 't';
	restore_graphics_if_needed();

	are_there_segments = 'f';

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		if(segment_nums[i] != -1)
			are_there_segments = 't';


	if(are_there_segments == 'f')
	{
		change_page_gmark_undraw();

		my_set_viewport(-2048,-2048,2048,2048,0);
		temp = return_window_color(GRAPHICS_WINDOW,BACKGROUND);
		my_flood((temp << 4) | temp);

		change_page_gmark_undraw();
		my_set_viewport(-2048,-2048,2048,2048,0);
	}



		rw_sub1_0_flag = 'x';	
		is_a_seg_open = 't';

		diag_num = 71;

		a_segment = comp_rel_coord(megatek_data[1],megatek_data[2]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"open segment %d\n",a_segment);
		my_diag(diag_string);
#endif
		diag_num = -71;

		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
			{
				update_current_segments_end();
		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;

			which_cseg = i;

			for(j = 0 ; megatek_locate[which_cseg][j] != 0xff ;
			   j++)
				megatek_locate[which_cseg][j] = 0xff;


			for(j = 0 ; j < 255 ; j++)
				if(available_pages[j] == which_cseg + 100)
					available_pages[j] = -1;

			current_megatek_page = get_next_free_page(MEGATEK);
			megatek_locate[which_cseg][0] = current_megatek_page;
			megatek_ends[which_cseg] = 0L;
			current_megatek_insert_pos = 0L;
			megatek_map_handle(current_megatek_page,handle);


			dl_pointer_element.op_code = 44;
			for(j = 0 ; j < size_header ; j++)
				insert_megatek();

			/*this was NEVER done BEFORE!!!!!!!(free pixel pages)*/
				for(j = 0 ; j < 255 ; j++)
					if(available_pages[j] == which_cseg)
						available_pages[j] = -1;


				current_logical_page = get_next_free_page(
					DISPLAY_LIST);
				current_offset = 0;
				current_word[current_logical_page] = 0;
				last_word[which_cseg] = 0;
				start_segment_ptrs[which_cseg].logical_page_num
					 = current_logical_page;
				start_segment_ptrs[which_cseg].offset = 0;
				save_map_handle_page = 't';
				map_handle_page(current_logical_page,
					current_physical_page,handle);
				current_pointer_to_dl = physical_pages_pointer;

				goto reset_header;

			} /*end if within for*/


		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == -1)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,
				"error no more segments available in open segment call\n");
			error_logger(diag_string);
			which_cseg = 22;
			return;
		}

		update_current_segments_end();
		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;

		segment_nums[i] = a_segment;

		which_cseg = i;


		current_megatek_page = get_next_free_page(MEGATEK);
		megatek_locate[i][0] = current_megatek_page;
		megatek_ends[which_cseg] = 0L;
		current_megatek_insert_pos = 0L;
		megatek_map_handle(current_megatek_page,handle);

		dl_pointer_element.op_code = 44;
		for(j = 0 ; j < size_header ; j++)
			insert_megatek();

		current_logical_page = get_next_free_page(DISPLAY_LIST);
		current_offset = 0;
		current_word[current_logical_page] = 0;
		last_word[which_cseg] = 0;

		end_segment_ptrs[which_cseg].logical_page_num =
			current_logical_page;
		start_segment_ptrs[which_cseg].logical_page_num =
			 current_logical_page;
		start_segment_ptrs[which_cseg].offset = 0;
		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);
		current_pointer_to_dl = physical_pages_pointer;


reset_header:
		if((megatek_data[0] & 2) != 0) /*transform segment*/
		{
#ifdef HAVE_DIAGS
			my_diag("transform segment\n");
#endif

		}
		else  /*basic segment*/
		{
#ifdef HAVE_DIAGS
			my_diag("basic segment\n");
#endif

		}

	
		if((megatek_data[0] & 1) != 0) /*visability ON*/
		{
#ifdef HAVE_DIAGS
			my_diag("visibility ON\n\n");
#endif
			segment_visibility[i] = 't';
			an_element.flag1 = 't';
		}
		else  /*visability OFF*/
		{
#ifdef HAVE_DIAGS
			my_diag("visibility OFF\n\n");
#endif
			segment_visibility[i] = 'f';
			an_element.flag1 = 'f';
		}




	/*fill segment i's  Xform header  (SHOULD COPY IT FROM SEGMENT 0) */
	
		pw_dl_init(current_pointer_to_dl,0,NULL,0);

		my_set_viewport(-2048,-2048,2048,2048,0);
		vwmatx3(&unity_viewing_matrix[0]);
		slpat32(1,0xffffffff);
		linpat(1);

		pw_dl_rdptr((unsigned short **)&current_pointer_to_dl);

		dlint((short)(END_OF_SEGMENT_INTERRUPT + i));
		pw_dl_init(NULL,0,NULL,0);


	/* run the header data */
		my_set_viewport(-2048,-2048,2048,2048,0);
		vwmatx3(&unity_viewing_matrix[0]);
		slpat32(1,0xffffffff);
		linpat(1);


		current_offset = end_segment_ptrs[i].offset = 
		    current_pointer_to_dl -
			physical_pages_pointer;

}







void  e_func(sub_count,which_subr)  /* call subroutine */
int *sub_count,which_subr;
{
int i,j,k;





	if(our_call == 'f')
	{
		read_more(2);
	}

	if(fill_sub == 't')  /*insert sub call into current subroutine*/
	{
		an_element.op_code = 0;
		an_element.arg1 = comp_rel_coord(megatek_data[0],megatek_data[1]);
		insert_sub(an_element,which_sub);
		a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"call subroutine %d\n",a_subroutine);
		my_diag(diag_string);
#endif

                if((sub_rewrite == 't') && (last_sub_rewritten >= 10))
                {
                        if((a_subroutine == 0) || (a_subroutine == 1))
                        {
                                rw_sub_w_0_1 = 't';
                                rw_which_seg = where_called[last_sub_rewritten
                                         - 10][0];

                                if(a_subroutine == 0)
                                        black = 't';
                                else
                                        black = 'f';
                        }
                        else
                        {
                                rw_sub_w_0_1 = 'f';
                                black = 'f';
                        }
                        
                }
                else
                {
                        rw_sub_w_0_1 = 'f';
                        black = 'f';
                }

	}
	else    /*insert sub call into current segment*/
	{
		diag_num = 81;

		if(our_call == 'f')
			a_subroutine = comp_rel_coord(megatek_data[0],megatek_data[1]);
		else if(our_call == 's')
			a_subroutine = ptr_sub -> arg1;


#ifdef HAVE_DIAGS
		sprintf(diag_string,"call subroutine %d\n",a_subroutine);
		my_diag(diag_string);
#endif


		if(our_call == 'f')
		{
			an_element.op_code = 0;
			an_element.arg1 = a_subroutine;
			insert_ours(an_element,which_cseg);

/*
                        if(a_subroutine >= 10) 
                        {
                                for(k = 0 ; (k < 10) && (where_called[a_subroutine
                                        - 10][k] != -1) && (where_called[
                                          a_subroutine - 10][k] != which_cseg)
                                           ; k++)
                                                ;
                                if(where_called[a_subroutine - 10][k] == -1)
                                        where_called[a_subroutine - 10][k] = 
                                             which_cseg;
                        }
*/


                        if(a_subroutine >= 10)
                                segment_calls[which_cseg] = a_subroutine;

		}
	

		our_call = 'f';


		for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
			if(sub_nums[i] == a_subroutine)
				break;

		if(i == NUMBER_OF_SUBS)
		{
			sprintf(diag_string,"no such subroutine as %d in call subroutine\n",
					a_subroutine);
			error_logger(diag_string);
			return;
		}


		if(rewrite_flag == 't')
		{
			if((a_subroutine == 0) || (a_subroutine == 1) ||
				(a_subroutine == 2))
			{
				if(a_subroutine == 0)
				{
#ifdef HAVE_DIAGS
					my_diag("rewrite call sub 0\n");
#endif
					black = 't';
					black_flags[rw_counter - 1] = 't';
					my_color(0);
				}
				else
				{
					black = 'f';
					black_flags[rw_counter - 1] = 'f';
				}

				rw_sub1_0_flag = 't';
				sub1_0_seg = which_cseg;
				rw_op_code = 4;
				num_op_code_0s = 0;

				ptr_rw_sub1_0 = my_ftell();

				if(op_code_to_overwrite != 0)
				{
					sprintf(diag_string,
						"SUB CALL REWRITE ERROR in seg %d: Host ID = %ld , DISP = %ld\n",
						which_cseg,save_host_id,
						save_host_displacement);
					error_logger(diag_string);
					sprintf(diag_string,
						"OPCODE OVERWRITTEN = %d\n",
						op_code_to_overwrite);
					error_logger(diag_string);
					sprintf(diag_string,
						"WE REWRITE at %ld",ptr_rw_sub1_0);
					error_logger(diag_string);
				}

			}
			else
				rw_sub1_0_flag = 'x';

		}
		else
		{
			black = 'f';

			if(a_subroutine == 0)
				no_draw_flag = 't';

			if(a_subroutine == 1)
				no_draw_flag = 'f';

		}

		which_subr = i;


	if(do_not_dlrun_flag == 't')
	{
		slpat32(1,0xffffffff);
		return;
	}


		if((sub_nums[i] == 6) || (sub_nums[i] == 7) || 
			(sub_nums[i] == 3))
		{
			if(segment_visibility[which_cseg] == 't')
			{
				my_set_viewport(-2048,-2048,2048,2048,
					which_page);
				interp_sub(sub_nums[which_subr]);
			}
		}
		else
		{
			if((rewrite_flag == 'f') && ((sub_nums[i] !=
				last_subroutine_run) || (sub_nums[i] >= 10) 
				|| (sub_nums[i] == 4 /* grid subr */)))
			{
			    if(segment_visibility[which_cseg] == 't')
			    {
				pw_dl_init(&temp2_dl[0],0,NULL,0);
				call(&(sub_struct_ptr ->
					sub_calling_table[sub_nums[i]][0]));
				dlend();
				pw_dl_init(NULL,0,NULL,0);

				dlrun(&temp2_dl[0]);
	
				pw_dl_wait();
			    }
			}


			last_subroutine_run = sub_nums[i];
		}

	}
}





void  display_segs()
{
int i,j,k,num_sub_entries,num_seg_entries,original_seg,sub_index;
char tt,save_fill_sub;
int ch,redisplay_page;
long dummy,start_time,end_time;
unsigned long interrupt_pos;
unsigned long old_interrupt_pos;
unsigned long physical_interrupt_pos;
int save_current_my_x,save_current_my_y,save_current_my_z;

/*
printf("\n display_segs() called   returns instantly\n");
return;
*/

	if(do_not_dlrun_flag == 't')
		return;


/*
printf("\n\nENTER DISPLAY_SEGS() :  current_pointer_to_dl = %p\n",
	current_pointer_to_dl);
printf("current_logical_page = %d, current_offset = %d, which_cseg = %d\n",
	current_logical_page,current_offset,which_cseg);
printf("current_megatek_pointer = %p , current_megatek_page = %d\n",
	current_megatek_pointer,current_megatek_page);
printf("current_megatek_insert_pos = %ld",current_megatek_insert_pos);
*/


	save_map_info();
/*
printf("\n save_map_info()  was called here\n");
*/	
	save_current_my_x = my_x;
	save_current_my_y = my_y;
	save_current_my_z = my_z;


	doing_redisplay = 't';

/*
	read_curser_pos();
	blinking_string("SCREEN UPDATE",24,20);
	goto_row_col(row_curser,col_curser);
*/

	if((time_redisplay == 't') && (doing_an_alt_u == 't'))
	{
		start_time = time(&dummy);
/*
		read_curser_pos();
		blinking_string("TIMER      ",24,68);
		goto_row_col(row_curser,col_curser);
*/
	}


	which_redis = 1;
	save_fill_sub = fill_sub;
	quit_redis = 'f';

	original_seg = which_cseg;



/* for all visible segments EXCEPT segment 0 */

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
	{
		if(segment_nums[i] == -1)
			continue;

		which_cseg = i;

		if((i > 0) && (end_segment_ptrs[i].logical_page_num == 0))
			continue;

		if((locate_called == 't') && (which_cseg == locate_seg))
			continue;

		if(segment_visibility[i] == 'f')
			continue;


#ifdef HAVE_DIAGS
		sprintf(diag_string,"SEGMENT %d STARTS HERE\n",i);
		my_diag(diag_string);
#endif



		redisplay_page = -1;
		my_interrupt_reason = -1;

		save_map_handle_page = 't';
		map_handle_page(start_segment_ptrs[i].logical_page_num,
			current_physical_page,handle);

		while((my_interrupt_reason & 0xf000)
			 != END_OF_SEGMENT_INTERRUPT)
		{
/*
			while(pw_dl_busy() == TRUE)
				;
*/
			pw_dl_wait();

/*
			if(ems_mode == HARDWARE_EMS)
			{
				(*interrupt_reason_ptr) = -1;
				dlrun(physical_pages_pointer);
			}
			else
			{
				(*interrupt_reason_ptr) = -1;
				memcpy(&software_ems_buffer[0],
					physical_pages_pointer,16380);
				dlrun(&software_ems_buffer[0]);
			}
*/

			my_interrupt_reason = -1;
			dlrun(physical_pages_pointer);


re_wait: 		pw_dl_wait();
			my_interrupt_reason = pw_dl_reason();

/*
		if(my_pw_dl_wait() == TIME_OUT)
				goto my_quit;
*/

			/* handles keyboard mashing (ONLY ugouts are allowed)*/
				do_ugouts_only = 't';
				my_pckb();
				do_ugouts_only = 'f';

/*
			rdladdrp(&interrupt_pos);
*/


			if((my_interrupt_reason >= 1) &&
				(my_interrupt_reason < 200))
			{
				if(my_interrupt_reason == 1)
				{
					putchar(7);

			/*
					goto_row_col(24,65);
					printf("REASON =  1 ");
					goto_row_col(row_curser,col_curser);
			*/

					error_logger("Display_segs() : Reason = 1\n");

					if(redisplay_page >= 0)
						reason_message(1,"Display_segs()",
							redisplay_page);
					else
						reason_message(1,"Display_segs()",
							start_segment_ptrs[i].
							     logical_page_num);


					goto my_quit;
				}
				else
				{
					putchar(7);
			/*
					goto_row_col(24,65);
					printf("REASON =  %d ",
						my_interrupt_reason);
					goto_row_col(row_curser,col_curser);
			*/

					sprintf(diag_string,
					   "Display_segs() : REASON =  %d \n",
						my_interrupt_reason);

					error_logger(diag_string);


					if(redisplay_page >= 0)
						reason_message(
							my_interrupt_reason,
							"Display_segs()",
							redisplay_page);
					else
						reason_message(
							my_interrupt_reason,
							"Display_segs()",
							start_segment_ptrs[i].
							     logical_page_num);


					goto my_quit;
				}
					
			}
			else if(my_interrupt_reason < 0)
				goto my_quit;



			switch(my_interrupt_reason & 0xf000)
			{
				case MATRIX_SUB_INTERRUPT :
				{


					dlgo();

			/*
					dlrunp(interrupt_pos);
			*/

					goto re_wait;
				}

				case INTERP_A_SUB_TIME :
				{
/*					printf("\n\nBEFORE INTERPRETING A SUB: interrupt_pos = %lx  offset = %ld bytes\n",
					           interrupt_pos,interrupt_pos - 
							(unsigned long)0xd0000);
*/


					interp_sub(my_interrupt_reason -
						INTERP_A_SUB_TIME);

/*
					printf("\n\nAFTER INTERPRETING A SUB: interrupt_pos = %lx  offset = %ld bytes\n",
					           interrupt_pos,interrupt_pos -
							(unsigned long)0xd0000);
*/

/*
					while(pw_dl_busy() == TRUE)
						;
*/
					pw_dl_wait();

printf("\nafter interpreting a sub\n");
				/*
					dlrunp(interrupt_pos);
				*/
			
					goto re_wait;
				}

				case XFORM_ENABLE_INTERRUPT :
				{

					dlgo();

			/*
					dlrunp(interrupt_pos);
			*/

					goto re_wait;
				}

				case XFORM_DISABLE_INTERRUPT :
				{

					dlgo();

			/*
					dlrunp(interrupt_pos);
			*/

					goto re_wait;
				}


				case INSERT_2D_XLATE_INTERRUPT :
				{

/*					my_3d_viewing_matrix[12] =
						insert_sub_x_Xlate[
						  (*interrupt_reason_ptr) -
						    INSERT_2D_XLATE_INTERRUPT];
					my_3d_viewing_matrix[13] =
						insert_sub_y_Xlate[
						  (*interrupt_reason_ptr) -
						    INSERT_2D_XLATE_INTERRUPT];
					vwmatx3(&my_3d_viewing_matrix[0]);

					sdladdrp(interrupt_pos);
*/

					dlgo();

				/*
					dlrunp(interrupt_pos);
				*/

					goto re_wait;
				}

				case DL_BRANCH_INTERRUPT :
				{
					redisplay_page = my_interrupt_reason
						- DL_BRANCH_INTERRUPT;

					break;
				}

				case END_OF_SEGMENT_INTERRUPT :
				{
					redisplay_page = -1;

					break;
				}

				default :
				{
					printf("\ndisplay_segs() : Bogus reason  = %d\n",my_interrupt_reason);
					break;
				}

			}  /*end interrupt reason switch*/


			if(redisplay_page > -1)
			{
				save_map_handle_page = 'f';
				map_handle_page(redisplay_page,
					current_physical_page,handle);

			}

		} /*end while in segment i*/


	} /*end for i*/



my_quit:

	which_redis = 4;


#ifdef HAVE_DIAGS
	my_diag("END OF REDISPLAY SEGMENTS\n");
#endif


	if((time_redisplay == 't') && (doing_an_alt_u == 't'))
	{
		end_time = time(&dummy);
/*
		read_curser_pos();
*/
		putchar(7);
		sprintf(diag_string,"%ld Seconds ",end_time -
			start_time);

/*
		normal_string(diag_string,24,68);
		goto_row_col(row_curser,col_curser);
*/
	}


/*
	read_curser_pos();
	normal_string("              ",24,20);
	goto_row_col(row_curser,col_curser);
*/

	doing_redisplay = 'f';
	which_cseg = original_seg;

	my_x = save_current_my_x;
	my_y = save_current_my_y;
	my_z = save_current_my_z;



/* go back to the EMS state you started this with */

	restore_map_info();
/*
printf("\nrestore_map_info()  was called here\n");
*/

/*
printf("\nLEAVING :  current_pointer_to_dl = %p\n",
	current_pointer_to_dl);
printf("current_logical_page = %d, current_offset = %d, which_cseg = %d\n",
	current_logical_page,current_offset,which_cseg);
printf("current_megatek_pointer = %p , current_megatek_page = %d\n",
	current_megatek_pointer,current_megatek_page);
printf("current_megatek_insert_pos = %ld\n",current_megatek_insert_pos);
*/

}






char  ignore_bogus_color(current_where_to_end)
long  current_where_to_end;
{
	if(op_code_to_overwrite == 0)    /* sub rewrite */
	{
		if(rw_position_in_file + 1L == current_where_to_end)
		{
			return('t');
		}
		else
			return('f');
	}
	else if(op_code_to_overwrite == 4)    /* color rewrite */
	{
		if((rw_position_in_file + 1L == current_where_to_end) ||
			(rw_position_in_file + 2L == current_where_to_end))
		{
			return('t');
		}
		else
			return('f');
	}


	return('f');
}




void  display_part_seg()  
{
long where_to_start,where_to_end;
int page_to_start_on,offset_to_start_at,next_page;
int page_to_end_on,offset_to_end_on;
char split_partial;
char ignore_1st_color;
long position_of_bogus_color;
unsigned long interrupt_pos;


/*
printf("\ndisplay_part_seg() called   returns instantly\n");
return;
*/

	if(do_not_dlrun_flag == 't')
		return;


	ignore_1st_color = 'f';

	if(segment_visibility[which_cseg] == 'f')
		return;

	if(no_more_ems_pages == 't')
		return;



/* DLRUN THE SEGMENT'S HEADER & MATRIX SUB PART */
	partial_map_handle(start_segment_ptrs[which_cseg].logical_page_num,
		handle);


	pw_dl_wait();


	my_interrupt_reason = -1;
	dlrun(blt_page_frame_ptr);
	
	
	pw_dl_wait();
	my_interrupt_reason = pw_dl_reason();

	if(my_interrupt_reason != MATRIX_SUB_INTERRUPT)
	{
		sprintf(diag_string,
			"Display_part_seg(): DLRUN HEADER reason = %x\n",
				my_interrupt_reason);
		error_logger(diag_string);
	}



/* Partial find & run code */

	if(black == 't')  /*draw partial redisplay stuff in black*/
	{
		wrmask(io_masks[which_page]);   /*undo call sub 0 effect*/

/* replace sub 0's entry in the sub calling table with a black sub 0 call*/
		install_black_sub0();    
		my_dash_enable(0);   /*disable dashing*/
	}


	wrmask(io_masks[which_page]);


	where_to_start = rw_position_in_file;
	megatek_seek(which_cseg,where_to_start);

	get_our_value(8,8);

	while(read_element.op_code != 4)
	{
		where_to_start = where_to_start - 1L;
		if(megatek_seek(which_cseg,where_to_start) == FAILURE)
			break;

		get_our_value(9,3);
	}


#ifdef yucky_yucky_yucky

/* back up 2 more & see if it helps */
	where_to_start = where_to_start - 2L;
	if(megatek_seek(which_cseg,where_to_start) == SUCCESS)
		get_our_value(9,99);
	else
	{
		where_to_start = where_to_start + 2L;
		megatek_seek(which_cseg,where_to_start);
		get_our_value(9,77);
	}

#endif


	page_to_start_on = read_element.logical_page_num;
	offset_to_start_at = read_element.offset;

	where_to_end = rw_position_in_file;

	if(megatek_seek(which_cseg,where_to_end) == SUCCESS)
		quit_segment = 'f';
	else
		quit_segment = 't';




/*find where to end partial redisplay*/

	get_our_value(7,7);

	if(quit_segment == 'f')
	{
		if(read_element.op_code == 4)  /*then its the color being rewritten
		 (skip over it)*/
		{
			where_to_end = where_to_end + 1L;
			megatek_seek(which_cseg,where_to_end);
			get_our_value(2,3);
		}


		while(read_element.op_code != 4)
		{
			where_to_end = where_to_end + 1L;
			if(megatek_seek(which_cseg,where_to_end) == FAILURE)
			{
				quit_segment = 't';
				break;
			}

			get_our_value(8,0);


			if(read_element.op_code == 4)
			{

			/*
				if(rw_position_in_file + 1L == where_to_end)
					||
				  (rw_position_in_file + 2L == where_to_end))
			  A  NO NO :   + 2L   CAUSES TROUBLE (FOR RICK SMITH)
				DUE TO MORE BULL IN NEW UNIGRAPHICS SOFTWARE
			*/


				if(ignore_bogus_color(where_to_end) == 't')
				{
					ignore_1st_color = 't';
					position_of_bogus_color = where_to_end;

					where_to_end = where_to_end + 1L;
					if(megatek_seek(which_cseg,
						where_to_end) == FAILURE)
					{
						my_diag("While(seeking the End): FAILURE\n");
						quit_segment = 't';
						break;
					}

					get_our_value(9,9);

					continue;

				}  /* end if */

			}  /* end if */

		}  /* end while */

	}  /* end if */



	page_to_end_on = read_element.logical_page_num;
	offset_to_end_on = read_element.offset;


	if(page_to_end_on != page_to_start_on)
		split_partial = 't';
	else
		split_partial = 'f';

/*
printf("\ndisplay_part_seg():  page_to_start_on = %d , offset_to_start_on = %d\n",page_to_start_on,offset_to_start_at); 
printf("page_to_end_on = %d , offset_to_end_on = %d\n\n",page_to_end_on,
	offset_to_end_on);
system("sleep 5");
*/


/*go to start position*/
	partial_map_handle(page_to_start_on,handle);

/*do the partial redisplay*/
	my_interrupt_reason = -1;
	next_page = -1;


	while(my_interrupt_reason != 1)
	{
	/*
		memcpy(&software_ems_buffer[0],physical_pages_pointer,16380);
	*/

		if(ignore_1st_color == 't')
		{
			megatek_seek(which_cseg,position_of_bogus_color);
			get_our_value(9,99);

		/*
			software_ems_buffer[read_element.offset] = 0;
			software_ems_buffer[read_element.offset + 1] = 0;
			software_ems_buffer[read_element.offset + 2] = 0;
			software_ems_buffer[read_element.offset + 3] = 0;
		*/
			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2] = 0;
			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 1] = 0;

			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 2] = 0;
			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 3] = 0;

			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 4] = 0;
			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 5] = 0;

			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 6] = 0;
			sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
				read_element.offset*2 + 7] = 0;
		}



		if((split_partial == 'f') && (quit_segment == 'f'))
	  /* if((No/End of) split partial && partial ends before end-of-segment)
		 put on dlend() 
	  */
		{
		/*
			pw_dl_init(&software_ems_buffer[offset_to_end_on],0,
				NULL,0);
		*/
			pw_dl_init(&(sub_struct_ptr -> my_ems_buffer[
				SUB_HOLDER_BLT][2*offset_to_end_on]),0,NULL,0);
			dlend();
			pw_dl_init(NULL,0,NULL,0);
		}


/*
		while(pw_dl_busy() == TRUE)
			;
*/

		pw_dl_wait();

	/*
		dlrun(&software_ems_buffer[offset_to_start_at]);
	*/
		wrmask(io_masks[which_page]);
		rdmask(io_masks[which_page]);
		dlrun(&(sub_struct_ptr -> my_ems_buffer[SUB_HOLDER_BLT][
			2*offset_to_start_at]));	


re_wait:	pw_dl_wait();
		my_interrupt_reason = pw_dl_reason();


/*
		rdladdrp(&interrupt_pos);
*/

		if(my_interrupt_reason == 1)
			goto the_end;
		else if((my_interrupt_reason > 1) &&
			 (my_interrupt_reason < 200))
		{
			putchar(7);
/*
			goto_row_col(24,65);
			printf("REASON =  %d ",*interrupt_reason_ptr);
			goto_row_col(row_curser,col_curser);
*/

			sprintf(diag_string,"Display Partial : REASON =  %d \n",
				my_interrupt_reason);
			error_logger(diag_string);

			if(next_page >= 0)
				reason_message(my_interrupt_reason,
					"Display Partial",
					next_page);
			else
				reason_message(my_interrupt_reason,
					"Display Partial",
					page_to_start_on);

			goto the_end;
		}


		switch(my_interrupt_reason & 0xf000)
		{
			case MATRIX_SUB_INTERRUPT :
			case INTERP_A_SUB_TIME :
			case XFORM_ENABLE_INTERRUPT :
			case XFORM_DISABLE_INTERRUPT :
			case INSERT_2D_XLATE_INTERRUPT :
			{

				dlgo();

			/*
				dlrunp(interrupt_pos);
			*/

				goto re_wait;
			}

			case DL_BRANCH_INTERRUPT :
			{
				next_page = my_interrupt_reason
					- DL_BRANCH_INTERRUPT;

				break;
			}

			case END_OF_SEGMENT_INTERRUPT :
				goto the_end;


		}  /*end interrupt reason switch*/


		if(next_page > -1)
		{
			offset_to_start_at = 0;
			save_map_handle_page = 'f';
		/*
			last_map_handle_page = 50;
		*/
			partial_map_handle(next_page,handle);

			if(next_page == page_to_end_on)
				split_partial = 'f';
		}
	}



the_end:


	if(black == 't')
	{
/*put the call to host's sub 0 back into the sub calling table*/
		restore_host_sub0();
		my_dash_enable(4);   /*enable dashing*/
		wrmask(io_masks[which_page]);
	}

	black = 'f';

}






void  display_a_seg(which_seg)
int which_seg;
{
int i,j,save_which_cseg;
unsigned long interrupt_pos;
int redisplay_page;

/*
printf("\ndisplay_a_seg() called   returns instantly\n");
return;
*/

	if(do_not_dlrun_flag == 't')
		return;



	if((which_seg < 0) || (which_seg > NUMBER_OF_SEGS))
		return;

	if(segment_visibility[which_seg] == 'f')
		return;

	if(megatek_locate[which_seg][0] == 0xff)
		return;



#ifdef HAVE_DIAGS
		my_diag("************************DISPLAY_A_SEG STARTS HERE\n");
#endif


		save_map_info();

/*
printf("\nsave_map_info()  was called here\n");
*/

		save_which_cseg = which_cseg;
		which_cseg = which_seg;
		which_redis = 3;

		my_interrupt_reason = -1;
		redisplay_page = -1;

		save_map_handle_page = 't';
		map_handle_page(start_segment_ptrs[which_seg].logical_page_num,
			current_physical_page,handle);

		while((my_interrupt_reason & 0xf000)
			 != END_OF_SEGMENT_INTERRUPT)
		{
/*
			while(pw_dl_busy() == TRUE)
				;
*/
			pw_dl_wait();

/*
			if(ems_mode == HARDWARE_EMS)
			{
				(*interrupt_reason_ptr) = -1;
				dlrun(physical_pages_pointer);
			}
			else
			{
				(*interrupt_reason_ptr) = -1;
				memcpy(&software_ems_buffer[0],
					physical_pages_pointer,16380);
				dlrun(&software_ems_buffer[0]);
			}

*/


			my_interrupt_reason = -1;
			dlrun(physical_pages_pointer);


re_wait:		pw_dl_wait();
			my_interrupt_reason = pw_dl_reason();

/*
printf("\n\nDisplay_a_seg() after pw_dl_wait() : reason = %x = %d\n",
	my_interrupt_reason,my_interrupt_reason);
*/

/*
		if(my_pw_dl_wait() == TIME_OUT)
				goto my_quit;
*/
		/*
			rdladdrp(&interrupt_pos);
		*/

			if((my_interrupt_reason >= 1) &&
				(my_interrupt_reason < 200))
			{
				if(my_interrupt_reason == 1)
				{
					putchar(7);
			/*
					goto_row_col(24,65);
					printf("REASON =  1 ");
					goto_row_col(row_curser,col_curser);
			*/

					error_logger("Display_a_seg() : Reason = 1\n");	

					if(redisplay_page >= 0)
						reason_message(1,"Display_a_seg()",
							redisplay_page);
					else
						reason_message(1,"Display_a_seg()",
							start_segment_ptrs[i].
							     logical_page_num);



					goto my_quit;
				}
				else
				{
					putchar(7);
			/*
					goto_row_col(24,65);
					printf("REASON =  %d ",
						*interrupt_reason_ptr);
					goto_row_col(row_curser,col_curser);
			*/

					sprintf(diag_string,
					    "Display_a_seg() : REASON =  %d \n",
						my_interrupt_reason);
					error_logger(diag_string);


					if(redisplay_page >= 0)
						reason_message(
							my_interrupt_reason,
							"Display_a_seg()",
							redisplay_page);
					else
						reason_message(
							my_interrupt_reason,
							"Display_a_seg()",
							start_segment_ptrs[i].
							     logical_page_num);



					goto my_quit;
				}
					
			}
			else if(my_interrupt_reason < 0)
				goto my_quit;


			switch(my_interrupt_reason & 0xf000)
			{
				case MATRIX_SUB_INTERRUPT :
				{
					if(stop_after_matrix_sub == 't')
					{
/*
printf("\ndisplay_a_seg() MATRIX_SUB_INTERRUPT  got my_quit\n");
*/
						goto my_quit;
					}
					else
					{
/*
printf("\ndisplay_a_seg() MATRIX_SUB_INTERRUPT   dlgo()\n");
*/
						dlgo();
					}

				/*
					dlrunp(interrupt_pos);
				*/


					goto re_wait;
				}

				case INTERP_A_SUB_TIME :
				{
/*					printf("\n\nBEFORE INTERPRETING A SUB: interrupt_pos = %lx  offset = %ld bytes\n",
					           interrupt_pos,interrupt_pos - 
							(unsigned long)0xd0000);
*/


					interp_sub(my_interrupt_reason -
						INTERP_A_SUB_TIME);

/*
					printf("\n\nAFTER INTERPRETING A SUB: interrupt_pos = %lx  offset = %ld bytes\n",
					           interrupt_pos,interrupt_pos -
							(unsigned long)0xd0000);
*/

/*
					while(pw_dl_busy() == TRUE)
						;
*/
					pw_dl_wait();

printf("\after interpreting a sub\n");
				/*
					dlrunp(interrupt_pos);
				*/
			
					goto re_wait;
				}

				case XFORM_ENABLE_INTERRUPT :
				{

					dlgo();

			/*
					dlrunp(interrupt_pos);
			*/

					goto re_wait;
				}

				case XFORM_DISABLE_INTERRUPT :
				{

					dlgo();

			/*
					dlrunp(interrupt_pos);
			*/

					goto re_wait;
				}


				case INSERT_2D_XLATE_INTERRUPT :
				{

/*					my_3d_viewing_matrix[12] =
						insert_sub_x_Xlate[
						  (*interrupt_reason_ptr) -
						    INSERT_2D_XLATE_INTERRUPT];
					my_3d_viewing_matrix[13] =
						insert_sub_y_Xlate[
						  (*interrupt_reason_ptr) -
						    INSERT_2D_XLATE_INTERRUPT];
					vwmatx3(&my_3d_viewing_matrix[0]);
*/
/*					sdladdrp(interrupt_pos);
*/


					dlgo();

			/*
					dlrunp(interrupt_pos);
			*/

					goto re_wait;
				}

				case DL_BRANCH_INTERRUPT :
				{
					redisplay_page = my_interrupt_reason
						- DL_BRANCH_INTERRUPT;

					break;
				}

				case END_OF_SEGMENT_INTERRUPT :
				{
					redisplay_page = -1;

					break;
				}

				default :
				{
					printf("\ndidn,t match anything %x\n",
						my_interrupt_reason & 0xf000);
				}
			}  /*end interrupt reason switch*/


			if(redisplay_page > -1)
			{
				save_map_handle_page = 'f';
				map_handle_page(redisplay_page,
					current_physical_page,handle);

			}

		} /*end while in segment which_seg*/



my_quit:

/*
printf("\ndisplay_a_seg()  before restore_map_info()\n");
*/

		restore_map_info();

/*
printf("\nrestore_map_info()  was called here\n");
*/

		which_cseg = save_which_cseg;

/*
printf("\ndisplay_a_seg()  returns\n");
*/
}






void  cap_A_func(sub_count,which_subr)  /* append segment */
int *sub_count,which_subr;
{
int i;

	is_there_graphics = 't';
	restore_graphics_if_needed();

	read_more(2);

		is_a_seg_open = 't';
	
		diag_num = 75;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
		sprintf(diag_string,"append segment %d\n",a_segment);
		my_diag(diag_string);
#endif
		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"no such segment # as %d in append segment\n",
					a_segment);
			error_logger(diag_string);
			return;
		}


		update_current_segments_end();
		if(megatek_ends[which_cseg] < current_megatek_insert_pos)
			megatek_ends[which_cseg] = current_megatek_insert_pos;


		if(rewrite_flag == 't')
		{
			rewrite_flag = 'f';

                        if((rw_sub1_0_flag == 't') || (rewrite_color == 't'))
			{
				if(rw_threshold == 0)
				{
/*
printf("\nappend segment calls display_part_seg()\n");
*/
					display_part_seg();
					rw_counter = 0;
					save_map_handle_page = 'f';
				/*
					last_map_handle_page = 50;
				*/
				}
				else
					save_map_handle_page = 't';


				rw_sub1_0_flag = 'x';
                                rewrite_color = 'f';
				num_op_code_4s = 0;
				black = 'f';
			}
			else
				save_map_handle_page = 't';
		}
		else
			save_map_handle_page = 't';


		which_cseg = i;
/*
printf("\n append segment before display_a_seg()\n");
*/

/* DLRUN segments header and matrix sub */
		stop_after_matrix_sub = 't';
		display_a_seg(which_cseg);
		stop_after_matrix_sub = 'f';

/*
printf("\nappend segment after display_a_seg()  before goto_megatek_end()\n");
*/

		goto_megatek_end();

/*
printf("\nappend segment after  goto_megatek_end()\n");
*/

		current_logical_page = end_segment_ptrs[which_cseg].
			logical_page_num;
		current_offset = end_segment_ptrs[which_cseg].offset;

/*
printf("\nappend segment before map_handle_page()\n");
*/

		save_map_handle_page = 't';
		map_handle_page(current_logical_page,current_physical_page,
			handle);

/*
printf("\nappend segment after map_handle_page()\n");
*/

		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;

/*
printf("\nappend segment returns\n");
*/

}





void  d_func(sub_count,which_subr) /*close subroutine*/
int *sub_count,which_subr;
{
int i,segment_num;



	fill_sub = 'f';


	if(change_sub3_6_7 == 't')
	{
		change_sub3_6_7 = 'f';
		sub_insert_ptrs[NUMBER_OF_SUBS] = -1;
		compile_sub_curser(6);
	}


	if(sub_rewrite == 't')
	{
		sub_rewrite = 'f';

		if(save_sub_insert_ptr > -2)
			sub_insert_ptrs[sub_nums[which_sub]] =
				 save_sub_insert_ptr;
	

                if(rw_sub_w_0_1 == 't')
                {
                        rw_sub_w_0_1 = 'f';

                        if(last_sub_rewritten > 10)
                        {
			     for(segment_num = 1 ; segment_num < NUMBER_OF_SEGS
					; segment_num++)
				if(segment_calls[segment_num] ==
					last_sub_rewritten)
						break;


			     if(((segment_visibility[segment_num] == 'f')  &&
				(visibility_changed_to_on[segment_num] == 'f'))
				|| (segment_num == NUMBER_OF_SEGS))
			     {
				which_sub = -1;
				last_sub_rewritten = -1;
				black = 'f';
				return;
			     }

/*
			     while(pw_dl_busy() == TRUE)
					;
*/
			     pw_dl_wait();	

			     vwmatx3(&unity_viewing_matrix[0]);
			     my_set_viewport(-2048,-2048,2048,2048,0);

				if(black == 't')
				{
					install_black_sub0();
					black_out_colors();
				}


			if(do_not_dlrun_flag == 'f')
			{
				pw_dl_init(&temp2_dl[0],0,NULL,0);
				call(&(sub_struct_ptr ->
					sub_display_lists[last_sub_rewritten]
					[0]));
				dlend();
				pw_dl_init(NULL,0,NULL,0);

				dlrun(&temp2_dl[0]);
				pw_dl_wait();
			}


				if(black == 't')
				{
					restore_host_sub0();
					restore_colors();
				}
                        }

			black = 'f';
                }

		last_sub_rewritten = -1;
	}
	else  /*not a sub rewrite*/
	{
		pw_dl_init(current_sub_pointer,0,NULL,0);
		ret();
		pw_dl_rdptr((unsigned short **)&sub_ends[sub_nums[which_sub]]);
		pw_dl_init(NULL,0,NULL,0);
	}


	diag_num = 88;
#ifdef HAVE_DIAGS
	my_diag("close subroutine\n");
#endif
	which_sub = -1;

}


