/* MY6.C  rewrite, read PID, read/write vector memory, 
	change & insert attributes, read segment start, 
	my_reset(), system reset, refresh control */


/*October 17, 1988 10:33  Made New read PID use the seg_starts array.
Made New rewrite use the seg_starts array. */

/*October 20, 1988 changed "change something" functions to change the default
header info inside the segment file*/

/*November 15, 1988 made changes to use generic graphics board calls*/

/*January 31, 1989 made changes to send error messages to the error file*/

/*February 8, 1989 changed new rewrite segment: if(temp + 1L <= temp_seg_ptr){}
to if(temp <= temp_seg_ptr){} (was causing trouble) */

/*February 10, 1989 added ins_matrix_flag = 't'; to change visibility to on &
changed insert blink enable into a sub to insert color "system" instead, ONLY
WHEN ITS DOING SUB 2*/

/*February 24, 1989 added int far op_code_to_overwrite to record the op code
that NEW REWRITE is going to overwrite and
made insert intensity/color check this variable when rewrite_flag == 't'
also added long far save_host_id & long far save_host_displacement*/

/*March 22, 1988 made changes to segment rewrite so it will really work for
the pixel version*/

/*March 24, 1989 made changes in my_reset() and New Rewrite for subs to
handle non-interpreted subs*/


/*March 28, 1989 added code to store the change-something commands stuff*/

/*March 30, 1989 added long far rw_position_in_file & int far
rw_file_number, and set them in NEW REWRITE segment*/

#include "my_utils.h"
#include "my_globs.h"
#include "c:\emulate\clipper\pw_lib.h"
#include "c:\emulate\clipper\expanded.h"



extern void far restore_colors();   /* restores color sub table */
extern void far update_a_header(struct header_data,int,int);

extern char far the_flood_flag;
extern int far joy_x_sample;
extern int far joy_y_sample;
extern int far last_mouse_x;
extern int far last_mouse_y;
extern int far last_neg_mouse_y;
extern int far unity_viewing_matrix[];
extern char far a_segment_deleted[];
extern char far a_segment_cleared[];
extern int far rw_threshold;
extern long far megatek_rw_positions[];
extern int far rw_segment[];
extern int far rw_counter;
extern short far *start_of_page_frame_0;
extern int far the_check_handle;
extern int far last_subroutine_run;
extern char far no_more_ems_pages;

extern char far licensee[];
extern char far use_the_debug_mode;


int far save_system_color = 7;
int far op_code_to_overwrite;
long far save_host_id, far save_host_displacement;
long far rw_position_in_file;
int far rw_file_number;

char far visibility_changed_to_on[NUMBER_OF_SEGS];



void far my_reset()
{
int i,subs,from_segs,row,col;


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #1\n");


	segment_nums[0] = 0;
	which_cseg = 0;



	restore_colors();

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #2\n");


	for(row = 0 ; row < NUMBER_OF_SUBS ; row++)
		for(i = 0 ; i < SUB_NUM_ELS ; i++)
			sub_host_to_pix_ptrs[row][i] = NULL;


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #3\n");

	last_subroutine_run = 999;

	for(i = 1 ; i < NUMBER_OF_SEGS ; i++)
		segment_nums[i] = -1;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #4\n");


	for(i = 0 ; i < 256 ; i++)
		available_pages[i] = -1;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #5\n");


	for(row = 0 ; row < 50 ; row++)
		for(col = 0 ; col < 64 ; col++)
			megatek_locate[row][col] = 0xff;


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #6\n");


	for(i = 0 ; i < 50 ; i++)
		megatek_ends[i] = 0L;


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #7\n");


	current_megatek_pointer = (char far *)start_of_page_frame_0;
	current_megatek_page = get_next_free_page(MEGATEK);
	megatek_locate[0][0] = current_megatek_page;
	megatek_map_handle(current_megatek_page,handle);
	current_megatek_insert_pos = 0L;


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #8\n");


	dl_pointer_element.op_code = 44;
	for(i = 0 ; i < 14 ; i++)   	/*put in the null words in the header*/
		insert_megatek();


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #9\n");

	megatek_ends[0] = 14L;


	for(i = 0 ; i < NUMBER_OF_SUBS + 1 ; i++)
		sub_insert_ptrs[i] = -1;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #10\n");


	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
	{
		a_segment_deleted[i] = 'f';
		a_segment_cleared[i] = 'f';
	}

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #11\n");


	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
	{
		segment_visibility[i] = 'f';
		visibility_changed_to_on[i] = 'f';
	}

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #12\n");

	my_x = 0;
	my_y = 0;
	my_z = 0;




	current_logical_page = get_next_free_page(DISPLAY_LIST);
	current_offset = 0;
	map_handle_page(current_logical_page,current_physical_page,handle);

	current_pointer_to_dl = physical_pages_pointer;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #13\n");


/*fill segment 0's  Xform header*/

	pw_dl_init(current_pointer_to_dl,0,NULL,0);

	my_set_viewport(-2048,-2048,2048,2048,0);
	vwmatx3(&unity_viewing_matrix[0]);
	slpat32(1,0xffffffff);
	linpat(1);

	pw_dl_rdptr(&current_pointer_to_dl);

	dlint(END_OF_SEGMENT_INTERRUPT);
	pw_dl_init(NULL,0,NULL,0);


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #14\n");

	for(i = 0 ; i < 256 ; i++)
		current_word[i] = 0;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #15\n");


	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
	{
		last_word[i] = 0;

		start_segment_ptrs[i].logical_page_num = 0;
		start_segment_ptrs[i].offset = 0;

		end_segment_ptrs[i].logical_page_num = 0;
		end_segment_ptrs[i].offset = 0;
	}


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #16\n");


	end_segment_ptrs[0].offset = current_pointer_to_dl - 
		physical_pages_pointer;
	end_segment_ptrs[0].logical_page_num = current_logical_page;
	start_segment_ptrs[0].logical_page_num = current_logical_page;



	for(i = 0 ; i < NUMBER_OF_SUBS + 1 ; i++)
		sub_nums[i] = -1;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #17\n");


/* put the returns in all non-interpreted subroutines */
	for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
	{
		pw_dl_init(&(sub_display_lists[i][0]),0,NULL,0);
		ret();
		pw_dl_init(NULL,0,NULL,0);

		sub_ends[i] = &(sub_display_lists[i][1]);
	}


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #18\n");

	current_sub_pointer = &(sub_display_lists[0][0]);


/* go back to "page" 0 after flooding both "pages" */

	my_set_viewport(-2048,-2048,2048,2048,0);

	which_page = 0;
	wrmask(0xff);
	rdmask(0xff);
	flood(0);
	wrmask(0x0f);
	rdmask(0x0f);

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #19\n");


	my_dash_enable(line_type);

        for(subs = 0 ; subs < 10 ; subs++)
                for(from_segs = 0 ; from_segs < 10 ; from_segs++)
                        where_called[subs][from_segs] = -1;


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #20\n");


        for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
                segment_calls[i] = -1;

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #21\n");



/*initialize "megatek" sub calling table*/
	for(i = 0 ; i < 64 ; i++)
	{
		pw_dl_init(&(sub_calling_table[i][0]),0,NULL,0);
		ret();
		pw_dl_init(NULL,0,NULL,0);
	}


	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION #22\n");


        draw_black = 'f';
        black = 'f';
        no_draw_flag = 'f';
	rewrite_color = 'f';
	rewrite_flag = 'f';
	rw_sub1_0_flag = 'f';
	num_op_code_4s = 0;
	ins_matrix_flag = 'f';
	button1 = 'f';
	which_redis = 4;
	rw_counter = 0;
	fill_sub = 'f';
	our_call = 'f';
	curser_displayed = 'f';
	the_flood_flag = 'f';
	no_more_ems_pages = 'f';

	if(use_the_debug_mode == 't')
		printf("\tRESET FUNCTION IS TOTALLY FINISHED\n");
}





void far n_func(sub_count,which_subr)  /* system reset */
int *sub_count,which_subr;
{

	read_more(1);
	
	
		diag_num = 65;
	
		switch(megatek_data[0] & 0x07)
		{
			case 0 :
			case 4 :
			case 5 :
			case 1 : /*no action*/
			{
			#ifdef HAVE_DIAGS
				my_diag("system reset no action\n");
			#endif
				break;
			}

			case 2 : /*software reset*/
			{
			#ifdef HAVE_DIAGS
				my_diag("system reset software reset\n");
			#endif
				my_reset();
				break;
			}

			case 3 : /*hardware reset*/
			{
			#ifdef HAVE_DIAGS
				my_diag("system reset hardware reset\n");
			#endif
				vm512.least_sig = 836;
				my_reset();
				break;
			}

			case 6 : /*software reset into EDLM mode*/
			{
			#ifdef HAVE_DIAGS
				my_diag("software reset into EDLM mode\n");
			#endif
				vm512.least_sig = 0;
				my_reset();
				break;
			}

			case 7 : /*hardware reset into EDLM mode*/
			{
			#ifdef HAVE_DIAGS
				my_diag("hardware reset into EDLM mode\n");
			#endif
				my_reset();
				break;
			}
		}
	
}




void far m_func(sub_count,which_subr)  /* refresh control */
int *sub_count,which_subr;
{

	read_more(4);

		diag_num = 66;
	#ifdef HAVE_DIAGS
		my_diag("refresh control\n");
	#endif
	
}





void far vert_line(sub_count,which_subr)  /* read current position ID */
int *sub_count,which_subr;
{


}









void far new_read_pid()  /* NEW READ CURRENT POSITION ID (EDLM Mode)*/
{
unsigned char most_sig,least_sig,a_char,first,second,third,fourth;
unsigned int sub_seg_num;
unsigned long sub_seg_ptr;

	read_more(1);
	
	diag_num = 68;
#ifdef HAVE_DIAGS
	my_diag("NEW read current position ID\n");
#endif
	a_char = 0x40;

	if(fill_sub == 't')  /*read subroutine position ID*/
	{
		a_char |= 16; 
		sub_seg_num = sub_nums[which_sub];

		if((sub_seg_num != 3) && (sub_seg_num != 6) && 
		     (sub_seg_num != 7))  /* non - interpreted */
		{
			sub_seg_ptr = (unsigned long)sub_insert_ptrs[
				sub_seg_num] + 1L;
		}
		else   /* interpreted */
			sub_seg_ptr = (unsigned long)sub_insert_ptrs[
				sub_seg_num];

		sub_seg_ptr = sub_seg_ptr +
			(unsigned long)sub_starts[sub_seg_num];
	}
	else   /* read segment position ID*/
	{
		sub_seg_num = segment_nums[which_cseg];	
		sub_seg_ptr = (unsigned long)my_ftell();

		sub_seg_ptr = sub_seg_ptr + seg_starts[which_cseg];  
/*Newest version*/
	}


	most_sig = (sub_seg_num >> 6) & 0x003f;
	least_sig = sub_seg_num & 0x003f;

	first = (sub_seg_ptr >> 18) & 0x0003;
	second = (sub_seg_ptr >> 12) & 0x003f;
	third = (sub_seg_ptr >> 6) & 0x003f;
	fourth = sub_seg_ptr & 0x003f;

	my_ugout(0x1d);
	my_ugout(a_char);
	my_ugout(most_sig | 0x40);
	my_ugout(least_sig | 0x40);
	my_ugout(first | 0x40);
	my_ugout(second | 0x40);
	my_ugout(third | 0x40);
	my_ugout(fourth | 0x40);
	my_ugout(0x0d);

}



void far cap_E_func(sub_count,which_subr)  /* rewrite segment/subroutine */
int *sub_count,which_subr;
{
	
}







void far new_rewrite()  /*NEW REWRITE SEGMENT/SUBROUTINE (EDLM Mode) */
{
int i;
unsigned long id,displacement,sub_seg_ptr,temp_seg_ptr;
unsigned long temp,old_num_blocks,new_num_blocks;
unsigned long block_adjust,remainder,final_result,end_adjust;
int real_sub,j;

	read_more(8);

		diag_num = 69;
		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

		id = (unsigned long)megatek_data[2] & 0x03;
		id <<= 6;
		id |= (unsigned long)megatek_data[3] & 0x3f;
		id <<= 6;
		id |= (unsigned long)megatek_data[4] & 0x3f;
		id <<= 6;
		id |= megatek_data[5] & 0x3f;

		displacement = (unsigned long)(((megatek_data[6] & 0x3f) << 6) 
					| (megatek_data[7] & 0x3f));


		if((megatek_data[2] & 32) != 0) /*specifies a subroutine*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"new rewrite subroutine sub = %d ID = %d displacement = %d\n",
				a_segment,id,displacement);
			my_diag(diag_string);
		#endif
		

			for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
				if(sub_nums[i] == a_segment)
					break;

			fill_sub = 't';

			if(i == NUMBER_OF_SUBS) /*subroutine doesn't exist*/
			{
				for(i = 0 ; i < NUMBER_OF_SUBS ; i++)
					if(sub_nums[i] == -1)
						break;

				sub_nums[i] = a_segment;
				which_sub = i;
				return;
			}


			if((a_segment == 6) || (a_segment == 3) 
				|| (a_segment == 7))
				change_sub3_6_7 = 't';

                        if(a_segment >= 10) /*record which sub*/
                                last_sub_rewritten = a_segment;
                        else
                                last_sub_rewritten = -1;


			which_sub = i;
			real_sub = a_segment;

			sub_rewrite = 't';


			if((real_sub == 3) || (real_sub == 6) || 
			        (real_sub == 7))
			{
				if(id + displacement - (unsigned long)
				     sub_starts[real_sub] <
					(unsigned long) sub_insert_ptrs
							[real_sub])
				{
					save_sub_insert_ptr = 
						sub_insert_ptrs[real_sub];
					sub_insert_ptrs[real_sub] = 
						(int)(id + displacement)
						    - sub_starts[real_sub];
				}
				else 
					save_sub_insert_ptr = -2;
			}
		 	else    /*non_interpreted subs*/
			{
/*
				for(j = 0 ; (j < 2*SUB_NUM_ELS) &&
				  (sub_host_to_pix_ptrs[real_sub][j] != NULL)
				     ; j++)
				{
					if((unsigned long)(sub_host_to_pix_ptrs
					  [real_sub][j] -
					      &(sub_display_lists[
					       real_sub][0]))
						>= id + displacement -
						  (unsigned long)
						    sub_starts[real_sub])
							break;
				}

*/				
				j = (int)(id + displacement - (unsigned long)
					sub_starts[real_sub]);
				current_sub_pointer =
				    sub_host_to_pix_ptrs[real_sub][j];

				save_sub_insert_ptr = sub_insert_ptrs[real_sub];
			}

		}
		else  /*specifies a segment*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"new rewrite segment segment = %d ID = %ld displacement = %ld\n",
				a_segment,id,displacement);
			my_diag(diag_string);
		#endif

			for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
				if(segment_nums[i] == a_segment)
					break;

			if(i == NUMBER_OF_SEGS)
			{
				sprintf(diag_string,
					"no such segment as %d in rewrite segment\n",
						a_segment);
				error_logger(diag_string);
				return;
			}


			update_current_segments_end();  /*before closing it*/
			if(megatek_ends[which_cseg] < 
				current_megatek_insert_pos)
					megatek_ends[which_cseg] =
						current_megatek_insert_pos;



			if(no_more_ems_pages == 't')
				return;





		if(rewrite_flag == 't')
		{
			rewrite_flag = 'f';

                        if((rw_sub1_0_flag == 't') || (rewrite_color == 't'))
			{
				if(rw_threshold == 0)
				{
					display_part_seg();
					rw_counter = 0;
				}


				rw_sub1_0_flag = 'x';
                                rewrite_color = 'f';
				num_op_code_4s = 0;
				black = 'f';
			}
		}  



			save_host_id = id;
			save_host_displacement = displacement;


			which_cseg = i;
			goto_megatek_end();   /*maps it too*/
			temp_seg_ptr = my_ftell();


		temp = id + displacement - seg_starts[which_cseg];
		block_adjust = (id - seg_starts[which_cseg])/64L;
			

/*JUNE 16, 1989 NEW VERSION OF WHERE WE REWRITE COMPUTATION*/

		old_num_blocks = 1L;
		new_num_blocks = 0L;
		final_result = temp;
		end_adjust = 0L;


		while(99)
		{
			old_num_blocks = new_num_blocks;

			new_num_blocks = final_result/64L - block_adjust
				+ end_adjust;

			final_result = temp + new_num_blocks*2L;
			remainder = final_result % 64L;

			if((remainder == 62L) || (remainder == 63L))
				end_adjust = 1L;
			else
			{
				end_adjust = 0L;
				if(old_num_blocks == new_num_blocks)
					break;
			}

		}




		
		if(final_result <= temp_seg_ptr)  /*allow it*/
		{
			if(megatek_seek(which_cseg,final_result) == FAILURE)
				report_seek_error(which_cseg,final_result);
		}
		else
		{
			sprintf(diag_string,
				"REWRITE AFTER END OF SEGMENT# %d: final_result = %lu temp_seg_ptr = %lu\n",
					which_cseg,final_result,temp_seg_ptr);
			error_logger(diag_string);
			sprintf(diag_string,"THE HOST THINKS ITS: ID = %lu , DISPL = %lu\n",
					id,displacement);
			error_logger(diag_string);
		}


		current_megatek_insert_pos = final_result;
		if(megatek_seek(which_cseg,final_result) == FAILURE)
			report_seek_error(which_cseg,final_result);

		get_our_value(8,9);

		op_code_to_overwrite = read_element.op_code;

		if((read_element.op_code != 0) && (read_element.op_code != 4))
		{
			sprintf(diag_string,
				"********REWRITE ERROR******** IN SEGMENT# %d:\n",
					which_cseg);
			error_logger(diag_string);
			sprintf(diag_string,"WE REWRITE AT %lu\n",final_result);
			error_logger(diag_string);
			sprintf(diag_string,"THE OP CODE = %d\n",
				read_element.op_code);
			error_logger(diag_string);
			sprintf(diag_string,"THE HOST THINKS ITS: ID = %lu , DISPL = %lu\n",
					id,displacement);
			error_logger(diag_string);
		}


		rw_position_in_file = final_result;
		rw_file_number = which_cseg;


		if(rw_counter <= rw_threshold)
		{
			megatek_rw_positions[rw_counter] = rw_position_in_file;
			rw_segment[rw_counter] = rw_file_number;

			rw_counter++;
		}


		current_logical_page = read_element.logical_page_num;
		current_offset = read_element.offset;


		map_handle_page(current_logical_page,current_physical_page,
			handle);

		current_pointer_to_dl = physical_pages_pointer + 
			current_offset;



		if((*current_pointer_to_dl != 0x7b) && (*current_pointer_to_dl 
			!= 0  /* noop before color*/ ))
		{
			sprintf(diag_string,"EMS REWRITE ERROR: page = %d , offset = %d\n",
				current_logical_page,current_offset);
			error_logger(diag_string);

			sprintf(diag_string,"megatek file op code = %d , EMS opcode = %x\n",
				read_element.op_code,*current_pointer_to_dl);
			error_logger(diag_string);
		}



	/* IF megatek file shows CALL SUBROUTINE*/
		if((read_element.op_code == 0) && (*current_pointer_to_dl !=
			0x7b))
		{
			sprintf(diag_string,"REWRITE ERROR  Megatek file -> Call sub , EMS -> %x\n",
				*current_pointer_to_dl);
			error_logger(diag_string);
		}


	/* IF megatek file shows a COLOR command*/
		if((read_element.op_code == 4) && (*current_pointer_to_dl !=
			0  /*the NOOP before it*/ ))
		{
			sprintf(diag_string,"REWRITE ERROR  Megatek file -> Color , EMS -> %x\n",
				*current_pointer_to_dl);
			error_logger(diag_string);
		}


		rewrite_flag = 't';

	}
	
}








void far insert_sub_1_attr()
{
		if((flag & 32) != 0) /* high bit set */
		{
			if((flag & 16) != 0) /* Xform Enable */
			{
				an_element.op_code = 6;
				an_element.arg1 = flag;

				diag_num = 53;
			#ifdef HAVE_DIAGS
				my_diag("insert Xform enable\n");
			#endif

				insert_sub(an_element,which_sub);

			}
			else   /*Blink or Dash Enable */
			{
				if((flag & 8) != 0) /*Dash Enable */
				{
					diag_num = 54;

					an_element.op_code = 2;
					an_element.arg1 = flag;

					insert_sub(an_element,which_sub);

				}
				else   /*Blink Enable */
				{
					/*monitor can't do it use flag */

					if(sub_nums[which_sub] != 2)
					{
						an_element.op_code = 1; /*blink*/
						an_element.arg1 = flag;
					}
					else    /*going into sub 2, use color*/
					{
						an_element.op_code = 4; /*color*/
						an_element.arg1 = 
							save_system_color;
					}

					diag_num = 55;
				#ifdef HAVE_DIAGS
					my_diag("insert blink enable\n");
				#endif

					insert_sub(an_element,which_sub);

				}
			}
				
		}
		else   /* high bit not set */
		{
			if((flag & 16) != 0)  /*Blink Rate*/
			{
				flag &= 0x0f;
				diag_num = 56;
			#ifdef HAVE_DIAGS
				my_diag("insert blink rate\n");
			#endif
				an_element.op_code = 3;
				an_element.arg1 = flag;

				insert_sub(an_element,which_sub);

			}
			else  /*Intensity/color*/
			{
				flag &= 0x0f;
				my_color(flag);

				diag_num = 57;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert intensity/color %d\n",flag);
				my_diag(diag_string);
			#endif
				an_element.op_code = 4;
				an_element.arg1 = flag;

				if(sub_nums[which_sub] == 6) /*color into sub 6*/
					save_system_color = flag;

				insert_sub(an_element,which_sub);

			}
		}

}




void far cap_K_func(sub_count,which_subr) /*insert attributes 1 argument*/
int *sub_count,which_subr;
{

	read_more(1);
	flag = megatek_data[0];

	flag &= 0x3f;			

	if(fill_sub == 'f')
	{		
		if((flag & 32) != 0) /* high bit set */
		{
			if((flag & 16) != 0) /* Xform Enable */
			{
				an_element.op_code = 6;
				an_element.arg1 = flag;

				diag_num = 53;
			#ifdef HAVE_DIAGS
				my_diag("insert Xform enable\n");
			#endif
				if((flag & 1) == 0) /*enable*/
				{
				#ifdef HAVE_DIAGS
					my_diag("XFORM ENABLE\n");
				#endif

				}
				else
				{
				#ifdef HAVE_DIAGS
					my_diag("XFORM DISABLE\n");
				#endif

				}

				insert_ours(an_element,which_cseg);
				
			}
			else   /*Blink or Dash Enable */
			{
				if((flag & 8) != 0) /*Dash Enable */
				{
					diag_num = 54;
					if((flag & 1) != 0)
					{
					#ifdef HAVE_DIAGS
						my_diag("insert dash enable\n");
					#endif
					/*	my_dash_enable(4);*/
						line_type = 4;
					}
					else
					{
					#ifdef HAVE_DIAGS
						my_diag("insert dash disable\n");
					#endif
					/*	my_dash_enable(0);*/
						line_type = 0;
					}

					an_element.op_code = 2;
					an_element.arg1 = flag;

					insert_ours(an_element,which_cseg);
					
				}
				else   /*Blink Enable */
				{
					/*monitor can't do it use flag */
					an_element.op_code = 1;
					an_element.arg1 = flag;

					diag_num = 55;
				#ifdef HAVE_DIAGS
					my_diag("insert blink enable\n");
				#endif

					insert_ours(an_element,which_cseg);
					
				}
			}
				
		}
		else   /* high bit not set */
		{
			if((flag & 16) != 0)  /*Blink Rate*/
			{
				flag &= 0x0f;
				diag_num = 56;
			#ifdef HAVE_DIAGS
				my_diag("insert blink rate\n");
			#endif
				an_element.op_code = 3;
				an_element.arg1 = flag;

				insert_ours(an_element,which_cseg);

			}
			else  /*Intensity/color*/
			{
				rw_op_code = 4;
				flag &= 0x0f;

				if(rewrite_flag == 't')
				{
					rewrite_color = 't';
					sub1_0_seg = which_cseg;
					ptr_rw_sub1_0 = my_ftell();
					num_op_code_4s = 0;

					if(op_code_to_overwrite != 4)
					{
						sprintf(diag_string,
						   "COLOR REWRITE ERROR in seg %d: Host ID = %ld , DISP = %ld\n",
							which_cseg,save_host_id,
							    save_host_displacement);
						error_logger(diag_string);
						sprintf(diag_string,
						    "OPCODE OVERWRITTEN = %d\n",
							op_code_to_overwrite);
						error_logger(diag_string);
						sprintf(diag_string,
							"WE REWRITE at %ld",
							   ptr_rw_sub1_0);
						error_logger(diag_string);
					}


				}
				else
					rewrite_color = 'f';



				save_color = flag;

				diag_num = 57;
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"insert intensity/color %d\n",flag);
				my_diag(diag_string);
			#endif

				an_element.op_code = 4;
				an_element.arg1 = flag;


				insert_ours(an_element,which_cseg);
				
			}
		}

	}
	else
		insert_sub_1_attr();
}




void far cap_L_func(sub_count,which_subr) /*insert attributes 3 arguments*/
int *sub_count,which_subr;
{
	read_more(3);

	flag = megatek_data[0];
	flag &= 0x3f;

	if(((flag & 32) != 0) && ((flag & 16) == 0))
	{ /* insert dash pattern */
		if(fill_sub == 'f')
		{
			dash_pat = comp16_3(flag,megatek_data[1],megatek_data[2]);

			diag_num = 58;
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"insert dash pattern %x %x %x  %x\n",
				flag,megatek_data[1],megatek_data[2],dash_pat);
			my_diag(diag_string);
		#endif
			an_element.op_code = 24;
			an_element.arg1 = dash_pat;

			insert_ours(an_element,which_cseg);

			last_subroutine_run = 999;
		}
		else
		{
 			dash_pat = comp16_3(flag,megatek_data[1],megatek_data[2]);
			diag_num = 58;
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"insert dash pattern %x %x %x  %x\n",
				flag,megatek_data[1],megatek_data[2],dash_pat);
			my_diag(diag_string);
		#endif
			an_element.op_code = 24;
			an_element.arg1 = dash_pat;

			insert_sub(an_element,which_sub);

		}
	}
	else if(((flag & 32) == 0) && ((flag & 16) != 0))
	{

			diag_num = 59;
		#ifdef HAVE_DIAGS
			my_diag("insert pick label\n");
		#endif
	}
	else	  /* insert stroke-refresh color */
	{
		if(fill_sub == 'f')
		{
			megatek_data[2] &= 0x0f;

			diag_num = 60;
		#ifdef HAVE_DIAGS
			my_diag("insert stroke-refresh color\n");
		#endif
			an_element.op_code = 5;
			an_element.arg1 = megatek_data[2];

			insert_ours(an_element,which_cseg);
			
		}
		else
		{
			megatek_data[2] &= 0x0f;
	
			diag_num = 60;
		#ifdef HAVE_DIAGS
			my_diag("insert stroke-refresh color\n");
		#endif
			an_element.op_code = 5;
			an_element.arg1 = megatek_data[2];

			insert_sub(an_element,which_sub);
		}
	}

}



void far left_curly(sub_count,which_subr)  /* read segment starting address */
int *sub_count,which_subr;
{
int i;
unsigned int address;
char first,second,third;


	read_more(2);
	
	diag_num = 67;

	a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);
#ifdef HAVE_DIAGS
	sprintf(diag_string,"read segment starting address: %d\n",a_segment);
	my_diag(diag_string);
#endif
	for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
		if(segment_nums[i] == a_segment)
			break;

	if(i == NUMBER_OF_SEGS)
	{
		sprintf(diag_string,"no such segment as %d in read segment start address",
				a_segment);
		error_logger(diag_string);

		my_ugout(0x1d);
		my_ugout(0x20);
		my_ugout(0x43);
		my_ugout(0x44);
		my_ugout(0x0d);

		return;
	}

	address = i * 3333;

	first = address >> 12;
	if(segment_visibility[i] = 't')
		first |= 32;

	second = (address >> 6) & 0x003f;
	third = address & 0x003f;

	my_ugout(0x1d);
	my_ugout(first | 0x40);
	my_ugout(second | 0x40);
	my_ugout(third | 0x40);
	my_ugout(0x0d);
	
}






void far cap_I_func(sub_count,which_subr)  /* change something */
int *sub_count,which_subr;
{
int i;
long save_insert_ptr;


	read_more(5);

		diag_num = 83;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;
		if(i == NUMBER_OF_SEGS)
			return;


		if((megatek_data[2] & 32) != 0) /*change dash pattern*/
		{
			dash_pat = comp16_3(megatek_data[2],megatek_data[3],
				megatek_data[4]);

			if(i == which_cseg)
			{
				my_dash_pattern(dash_pat);
			}
	
			update_header_data.dash_pattern = dash_pat;
			update_a_header(update_header_data,THE_DASH_PATTERN,i);


		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change dash pattern in segment %d: %x %x %x  %x\n",
				a_segment,megatek_data[2],megatek_data[3],
					megatek_data[4],dash_pat);
			my_diag(diag_string);
		#endif
		}
		else  /*change stroke-refresh color*/
		{
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"change stroke-refresh color in segment %d\n",
					a_segment);
			my_diag(diag_string);
		#endif
		}
	
}





void far cap_H_func(sub_count,which_subr)  /* change something */
int *sub_count,which_subr;
{
int i;
long save_insert_ptr;

	read_more(3);

		diag_num = 84;

		a_segment = comp_rel_coord(megatek_data[0],megatek_data[1]);

		for(i = 0 ; i < NUMBER_OF_SEGS ; i++)
			if(segment_nums[i] == a_segment)
				break;

		if(i == NUMBER_OF_SEGS)
		{
			sprintf(diag_string,"Invalid segment # %d in change something in a segment\n",
					a_segment);
			error_logger(diag_string);
			return;
		}

	#ifdef HAVE_DIAGS
		sprintf(diag_string,"change something in segment %d\n",a_segment);
		my_diag(diag_string);
	#endif
		diag_num = - 84;



		if((megatek_data[2] & 32) != 0)
		{
			if((megatek_data[2] & 16) != 0) 
			{
				if((megatek_data[2] & 8) != 0) /*change visibility*/
				{
					an_element.op_code = 110;

					if((megatek_data[2] & 1) != 0) /*is visible*/
					{
					#ifdef HAVE_DIAGS
						my_diag("is visible\n");
					#endif
						if(segment_visibility[i]
						   == 'f')
						      visibility_changed_to_on[
							  i] = 't';
						else
						      visibility_changed_to_on[
							  i] = 'f';


					/*
						segment_visibility[i] = 't';
					*/
					}
					else /*segment is not visible*/
					{
					#ifdef HAVE_DIAGS
						my_diag("segment is not visible\n");
					#endif
						segment_visibility[i] = 'f';
						an_element.flag1 = 'f';
						visibility_changed_to_on[i] = 
							'f';
					}
	

				}
				else  /*change transformation enable*/
				{
					an_element.op_code = 111;

					if((megatek_data[2] & 1) == 0)
					{  /*HCRST is enabled*/
					#ifdef HAVE_DIAGS
						my_diag("HCRST is enabled\n");
					#endif
						an_element.flag1 = 't';
					}
					else  /*HCRST is disabled*/
					{
					#ifdef HAVE_DIAGS
						my_diag("HCRST is disabled\n");
					#endif

						an_element.flag1 = 'f';
					}

				}
			}
			else
			{
				if((megatek_data[2] & 8) != 0) /*change dash enable*/
				{
					an_element.op_code = 117;

					if((megatek_data[2] & 1) != 0) /*enable dashing*/
					{
					#ifdef HAVE_DIAGS
						my_diag("dash enabled\n");
					#endif

						an_element.flag1 = 't';
						my_dash_enable(4);
						line_type = 4;
					}
					else  /*disable dashing*/
					{
					#ifdef HAVE_DIAGS
						my_diag("dash disabled\n");
					#endif

						an_element.flag1 = 'f';
						my_dash_enable(0);
						line_type = 0;
					}
	

					update_header_data.dash_enable = 
						an_element.flag1;
					update_a_header(update_header_data,
						THE_DASH_ENABLE,i);

				#ifdef HAVE_DIAGS
					my_diag("change dash enable\n");
				#endif
				}

				else  /*change blink enable*/
				{
				#ifdef HAVE_DIAGS
					my_diag("change blink enable\n");
				#endif
				}
			}
		}
		else
		{
			if((megatek_data[2] & 16) != 0)  /*change blink rate*/
			{
			#ifdef HAVE_DIAGS
				my_diag("change blink rate\n");
			#endif
			}
			else  /*change intensity/color*/
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"change intensity/color %d\n",
					megatek_data[2] & 0x0f);
				my_diag(diag_string);
			#endif
				my_color(megatek_data[2] & 0x0f);
			}
		}

}






void far o_func(sub_count,which_subr)  /* read/write vector memory */
int *sub_count,which_subr;
{
unsigned int most_sig,least_sig,i,j,k,l,check_sum = 0;
char first,second,third;


	read_more(3);


	for(l = 0 ; l < 3 ; l++)
		check_sum += megatek_data[l] & 0x3f;


	diag_num = 70;

	i = comp16_3(megatek_data[0],megatek_data[1],
		megatek_data[2]);

	if((megatek_data[0] & 32) != 0)  /*write at insert pointer*/
	{

		read_more(3);
	

		j = comp16_3(megatek_data[0],megatek_data[1],
				megatek_data[2]);
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"write at insert pointer: most sig data = %u least sig data = %u\n",i,j);
		my_diag(diag_string);
	#endif
		for(l = 0 ; l < 3 ; l++)
			check_sum += megatek_data[l] & 0x3f;
		
		if(fill_sub == 'f') /*write at insert pointer into a segment*/
		{
			an_element.op_code = 50;
			an_element.arg1 = i;
			an_element.arg2 = j;

			insert_ours(an_element,which_cseg);

		}
		else   /*write at insert pointer into a subroutine*/
		{
			if(i == 57358)
			{
				if(j == 65535)
					an_element.op_code = 60;
				else if(j == 65520)
					an_element.op_code = 70;
				else
				{
					an_element.op_code = 50;
					an_element.arg1 = i;
					an_element.arg2 = j;
				}
			}
			else
			{
				an_element.op_code = 50;
				an_element.arg1 = i;
				an_element.arg2 = j;
			}

			insert_sub(an_element,which_sub);
		}


                switch(check_resp)
                {
                        case 0 : /*real checksums*/
                        {
                  		my_ugout(0x1d);
		                my_ugout((check_sum % 63) | 0x40);
		                my_ugout(0x0d);
                                break;
                        }

                        case 1 : /*fixed checksums = 0x41*/
                        {
                                my_ugout(0x1d);
                                my_ugout(0x41);
                                my_ugout(0x0d);
                                break;
                        }

                        case 2 : break;
                }


	}
	else if((megatek_data[0] & 16) != 0) /*write specified address*/
	{

		read_more(6);
	

			j = comp16_3(megatek_data[0],megatek_data[1],
				megatek_data[2]);

			k = comp16_3(megatek_data[3],megatek_data[4],
				megatek_data[5]);
		#ifdef HAVE_DIAGS
			sprintf(diag_string,"write specified address: address = %u most sig data = %u least sig data = %u\n",i,j,k);
			my_diag(diag_string);
		#endif

		switch(i)
		{
			case 512 :
			{
				vm512.most_sig = j;
				vm512.least_sig = k;
				break;
			}

			case 345 :
			{
				vm345.most_sig = j;
				vm345.least_sig = k;
				break;
			}

			case 33536 :
			{
				vm33536.most_sig = j;
				vm33536.least_sig = k;
				break;
			}

			case 49536 :
			{
				vm49536.most_sig = j;
				vm49536.least_sig = k;
				break;
			}

			case 1170 :
			{
				vm1170.most_sig = j;
				vm1170.least_sig = k;
				break;
			}

			case 1166 :
			{
				vm1166.most_sig = j;
				vm1166.least_sig = k;
				break;
			}

			case 69 :
			{
				old_x = comp_coord(j & 0x0fff) + 1;
				old_y = comp_coord(k & 0x0fff) + 1;

				last_mouse_x = joy_x_sample = mouse_x = new_x 
					= old_x;

				last_mouse_y = joy_y_sample = mouse_y = new_y 
					= last_neg_mouse_y = old_y;



				vm69.most_sig = j;
				vm69.least_sig = k;
				break;
			}

			case 7 :
			{
				vm7.most_sig = j;
				vm7.least_sig = k;
				break;
			}

			default :
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"Unknown address in write specified address %u\n",
					i);
				my_diag(diag_string);
			#endif
			}
		}

		for(l = 0 ; l < 6 ; l++)
			check_sum += megatek_data[l] & 0x3f;



                switch(check_resp)
                {
                        case 0 : /*real checksums*/
                        {
                  		my_ugout(0x1d);
		                my_ugout((check_sum % 63) | 0x40);
		                my_ugout(0x0d);
                                break;
                        }

                        case 1 : /*fixed checksums = 0x41*/
                        {
                                my_ugout(0x1d);
                                my_ugout(0x41);
                                my_ugout(0x0d);
                                break;
                        }

                        case 2 : break;
                }


	}
	else  /*read vector memory*/
	{
	#ifdef HAVE_DIAGS
		sprintf(diag_string,"read vector memory: address = %u\n",i);
		my_diag(diag_string);
	#endif
		switch(i)
		{
			case 512 :
			{
				most_sig = vm512.most_sig;
				least_sig = vm512.least_sig;
				break;
			}

			case 345 :
			{
				most_sig = vm345.most_sig;
				least_sig = vm345.least_sig;
				break;
			}

			case 33536 :
			{
				most_sig = vm33536.most_sig;
				least_sig = vm33536.least_sig;
				break;
			}

			case 49536 :
			{
				most_sig = vm49536.most_sig;
				least_sig = vm49536.least_sig;
				break;
			}

			case 1170 :
			{
				most_sig = vm1170.most_sig;
				least_sig = vm1170.least_sig;
				break;
			}

			case 1166 :
			{
				most_sig = vm1166.most_sig;
				least_sig = vm1166.least_sig;
				break;
			}

			case 69 :
			{
				most_sig = vm69.most_sig;
				least_sig = vm69.least_sig;
				break;
			}

			case 7 :
			{
				most_sig = vm7.most_sig;
				least_sig = vm7.least_sig;
				break;
			}

			default : 
			{
			#ifdef HAVE_DIAGS
				sprintf(diag_string,"Unknown address in read vector memory %u\n",
						i);
				my_diag(diag_string);
			#endif
			}
		}

		my_ugout(0x1d);

		first = most_sig >> 12;
		second = (most_sig >> 6) & 0x003f;
		third = most_sig & 0x003f;

		my_ugout(first | 0x60);
		my_ugout(second | 0x40);
		my_ugout(third | 0x40);

		first = least_sig >> 12;
		second = (least_sig >> 6) & 0x003f;
		third = least_sig & 0x003f;

		my_ugout(first | 0x40);
		my_ugout(second | 0x40);
		my_ugout(third | 0x40);

		my_ugout(0x0d);


	}

}

